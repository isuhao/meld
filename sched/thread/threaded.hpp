
#ifndef SCHED_THREAD_THREADED_HPP
#define SCHED_THREAD_THREADED_HPP

#include <boost/thread/mutex.hpp>
#include <boost/thread/barrier.hpp>
#include <vector>

#include "utils/types.hpp"
#include "utils/spinlock.hpp"
#include "sched/base.hpp"
#include "sched/thread/termination_barrier.hpp"

namespace sched
{

class threaded
{
private:
   
   static boost::barrier *thread_barrier;
   static termination_barrier *term_barrier;
   
   volatile enum {
      THREAD_ACTIVE,
      THREAD_INACTIVE
   } state;
   
   utils::byte _pad_threaded[128];
   
protected:
   
	utils::spinlock lock;
   
   static std::vector<sched::base*> ALL_THREADS;
   
   static inline void add_thread(sched::base *add)
   {
      ALL_THREADS[add->get_id()] = add;
   }
   
   static inline void threads_synchronize(void)
   {
      thread_barrier->wait();
   }
   
   static void init_barriers(const size_t);

   inline void set_active(void)
   {
      assert(state == THREAD_INACTIVE);
      state = THREAD_ACTIVE;
      term_barrier->is_active();
   }
   
   inline void set_inactive(void)
   {
      assert(state == THREAD_ACTIVE);
      state = THREAD_INACTIVE;
      term_barrier->is_inactive();
   }
   
   inline void set_active_if_inactive(void)
   {
      if(is_inactive()) {
			utils::spinlock::scoped_lock l(lock);
         if(is_inactive())
            set_active();
      }
   }
   
   inline const bool is_inactive(void) const { return state == THREAD_INACTIVE; }
   inline const bool is_active(void) const { return state == THREAD_ACTIVE; }
   
   inline const bool all_threads_finished(void) const
   {
      return term_barrier->all_finished();
   }
   
public:
   
   explicit threaded(void): state(THREAD_ACTIVE)
   {
   }
   
   virtual ~threaded(void)
   {
      assert(state == THREAD_INACTIVE);
   }
};

#define DEFINE_START_FUNCTION(CLASS)                  \
   static std::vector<sched::base*>&                  \
   start(const size_t num_threads)                    \
   {                                                  \
      init_barriers(num_threads);                     \
      for(vm::process_id i(0); i < num_threads; ++i)  \
         add_thread(new CLASS(i));                    \
      assert_thread_disable_work_count();             \
      return ALL_THREADS;                             \
   }

#define BUSY_LOOP_MAKE_INACTIVE()         \
   if(is_active() && !has_work()) {       \
      spinlock::scoped_lock l(lock);      \
      if(!has_work()) {                   \
         if(is_active())                  \
            set_inactive();               \
      } else                              \
         break;                           \
   }
   
#define BUSY_LOOP_CHECK_TERMINATION_THREADS()   \
   if(!has_work() && is_inactive() && all_threads_finished()) {      \
      assert(!has_work());                                           \
      assert(is_inactive());                                         \
      assert(all_threads_finished());                                \
      return false;                                                  \
   }

}

#endif
