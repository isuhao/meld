#include <iostream>
#include <boost/thread/barrier.hpp>

#include "thread/prio.hpp"
#include "db/database.hpp"
#include "db/tuple.hpp"
#include "process/remote.hpp"
#include "sched/thread/assert.hpp"
#include "vm/state.hpp"
#include "sched/common.hpp"
#include "utils/time.hpp"

using namespace boost;
using namespace std;
using namespace process;
using namespace vm;
using namespace db;
using namespace utils;

//#define DEBUG_PRIORITIES
//#define PROFILE_QUEUE

#ifdef PROFILE_QUEUE
static atomic<size_t> prio_count(0);
static atomic<size_t> prio_immediate(0);
static atomic<size_t> prio_marked(0);
static atomic<size_t> prio_moved_pqueue(0);
static atomic<size_t> prio_add_pqueue(0);
static atomic<size_t> prio_removed_pqueue(0);
static atomic<size_t> prio_nodes_compared(0);
static atomic<size_t> prio_nodes_changed(0);
#endif

namespace sched
{

void
threads_prio::assert_end(void) const
{
   assert(priority_buffer.empty());
	static_local::assert_end();
}

void
threads_prio::assert_end_iteration(void) const
{
	static_local::assert_end_iteration();
   assert(priority_buffer.empty());
}

#ifdef TASK_STEALING
void
threads_prio::check_stolen_nodes(void)
{
   while(!stolen_nodes_buffer.empty()) {
      thread_intrusive_node *n(stolen_nodes_buffer.pop());

      assert(n->in_queue());

#ifdef INSTRUMENTATION
      stolen_total++;
#endif
			
		// nodes may have been added
		if(priority_queue::in_queue(n))
			continue;
		if(node_queue::in_queue(n))
			continue;

      assert(n->moving_around);

		if(n->has_priority_level()) {
         assert(!priority_queue::in_queue(n));
			prio_queue.insert(n, n->get_priority_level());
		} else {
      	queue_nodes.push_tail(n);
		}
      n->moving_around = false;
		assert(n->in_queue());
   }
}

void
threads_prio::answer_steal_requests(void)
{
   bool flag(true);

   while(!steal_request_buffer.empty() && (!queue_nodes.empty() || !prio_queue.empty())) {
      threads_prio *target((threads_prio*)steal_request_buffer.pop());
      thread_intrusive_node *node(NULL);

      size_t size(queue_nodes.size() + prio_queue.size());
      const size_t frac((int)((double)size * (double)state::TASK_STEALING_FACTOR));
      size = max(min(size, (size_t)4), frac);

      while(size > 0 && (!queue_nodes.empty() || !prio_queue.empty())) {
         node = NULL;

         if(flag) {
            if(!queue_nodes.empty())
               queue_nodes.pop(node);
            else if(!prio_queue.empty())
               node = prio_queue.pop();
            flag = false;
         } else {
            if(!prio_queue.empty())
               node = prio_queue.pop();
            else if(!queue_nodes.empty())
               queue_nodes.pop(node);
            flag = true;
         }
		
         if(node == NULL)
            continue;

         assert(node != NULL);
         assert(node != current_node);
         assert(node->get_owner() == this);
         assert(node->in_queue());

         node->moving_around = true;
         node->set_owner(target);
         target->stolen_nodes_buffer.push(node);
         size--;
      }

      spinlock::scoped_lock l(target->lock);
   
      if(target->is_inactive() && target->has_work())
      {
         target->set_active();
      }
   }
}
#endif

void
threads_prio::check_priority_buffer(void)
{
#if 0
   while(!priority_buffer.empty()) {
      priority_buffer.pop();
   }

   return;
#else

   while(!priority_buffer.empty()) {
      priority_add_item p(priority_buffer.pop());
      node *target(p.target);
      thread_intrusive_node *tn((thread_intrusive_node *)target);
      int howmuch(p.val);
      priority_add_type typ(p.typ);

		if(tn->get_owner() != this) {
         threads_prio *other((threads_prio*)tn->get_owner());
         other->priority_buffer.push(p);
			continue; // skip nodes we do not own
      }

      node_priorities::iterator it(node_map.find(target));
      if (it == node_map.end()) {
         switch(typ) {
            case ADD_PRIORITY:
               node_map[target] = tn->get_int_priority_level() + howmuch;
               break;
            case SET_PRIORITY:
               node_map[target] = howmuch;
               break;
            default:
               assert(false);
         }
      } else {
         const int oldval(it->second);
         switch(typ) {
            case ADD_PRIORITY:
               node_map[target] = oldval + oldval;
               break;
            case SET_PRIORITY:
               node_map[target] = howmuch;
               break;
         }
      }
   }

   // go through the map and set the priority
   for(node_priorities::iterator it(node_map.begin()), end(node_map.end());
         it != end;
         ++it)
   {
      node *target(it->first);
      const int priority(it->second);

      set_node_priority(target, priority);
   }

   node_map.clear();
#endif
}

void
threads_prio::retrieve_prio_tuples(void)
{
	while(!prio_tuples.empty()) {
		work new_work(prio_tuples.pop());
		thread_intrusive_node *to(dynamic_cast<thread_intrusive_node*>(new_work.get_node()));
		threads_prio *owner((threads_prio *)to->get_owner());
		
		if(owner == this) {
			add_prio_tuple(new_work, to, new_work.get_tuple());
		} else {
			owner->prio_tuples.push(new_work);
			
			spinlock::scoped_lock l(owner->lock);
      
         if(owner->is_inactive() && owner->has_work())
         {
            owner->set_active();
            assert(owner->is_active());
         }
		}
	}
}

void
threads_prio::new_agg(work& new_work)
{
   thread_intrusive_node *to(dynamic_cast<thread_intrusive_node*>(new_work.get_node()));
   
   assert_thread_push_work();
   
   to->add_work(new_work.get_tuple());
   
   if(!to->in_queue()) {
      to->set_in_queue(true);
		add_to_queue(to);
   }
}

void
threads_prio::add_prio_tuple(work new_work, thread_intrusive_node *to, db::simple_tuple *stpl)
{
	const field_num field(state::PROGRAM->get_priority_argument());
	vm::tuple *tpl(stpl->get_tuple());
	heap_priority valp;
	const bool has_prio_tuples(!to->prioritized_tuples.empty());
	tuple_field val(tpl->get_field(field));
	heap_priority min_before;

	if(has_prio_tuples)
		min_before = to->get_min_value();
	
	switch(priority_type) {
		case HEAP_INT_ASC:
		case HEAP_INT_DESC:
			valp.int_priority = val.int_field;
			break;
		case HEAP_FLOAT_ASC:
		case HEAP_FLOAT_DESC:
			valp.float_priority = val.float_field;
			break;
		default: assert(false);
	}

	to->prioritized_tuples.insert(new_work.get_tuple(), valp);

	if(to != current_node) {
		spinlock::scoped_lock l(to->spin);
			
		if(priority_queue::in_queue(to)) {
			assert(!node_queue::in_queue(to));
				
			const heap_priority min_now(to->get_min_value());
				
			assert(has_prio_tuples);

			bool different_priority(false);

			switch(priority_type) {
				case HEAP_INT_DESC:
				case HEAP_INT_ASC: different_priority = min_now.int_priority != min_before.int_priority; break;
				case HEAP_FLOAT_DESC:
				case HEAP_FLOAT_ASC: different_priority = min_now.float_priority != min_before.float_priority; break;
				default: assert(false);
			}
				
			if(different_priority) {
#ifdef PROFILE_QUEUE
				prio_moved_pqueue++;
#endif
            to->set_priority_level(min_now);
				prio_queue.move_node(to, min_now);
			}
		} else {
			if(node_queue::in_queue(to)) {
#ifdef PROFILE_QUEUE
				prio_removed_pqueue++;
#endif
				queue_nodes.remove(to);
			}
			assert(!node_queue::in_queue(to));
				
#ifdef PROFILE_QUEUE
			prio_add_pqueue++;
#endif
         //cout << "Add " << to->get_id() << " with " << to->get_min_value().int_priority << endl;
         to->set_priority_level(to->get_min_value());
			prio_queue.insert(to, to->get_min_value());
		}
			
		assert(priority_queue::in_queue(to));
		assert(!node_queue::in_queue(to));
			
		if(!to->in_queue())
			to->set_in_queue(true);
			
		assert(to->in_queue());
	}
}

void
threads_prio::new_work(const node *, work& new_work)
{
   thread_intrusive_node *to(dynamic_cast<thread_intrusive_node*>(new_work.get_node()));
   
   assert_thread_push_work();
   
	db::simple_tuple *stpl(new_work.get_tuple());
	const vm::predicate *pred(stpl->get_predicate());
	
	if(pred->is_global_priority()) {
		add_prio_tuple(new_work, to, stpl);
	} else {
   	to->add_work(new_work.get_tuple());
		if(!to->in_queue()) {
  			to->set_in_queue(true);
			add_to_queue(to);
	   }   
   }

	assert(to->in_queue());
}

void
threads_prio::new_work_other(sched::base *, work& new_work)
{
   assert(is_active());
   
   thread_intrusive_node *tnode(dynamic_cast<thread_intrusive_node*>(new_work.get_node()));
   
   assert_thread_push_work();
   
   node_work node_new_work(new_work);
	db::simple_tuple *stpl(node_new_work.get_tuple());
	const vm::predicate *pred(stpl->get_predicate());
	
	if(pred->is_global_priority()) {
		threads_prio *owner(dynamic_cast<threads_prio*>(tnode->get_owner()));
		
		owner->prio_tuples.push(new_work);
		
   	if(this != owner) {
      	spinlock::scoped_lock l2(owner->lock);
         
      	if(owner->is_inactive() && owner->has_work())
      	{
         	owner->set_active();
         	assert(owner->is_active());
      	}
   	} else {
      	assert(is_active());
   	}
	} else {
      threads_prio *owner(dynamic_cast<threads_prio*>(tnode->get_owner()));

      owner->buffer.push(new_work);

   	if(this != owner) {
      	spinlock::scoped_lock l2(owner->lock);
         
      	if(owner->is_inactive() && owner->has_work())
      	{
         	owner->set_active();
         	assert(owner->is_active());
      	}
   	} else {
      	assert(is_active());
   	}
	}
   
#ifdef INSTRUMENTATION
   sent_facts++;
#endif
}

void
threads_prio::new_work_remote(remote *, const node::node_id, message *)
{
   assert(false);
}

void
threads_prio::generate_aggs(void)
{
   iterate_static_nodes(id);
}

bool
threads_prio::busy_wait(void)
{
#ifdef TASK_STEALING
   ins_sched;
   make_steal_request();
   size_t count(0);
#endif
   
   ins_idle;

   while(!has_work()) {
#ifdef TASK_STEALING
      check_stolen_nodes();
      ++count;
      if(count == STEALING_ROUND_MAX) {
         ins_sched;
         make_steal_request();
         ins_idle;
         count = 0;
      }
#endif
      retrieve_tuples();
   	retrieve_prio_tuples();
      check_priority_buffer();
      BUSY_LOOP_MAKE_INACTIVE()
      BUSY_LOOP_CHECK_TERMINATION_THREADS()
	}
	
   // since queue pushing and state setting are done in
   // different exclusive regions, this may be needed
   set_active_if_inactive();
   ins_active;
   assert(is_active());
   assert(has_work());
   
   return true;
}

bool
threads_prio::terminate_iteration(void)
{
   START_ROUND();
   
   if(has_work())
      set_active();
   
   END_ROUND(
      more_work = num_active() > 0;
   );
}

void
threads_prio::finish_work(db::node *no)
{
   base::finish_work(no);
   
   assert(current_node != NULL);
   assert(current_node->in_queue());
   assert(current_node->get_owner() == this);
}

bool
threads_prio::check_if_current_useless(void)
{
	assert(current_node->in_queue());
	
	if(!state::PROGRAM->has_global_priority() &&
         !prio_queue.empty() && taken_from_priority_queue && !current_node->has_work())
   {
		// there could be higher priority nodes around
		const heap_priority current_min(prio_queue.min_value());

		if(current_min.int_priority < current_node->get_int_priority_level()) {
			// put this node back into the priority queue and use the higher priority one
#ifdef DEBUG_PRIORITIES
			//cout << "Node in queue has higher priority: " << current_min.int_priority << " vs " << node_pr.int_priority << endl;
#endif
         assert(!priority_queue::in_queue(current_node));
			prio_queue.insert(current_node, current_node->get_priority_level());
			current_node = prio_queue.pop();
         taken_from_priority_queue = true;
		}
#if 0
   } else if(state::PROGRAM->has_global_priority() && !prio_queue.empty() && !current_node->has_prio_work() && current_node->has_work()) {
      queue_nodes.push_tail(current_node);
      current_node = prio_queue.pop();
      taken_from_priority_queue = true;
		assert(current_node->in_queue());
		assert(current_node->has_work());
      return false;
#endif
	} else if(state::PROGRAM->has_global_priority() && !prio_queue.empty() && current_node->has_prio_work()) {
		const heap_priority current_min(prio_queue.min_value());
		const heap_priority node_min(current_node->get_min_value());

      assert(state::PROGRAM->has_global_priority());
		
#ifdef PROFILE_QUEUE
		prio_nodes_compared++;
#endif

		bool is_current_best(false);

		switch(priority_type) {
			case HEAP_INT_ASC: is_current_best = (current_min.int_priority < node_min.int_priority); break;
			case HEAP_INT_DESC: is_current_best = (current_min.int_priority > node_min.int_priority); break;
			case HEAP_FLOAT_ASC: is_current_best = (current_min.float_priority < node_min.float_priority); break;
			case HEAP_FLOAT_DESC: is_current_best = (current_min.float_priority > node_min.float_priority); break;
			default: assert(false);
		}
		
		if(is_current_best) {
			// put back into priority queue
			prio_queue.insert(current_node, node_min);
			assert(priority_queue::in_queue(current_node));
			current_node = prio_queue.pop();
         //cout << "Changing to node " << current_node->get_id() << " with priority " << current_min.int_priority << endl; 
         taken_from_priority_queue = true;
#ifdef PROFILE_QUEUE
			prio_nodes_changed++;
#endif
		}
		
		assert(current_node->in_queue());
		assert(current_node->has_work());
		return false;
	}
	
   if(!current_node->has_work()) {
      //spinlock::scoped_lock lock(current_node->spin);
      
      if(!node_queue::in_queue(current_node)) {
         current_node->set_in_queue(false);
         switch(state::PROGRAM->get_priority_type()) {
            case FIELD_INT:
               current_node->set_int_priority_level(0);
               break;
            case FIELD_FLOAT:
               current_node->set_float_priority_level(0.0);
               break;
            default:
               assert(false);
               break;
         }
         assert(!current_node->in_queue());
      }
      current_node = NULL;
      return true;
   }
   
   assert(current_node->has_work());
   return false;
}

//#define USE_DELTA_ADJUST
#define ROUND 1000

#ifdef ROUND
static size_t next_round(ROUND);
#endif

bool
threads_prio::set_next_node(void)
{
#ifdef USE_DELTA_ADJUST
   if(id == 0) {
      next_round--;
      if(next_round == 0) {
         next_round = ROUND;
         if(prio_queue.empty()) {
            const float_val cur(state::get_const_float(1));
            if(cur > 0.5) {
               const float_val up(cur / 2.0);
               //cout << "Current " << cur << " new " << up << endl;
               state::set_const_float(1, up);
            }
         }
      }
   }
#endif

   if(current_node != NULL)
      check_if_current_useless();
   
   while (current_node == NULL) {   
#ifdef TASK_STEALING
      check_stolen_nodes();
#endif
      retrieve_tuples();
		retrieve_prio_tuples();
      check_priority_buffer();
		
      if(!has_work()) {
         if(!busy_wait())
            return false;
      }
      
      assert(has_work());

#ifdef DO_ONE_PASS_FIRST
      if(to_takeout > 0) {
         const bool suc(queue_nodes.pop(current_node));
         if(!suc)
            continue;
         taken_from_priority_queue = false;
      } else
#endif
		if(!prio_queue.empty()) {
			current_node = prio_queue.pop();
         //cout << "Picked node " << current_node->get_id() << " with " << current_node->get_int_priority_level() << endl;
         taken_from_priority_queue = true;
			goto loop_check;
		} else if(!queue_nodes.empty()) {
			const bool suc(queue_nodes.pop(current_node));
			if(!suc)
				continue;
         taken_from_priority_queue = false;
		} else
         continue;

loop_check:
      assert(current_node != NULL);
      assert(current_node->in_queue());

#ifdef DO_ONE_PASS_FIRST
      if(!current_node->has_been_touched) {
         --to_takeout;
         current_node->has_been_touched = true;
      }
#endif
      
      check_if_current_useless();
   }
   
   ins_active;
   
   assert(current_node != NULL);
   
   return true;
}

node*
threads_prio::get_work(void)
{
	retrieve_prio_tuples();
   check_priority_buffer();
	
   if(!set_next_node())
      return NULL;

#ifdef TASK_STEALING
   answer_steal_requests();
#endif
      
   set_active_if_inactive();
   assert(current_node != NULL);
   assert(current_node->in_queue());
   assert(current_node->has_work());
   
   return current_node;
}

void
threads_prio::end(void)
{
#if defined(DEBUG_PRIORITIES) && defined(PROFILE_QUEUE)
	cout << "prio_immediate: " << prio_immediate << endl;
	cout << "prio_marked: " << prio_marked << endl;
	cout << "prio_count: " << prio_count << endl;
#endif
#ifdef PROFILE_QUEUE
	if(state::PROGRAM->has_global_priority()) {
		cout << "Moved nodes in priority queue count: " << prio_moved_pqueue << endl;
		cout << "Removed from normal queue count: " << prio_removed_pqueue << endl;
		cout << "Added to priority queue count: " << prio_add_pqueue << endl;
		cout << "Changed nodes count: " << prio_nodes_changed << endl;
		cout << "Compared nodes count: " << prio_nodes_compared << endl;
	}
#endif
	
#if defined(DEBUG_PRIORITIES)
	size_t total_prioritized(0);
	size_t total_nonprioritized(0);
	
   database::map_nodes::iterator it(state::DATABASE->get_node_iterator(remote::self->find_first_node(id)));
   database::map_nodes::iterator end(state::DATABASE->get_node_iterator(remote::self->find_last_node(id)));
   
   for(; it != end; ++it)
   {
      thread_intrusive_node *cur_node((thread_intrusive_node*)it->second);
		
		if(cur_node->has_been_prioritized)
			++total_prioritized;
		else
			++total_nonprioritized;
	}
	
	cout << "Number of prioritized nodes: " << total_prioritized << endl;
	cout << "Number of non prioritized nodes: " << total_nonprioritized << endl;
#endif
}

void
threads_prio::add_node_priority_other(node *n, const int priority)
{
   // this is called by the other scheduler!
   priority_add_item item;
   item.typ = ADD_PRIORITY;
   item.val = priority;
   item.target = n;
   priority_buffer.push(item);
}

void
threads_prio::set_node_priority_other(node *n, const int priority)
{
   // this is called by the other scheduler!
   priority_add_item item;
   item.typ = SET_PRIORITY;
   item.val = priority;
   item.target = n;
   priority_buffer.push(item);
}

void
threads_prio::add_node_priority(node *n, const int priority)
{
	thread_intrusive_node *tn((thread_intrusive_node*)n);

	const int old_prio(tn->get_int_priority_level());

	set_node_priority(n, old_prio + priority);
}

void
threads_prio::set_node_priority(node *n, const int priority)
{
	thread_intrusive_node *tn((thread_intrusive_node*)n);

#ifdef DO_ONE_PASS_FIRST
   if(!tn->has_been_touched) {
#ifdef PROFILE_QUEUE
		prio_marked++;
#endif
		assert(tn != current_node);
      if(priority != 0) {
         tn->set_priority_level(priority);
         tn->mark_priority();
      }
      return;
   }
#endif
	
#ifdef DEBUG_PRIORITIES
   if(priority > 0)
      tn->has_been_prioritized = true;
#endif

   if(current_node == tn) {
      if(priority == 0)
         taken_from_priority_queue = false;
      else
         taken_from_priority_queue = true;
      tn->set_int_priority_level(priority);
      return;
   }

	assert(tn->get_owner() == this);
	
	if(tn->in_queue() && !tn->moving_around) {
#ifdef PROFILE_QUEUE
      if(priority > 0)
         prio_immediate++;
#endif
		if(priority_queue::in_queue(tn)) {
         // node is in the priority queue
         if(tn->get_int_priority_level() != priority) {
            if(priority == 0) {
               tn->set_int_priority_level(0);
               prio_queue.remove(tn);
               queue_nodes.push_tail(tn);
            } else {
               // priority > 0
               assert(priority > 0);
               heap_priority pr;
               pr.int_priority = priority;
#ifdef DEBUG_PRIORITIES
               //cout << "Changing node priority " << tn->get_id() << " (" << tn->get_priority_level() << ") to" << priority << endl;
#endif
               tn->set_int_priority_level(priority);
					assert(tn->in_queue());
               prio_queue.move_node(tn, pr);
            }
			}
		} else {
         // node is in the normal queue
         if(priority > 0) {
            tn->set_int_priority_level(priority);
#ifdef DEBUG_PRIORITIES
            //cout << "Add node " << tn->get_id() << " with priority " << priority << endl;
#endif
            queue_nodes.remove(tn);
            assert(!priority_queue::in_queue(tn));
            add_to_priority_queue(tn);
         }
		}
		assert(tn->in_queue());
	} else {
#ifdef PROFILE_QUEUE
         prio_marked++;
#endif
      tn->set_int_priority_level(priority);
	}
	
#ifdef PROFILE_QUEUE
	prio_count++;
#endif
}

void
threads_prio::init(const size_t)
{
	if(state::PROGRAM->has_global_priority()) {
		predicate *p(state::PROGRAM->get_priority_predicate());
		const field_num field(state::PROGRAM->get_priority_argument());

		switch(p->get_field_type(field)) {
			case FIELD_INT:
				if(state::PROGRAM->is_global_priority_asc())
					priority_type = HEAP_INT_ASC;
				else
					priority_type = HEAP_INT_DESC;
				break;
			case FIELD_FLOAT:
				if(state::PROGRAM->is_global_priority_asc())
					priority_type = HEAP_FLOAT_ASC;
				else
					priority_type = HEAP_FLOAT_DESC;
				break;
			default:
				assert(false);
		}

	} else {
		// normal priorities
		priority_type = HEAP_INT_DESC;
	}

   prio_queue.set_type(priority_type);

   database::map_nodes::iterator it(state::DATABASE->get_node_iterator(remote::self->find_first_node(id)));
   database::map_nodes::iterator end(state::DATABASE->get_node_iterator(remote::self->find_last_node(id)));
   
#ifdef DO_ONE_PASS_FIRST
   to_takeout = 0;
#endif

   for(; it != end; ++it)
   {
      thread_intrusive_node *cur_node((thread_intrusive_node*)it->second);
      
      init_node(cur_node);

		cur_node->prioritized_tuples.set_type(priority_type);
      
      assert(cur_node->get_owner() == this);
      assert(cur_node->in_queue());
      assert(cur_node->has_work());

#ifdef DO_ONE_PASS_FIRST
      ++to_takeout;
#endif
   }
   
   threads_synchronize();
}

threads_prio*
threads_prio::find_scheduler(const node *n)
{
	thread_intrusive_node *tn((thread_intrusive_node*)n);
	
	return (threads_prio*)tn->get_owner();
}

simple_tuple_vector
threads_prio::gather_active_tuples(db::node *node, const vm::predicate_id pred)
{
	simple_tuple_vector ls;
	thread_intrusive_node *no((thread_intrusive_node*)node);
	predicate *p(state::PROGRAM->get_predicate(pred));
	
	typedef thread_node::queue_type fact_queue;
	
	if(p->is_global_priority()) {
		typedef thread_intrusive_node::prio_tuples_queue prio_queue;
		for(prio_queue::const_iterator it(no->prioritized_tuples.begin()),
				end(no->prioritized_tuples.end());
			it != end; ++it)
		{
			node_work w(*it);
			simple_tuple *stpl(w.get_tuple());
		
			if(stpl->can_be_consumed())
				ls.push_back(stpl);
		}
	} else {
		for(fact_queue::const_iterator it(no->queue.begin()), end(no->queue.end()); it != end; ++it) {
			node_work w(*it);
			simple_tuple *stpl(w.get_tuple());
		
			if(stpl->can_be_consumed() && stpl->get_predicate_id() == pred)
				ls.push_back(stpl);
		}
	}
	
	return ls;
}

void
threads_prio::gather_next_tuples(db::node *node, simple_tuple_list& ls)
{
   static_local::gather_next_tuples(node, ls);

	thread_intrusive_node *no((thread_intrusive_node*)node);

   if(no->get_local_strat_level() >= state::PROGRAM->get_priority_strat_level()) {
      while(!no->prioritized_tuples.empty()) {
         ls.push_back(no->prioritized_tuples.pop());
      }
   }
}

void
threads_prio::write_slice(statistics::slice& sl) const
{
#ifdef INSTRUMENTATION
   static_local::write_slice(sl);
   sl.priority_queue = prio_queue.size();
#else
   (void)sl;
#endif
}

threads_prio::threads_prio(const vm::process_id _id):
   static_local(_id)
{
}

threads_prio::~threads_prio(void)
{
}
   
vector<sched::base*>&
threads_prio::start(const size_t num_threads)
{
   init_barriers(num_threads);
   
   for(process_id i(0); i < num_threads; ++i)
      add_thread(new threads_prio(i));
      
   return ALL_THREADS;
}
   
}