
#ifndef STATE_HPP
#define STATE_HPP

#include "vm/tuple.hpp"
#include "db/node.hpp"
#include "vm/program.hpp"
#include "vm/instr.hpp"

// forward declaration
namespace process {
   class process;
   class remote;
   class machine;
   class router;
}

namespace db {
   class database;
}

namespace vm {

class state
{
private:
   
   static const size_t NUM_REGS = 32;
   typedef unsigned long int reg;
   
   reg regs[NUM_REGS];
   
public:
   
   vm::tuple *tuple;
   db::node *node;
   ref_count count;
   process::process *proc;
   
   static program *PROGRAM;
   static db::database *DATABASE;
   static process::machine *MACHINE;
   static process::remote *REMOTE;
   static process::router *ROUTER;
   static size_t NUM_THREADS;
   
#define define_get(WHAT, RET, BODY) \
   inline RET get_ ## WHAT (const reg_num& num) const { BODY; }
   
   define_get(reg, reg, return regs[num]);
   define_get(int, const int_val, return *(int_val*)(regs + num));
   define_get(float, const float_val, return *(float_val*)(regs + num));
   define_get(ptr, const ptr_val, return *(ptr_val*)(regs + num));
   define_get(bool, const bool_val, return get_int(num) ? true : false);
   define_get(int_list, runtime::int_list*, return (runtime::int_list*)get_ptr(num));
   define_get(float_list, runtime::float_list*, return (runtime::float_list*)get_ptr(num));
   define_get(node_list, runtime::node_list*, return (runtime::node_list*)get_ptr(num));
   define_get(tuple, vm::tuple*, return (vm::tuple*)get_ptr(num));
   define_get(node, vm::node_val, return *(node_val *)(regs + num));
   
#undef define_get

#define define_set(WHAT, ARG, BODY) \
   inline void set_ ## WHAT (const reg_num& num, ARG val) { BODY; };
   
   define_set(float, const float_val&, *(float_val*)(regs + num) = val);
   define_set(int, const int_val&, *(int_val*)(regs + num) = val);
   define_set(ptr, const ptr_val&, *(ptr_val*)(regs + num) = val);
   define_set(bool, const bool_val&, set_int(num, val ? 1 : 0));
   define_set(int_list, runtime::int_list*, set_ptr(num, (ptr_val)val));
   define_set(float_list, runtime::float_list*, set_ptr(num, (ptr_val)val));
   define_set(node_list, runtime::node_list*, set_ptr(num, (ptr_val)val));
   define_set(tuple, vm::tuple*, set_ptr(num, (ptr_val)val));
   define_set(node, const node_val, *(node_val*)(regs + num) = val);
   
#undef define_set
   
   inline void set_nil(const reg_num& num) { set_ptr(num, null_ptr_val); }
   
   inline void copy_reg(const reg_num& reg_from, const reg_num& reg_to) {
      regs[reg_to] = regs[reg_from];
   }
   
   explicit state(process::process *_proc):
      proc(_proc)
   {
   }
};

}

#endif