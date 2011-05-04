#include <iostream>
#include <algorithm>
#include <functional>
#include <cmath>

#include "vm/exec.hpp"
#include "vm/tuple.hpp"
#include "db/tuple.hpp"
#include "process/process.hpp"
#include "process/machine.hpp"

using namespace vm;
using namespace vm::instr;
using namespace std;
using namespace db;
using namespace runtime;

namespace vm
{
   
enum return_type {
   RETURN_OK,
   RETURN_SELECT,
   RETURN_NEXT
};

static inline return_type execute(pcounter, state&);

static inline node_val
get_node_val(pcounter& m, state& state)
{
   const node_val ret(pcounter_node(m));
   
   pcounter_move_node(&m);
   
   return ret;
}

static inline node_val
get_node_val(const pcounter& m, state& state)
{
   return pcounter_node(m);
}

static inline void
move_to_reg(const pcounter& m, state& state,
   const reg_num& reg, const instr_val& from)
{
   if(val_is_float(from))
      state.set_float(reg, pcounter_float(m));
   else if(val_is_int(from))
      state.set_int(reg, pcounter_int(m));
   else if(val_is_field(from)) {
      const field_num field(val_field_num(m));
      const reg_num from_reg(val_field_reg(m));
      const tuple *tuple(state.get_tuple(from_reg));
      
      switch(tuple->get_field_type(field)) {
         case FIELD_INT: state.set_int(reg, tuple->get_int(field)); break;
         case FIELD_FLOAT: state.set_int(reg, tuple->get_float(field)); break;
         case FIELD_LIST_INT: state.set_int_list(reg, tuple->get_int_list(field)); break;
         case FIELD_LIST_FLOAT: state.set_float_list(reg, tuple->get_float_list(field)); break;
         case FIELD_LIST_NODE: state.set_node_list(reg, tuple->get_node_list(field)); break;
         case FIELD_NODE: state.set_node(reg, tuple->get_node(field)); break;
      }
      
   } else if(val_is_host(from))
      state.set_node(reg, state.node->get_id());
   else if(val_is_node(from))
      state.set_node(reg, get_node_val(m, state));
   else if(val_is_reg(from))
      state.copy_reg(val_reg(from), reg);
   else if(val_is_tuple(from))
      state.set_tuple(reg, state.tuple);
   else {
      throw vm_exec_error("invalid move to reg");
   }
}

static inline void
move_to_field(pcounter m, state& state, const instr_val& from)
{
   if(val_is_float(from)) {
      const float_val flt(pcounter_float(m));
      
      pcounter_move_float(&m);
      
      tuple* tuple(state.get_tuple(val_field_reg(m)));
      
      tuple->set_float(val_field_num(m), flt);
   } else if(val_is_int(from)) {
      const int_val i(pcounter_int(m));
      
      pcounter_move_int(&m);
      
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      
      tuple->set_int(val_field_num(m), i);
   } else if(val_is_node(from)) {
      const node_val val(get_node_val(m, state));
      
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      
      tuple->set_node(val_field_num(m), val);
   } else if(val_is_field(from)) {
      const tuple *from_tuple(state.get_tuple(val_field_reg(m)));
      const field_num from_field(val_field_num(m));
      
      pcounter_move_field(&m);
      
      tuple *to_tuple(state.get_tuple(val_field_reg(m)));
      const field_num to_field(val_field_num(m));
      
      switch(to_tuple->get_field_type(to_field)) {
         case FIELD_INT:
            to_tuple->set_int(to_field, from_tuple->get_int(from_field));
            break;
         case FIELD_FLOAT:
            to_tuple->set_float(to_field, from_tuple->get_float(from_field));
            break;
         case FIELD_LIST_INT:
            to_tuple->set_int_list(to_field, from_tuple->get_int_list(from_field));
            break;
         case FIELD_LIST_FLOAT:
            to_tuple->set_float_list(to_field, from_tuple->get_float_list(from_field));
            break;
         case FIELD_LIST_NODE:
            to_tuple->set_node_list(to_field, from_tuple->get_node_list(from_field));
            break;
         case FIELD_NODE:
            to_tuple->set_node(to_field, from_tuple->get_node(from_field));
            break;
      }
   } else {
      tuple* tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      if(val_is_host(from))
         tuple->set_node(field, state.node->get_id());
      else if(val_is_reg(from)) {
         const reg_num reg(val_reg(from));
         
         switch(tuple->get_field_type(field)) {
            case FIELD_INT:
               tuple->set_int(field, state.get_int(reg));
               break;
            case FIELD_FLOAT:
               tuple->set_float(field, state.get_float(reg));
               break;
            case FIELD_NODE:
               tuple->set_node(field, state.get_node(reg));
               break;
            case FIELD_LIST_INT:
               tuple->set_int_list(field, state.get_int_list(reg));
               break;
            case FIELD_LIST_FLOAT:
               tuple->set_float_list(field, state.get_float_list(reg));
               break;
            case FIELD_LIST_NODE:
               tuple->set_node_list(field, state.get_node_list(reg));
               break;
            default: throw vm_exec_error("do not know how to move reg to this tuple field");
         }
      } else
         throw vm_exec_error("invalid move to field");
   }
}

static inline void
execute_move(const pcounter& pc, state& state)
{
   const instr_val to(move_to(pc));
   
   if(val_is_reg(to))
      move_to_reg(pc + MOVE_BASE, state, val_reg(to), move_from(pc));
   else if(val_is_field(to))
      move_to_field(pc + MOVE_BASE, state, move_from(pc));
   else
      throw vm_exec_error("invalid move target");
}

static inline void
execute_alloc(const pcounter& pc, state& state)
{
   tuple *tuple(state.PROGRAM->new_tuple(alloc_predicate(pc)));

   state.set_tuple(alloc_reg(pc), tuple);
}

static inline void
execute_send(const pcounter& pc, state& state)
{
   const reg_num msg(send_msg(pc));
   const reg_num dest(send_dest(pc));
   const node_val dest_val(state.get_node(dest));
   tuple *tuple(state.get_tuple(msg));
   const simple_tuple *stuple(new simple_tuple(tuple, state.count));
   
   if(msg == dest)
      // send to self
      state.proc->enqueue_work(state.node, stuple);
   else
      state::MACHINE->route(state.proc, (node::node_id)dest_val, stuple);
}

template <typename T>
static inline T get_op_function(const instr_val& val, pcounter& m, state& state);

template <>
float_val get_op_function<float_val>(const instr_val& val, pcounter& m, state& state)
{
   if(val_is_float(val)) {
      const float_val flt(pcounter_float(m));
      pcounter_move_float(&m);
      return flt;
   } else if(val_is_reg(val)) {
      return state.get_float(val_reg(val));
   } else if(val_is_field(val)) {
      const tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      pcounter_move_field(&m);
      
      return tuple->get_float(field);
   } else
      throw vm_exec_error("invalid float for float op");
}

template <>
int_val get_op_function<int_val>(const instr_val& val, pcounter& m, state& state)
{
   if(val_is_int(val)) {
      const int_val i(pcounter_int(m));
      pcounter_move_int(&m);
      return i;
   } else if(val_is_reg(val))
      return state.get_int(val_reg(val));
   else if(val_is_field(val)) {
      const tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      pcounter_move_field(&m);
      
      return tuple->get_int(field);
   } else
      throw vm_exec_error("invalid int for int op");
}

template <>
node_val get_op_function<node_val>(const instr_val& val, pcounter& m, state& state)
{
   if(val_is_host(val))
      return state.node->get_id();
   else if(val_is_node(val))
      return get_node_val(m, state);
   else if(val_is_reg(val))
      return state.get_node(val_reg(val));
   else if(val_is_field(val)) {
      const tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      pcounter_move_field(&m);
      
      return tuple->get_node(field);
   } else
      throw vm_exec_error("invalid node for node op");
}

template <>
ptr_val get_op_function<ptr_val>(const instr_val& val, pcounter& m, state& state)
{
   if(val_is_reg(val))
      return state.get_ptr(val_reg(val));
   else if(val_is_field(val)) {
      const tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      pcounter_move_field(&m);
      
      return tuple->get_ptr(field);
   } else
      throw vm_exec_error("invalid ptr for ptr op");
}

template <>
int_list* get_op_function<int_list*>(const instr_val& val, pcounter& m, state& state)
{
   if(val_is_reg(val))
      return state.get_int_list(val_reg(val));
   else if(val_is_field(val)) {
      const tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      pcounter_move_field(&m);
      
      return tuple->get_int_list(field);
   } else if(val_is_nil(val))
      return int_list::null_list();
   else
      throw vm_exec_error("unable to get an int list");
}

template <>
float_list* get_op_function<float_list*>(const instr_val& val, pcounter& m, state& state)
{
   if(val_is_reg(val))
      return state.get_float_list(val_reg(val));
   else if(val_is_field(val)) {
      const tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      pcounter_move_field(&m);
      
      return tuple->get_float_list(field);
   } else if(val_is_nil(val))
      return float_list::null_list();
   else
      throw vm_exec_error("unable to get a float list");
}

template <>
node_list* get_op_function<node_list*>(const instr_val& val, pcounter& m, state& state)
{
   if(val_is_reg(val))
      return state.get_node_list(val_reg(val));
   else if(val_is_field(val)) {
      const tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      pcounter_move_field(&m);
      
      return tuple->get_node_list(field);
   } else if(val_is_nil(val))
      return node_list::null_list();
   else
      throw vm_exec_error("unable to get an addr list");
}

template <typename T>
static inline void set_op_function(const pcounter& m,
   const instr_val& dest, T val, state& state);

template <>
void set_op_function<bool_val>(const pcounter& m, const instr_val& dest,
   bool_val val, state& state)
{
   (void)m;
   
   if(val_is_reg(dest))
      state.set_bool(val_reg(dest), val);
   else if(val_is_field(dest))
      throw vm_exec_error("can't put a bool in a tuple field - yet");
   else
      throw vm_exec_error("invalid destination for bool value");
}

template <>
void set_op_function<int_val>(const pcounter& m, const instr_val& dest,
   int_val val, state& state)
{
   if(val_is_reg(dest))
      state.set_int(val_reg(dest), val);
   else if(val_is_field(dest)) {
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      tuple->set_int(field, val);
   } else
      throw vm_exec_error("invalid destination for int value");
}

template <>
void set_op_function<float_val>(const pcounter& m, const instr_val& dest,
   float_val val, state& state)
{
   if(val_is_reg(dest))
      state.set_float(val_reg(dest), val);
   else if(val_is_field(dest)) {
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      tuple->set_float(field, val);
   } else
      throw vm_exec_error("invalid destination for float value");
}

template <>
void set_op_function<node_val>(const pcounter& m, const instr_val& dest,
   node_val val, state& state)
{
   if(val_is_reg(dest))
      state.set_node(val_reg(dest), val);
   else if(val_is_field(dest)) {
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      tuple->set_node(field, val);
   } else
      throw vm_exec_error("invalid destination for addr value");
}

template <>
void set_op_function<int_list*>(const pcounter& m, const instr_val& dest,
   int_list* val, state& state)
{
   if(val_is_reg(dest))
      state.set_int_list(val_reg(dest), val);
   else if(val_is_field(dest)) {
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      tuple->set_int_list(field, val);
   } else
      throw vm_exec_error("invalid destination for int list value");
}

template <>
void set_op_function<float_list*>(const pcounter& m, const instr_val& dest,
   float_list* val, state& state)
{
   if(val_is_reg(dest))
      state.set_float_list(val_reg(dest), val);
   else if(val_is_field(dest)) {
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      tuple->set_float_list(field, val);
   } else
      throw vm_exec_error("invalid destination for float list value");
}

template <>
void set_op_function<node_list*>(const pcounter& m, const instr_val& dest,
   node_list* val, state& state)
{
   if(val_is_reg(dest))
      state.set_node_list(val_reg(dest), val);
   else if(val_is_field(dest)) {
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      const field_num field(val_field_num(m));
      
      tuple->set_node_list(field, val);
   } else
      throw vm_exec_error("invalid destination for addr list value");
}

static inline void
execute_op(const pcounter& pc, state& state)
{
   const instr_val arg1(op_arg1(pc));
   const instr_val arg2(op_arg2(pc));
   const instr_val dest(op_dest(pc));
   const instr_op op(op_op(pc));
   pcounter m = pc + OP_BASE;
   
#define implement_operation(TYPE_ARGS, TYPE_RET, OP)               { \
   const TYPE_ARGS v1(get_op_function<TYPE_ARGS>(arg1, m, state));   \
   const TYPE_ARGS v2(get_op_function<TYPE_ARGS>(arg2, m, state));   \
   set_op_function<TYPE_RET>(m, dest, (TYPE_RET)(OP), state);        \
   break;                                                            \
}

   switch(op) {
      case OP_NEQF: implement_operation(float_val, bool_val, v1 != v2);
      case OP_NEQI: implement_operation(int_val, bool_val, v1 != v2);
      case OP_EQF: implement_operation(float_val, bool_val, v1 == v2);
      case OP_EQI: implement_operation(int_val, bool_val, v1 == v2);
      case OP_LESSF: implement_operation(float_val, bool_val, v1 < v2);
      case OP_LESSI: implement_operation(int_val, bool_val, v1 < v2);
      case OP_LESSEQF: implement_operation(float_val, bool_val, v1 <= v2);
      case OP_LESSEQI: implement_operation(int_val, bool_val, v1 <= v2);
      case OP_GREATERF: implement_operation(float_val, bool_val, v1 > v2);
      case OP_GREATERI: implement_operation(int_val, bool_val, v1 > v2);
      case OP_GREATEREQF: implement_operation(float_val, bool_val, v1 >= v2);
      case OP_GREATEREQI: implement_operation(int_val, bool_val, v1 >= v2);
      case OP_MODF: implement_operation(float_val, float_val, fmod(v1,v2));
      case OP_MODI: implement_operation(int_val, int_val, v1 % v2);
      case OP_PLUSF: implement_operation(float_val, float_val, v1 + v2);
      case OP_PLUSI: implement_operation(int_val, int_val, v1 + v2);
      case OP_MINUSF: implement_operation(float_val, float_val, v1 - v2);
      case OP_MINUSI: implement_operation(int_val, int_val, v1 - v2);
      case OP_TIMESF: implement_operation(float_val, float_val, v1 * v2);
      case OP_TIMESI: implement_operation(int_val, int_val, v1 * v2);
      case OP_DIVF: implement_operation(float_val, float_val, v1 / v2);
      case OP_DIVI: implement_operation(int_val, int_val, v1 / v2);
      case OP_NEQA: implement_operation(node_val, bool_val, v1 != v2);
      case OP_EQA: implement_operation(node_val, bool_val, v1 == v2);
   }
#undef implement_operation
}

static inline void
execute_not(pcounter& pc, state& state)
{
   const instr_val op(not_op(pc));
   const instr_val dest(not_dest(pc));
   pcounter m = pc + NOT_BASE;
   bool_val val;
   
   if(val_is_reg(op))
      val = state.get_bool(val_reg(op));
   else if(val_is_field(op)) {   
      const tuple *tuple(state.get_tuple(val_field_reg(m)));
      val = tuple->get_bool(val_field_num(m));
      
      pcounter_move_field(&m);
   } else
      throw vm_exec_error("invalid source for not instruction");
   
   // invert value
   val = !val;
   
   if(val_is_reg(dest))
      state.set_bool(val_reg(dest), val);
   else if(val_is_field(dest)) {
      tuple *tuple(state.get_tuple(val_field_reg(m)));
      
      tuple->set_bool(val_field_num(m), val);
   } else
      throw vm_exec_error("invalid destination for not instruction");
}

static inline bool
do_match(const tuple *tuple, const field_num& field, const instr_val& val,
   pcounter &pc, const state& state)
{
   if(val_is_reg(val)) {
      const reg_num reg(val_reg(val));
      
      switch(tuple->get_field_type(field)) {
         case FIELD_INT: return tuple->get_int(field) == state.get_int(reg);
         case FIELD_FLOAT: return tuple->get_float(field) == state.get_float(reg);
         case FIELD_NODE: return tuple->get_node(field) == state.get_node(reg);
      }
   } else if(val_is_field(val)) {
      const vm::tuple *tuple2(state.get_tuple(val_field_reg(pc)));
      const field_num field2(val_field_num(pc));
      
      pcounter_move_field(&pc);
      
      switch(tuple->get_field_type(field)) {
         case FIELD_INT: return tuple->get_int(field) == tuple2->get_int(field2);
         case FIELD_FLOAT: return tuple->get_float(field) == tuple2->get_float(field2);
         case FIELD_NODE: return tuple->get_node(field) == tuple2->get_node(field2);
      }
   } else if(val_is_nil(val))
      throw vm_exec_error("match for NIL not implemented XXX");
   else if(val_is_host(val))
      return tuple->get_node(field) == state.node->get_id();
   else if(val_is_int(val)) {
      const int_val i(pcounter_int(pc));
      
      pcounter_move_int(&pc);
      
      return tuple->get_int(field) == i;
   } else if(val_is_float(val)) {
      const float_val flt(pcounter_float(pc));
      
      pcounter_move_float(&pc);
      
      return tuple->get_float(field) == flt;
   } else
      throw vm_exec_error("match value in iter is not valid");
}

static inline bool
do_matches(pcounter pc, const tuple *tuple, const state& state)
{
   if(iter_match_none(pc))
      return true;
      
   iter_match match;
   
   do {
      match = pc;
      const field_num field(iter_match_field(match));
      const instr_val val(iter_match_val(match));
      
      pcounter_move_match(&pc);
      
      if(!do_match(tuple, field, val, pc, state))
         return false;
   } while(!iter_match_end(match));
      
   return true;
}

static inline void
execute_iter(pcounter pc, pcounter first, state& state, tuple_vector *tuples)
{
   random_shuffle(tuples->begin(), tuples->end());
   
   for(tuple_vector::iterator it(tuples->begin());
      it != tuples->end();
      ++it)
   {
      tuple *match_tuple(*it);
      
      if(do_matches(pc, match_tuple, state)) {
         tuple *old_tuple = state.tuple;
         
         state.tuple = match_tuple;
         
         execute(first, state);
         
         // restore state
         state.tuple = old_tuple;
      }
   }
}

static inline void
execute_move_nil(pcounter& pc, state& state)
{
   const instr_val dest(move_nil_dest(pc));
   
   // optimized
   if(val_is_reg(dest))
      state.set_nil(val_reg(dest));
   else if(val_is_field(dest)) {
      tuple *tuple(state.get_tuple(val_field_reg(pc + MOVE_NIL_BASE)));
      
      tuple->set_nil(val_field_num(pc + MOVE_NIL_BASE));
   } else
      throw vm_exec_error("unsupported destination for move-nil");
}

static inline void
execute_cons(pcounter pc, state& state)
{
   pcounter m = pc + CONS_BASE;
   const instr_val head(cons_head(pc));
   const instr_val tail(cons_tail(pc));
   const instr_val dest(cons_dest(pc));
   
#define implement_cons(ELEMENT, LIST) {                              \
   const ELEMENT head_val(get_op_function<ELEMENT>(head, m, state)); \
   LIST *ls(get_op_function<LIST*>(tail, m, state));                 \
   LIST *new_list(new LIST(ls, head_val));                           \
   set_op_function(m, dest, new_list, state);                        \
   break;                                                            \
}

   switch(cons_type(pc)) {
      case FIELD_LIST_INT: implement_cons(int_val, int_list);
      case FIELD_LIST_FLOAT: implement_cons(float_val, float_list);
      case FIELD_LIST_NODE: implement_cons(node_val, node_list);
      default: break; // does not happen
   }
#undef implement_cons
}

static inline void
execute_test_nil(pcounter pc, state& state)
{
   const instr_val op(test_nil_op(pc));
   const instr_val dest(test_nil_dest(pc));
   
   pc += TEST_NIL_BASE;
   
   const ptr_val val(get_op_function<ptr_val>(op, pc, state));
   
   set_op_function(pc, dest, val == null_ptr_val, state);
}

static inline void
execute_tail(pcounter& pc, state& state)
{
   pcounter m = pc + TAIL_BASE;
   const instr_val cons(tail_cons(pc));
   const instr_val dest(tail_dest(pc));
   
#define implement_tail(TYPE)     {                    \
   TYPE *ls(get_op_function<TYPE*>(cons, m, state));  \
   TYPE *tail(ls->get_tail());                        \
   set_op_function(m, dest, tail, state);             \
   break;                                             \
}

   switch(tail_type(pc)) {
      case FIELD_LIST_INT: implement_tail(int_list);
      case FIELD_LIST_FLOAT: implement_tail(float_list);
      case FIELD_LIST_NODE: implement_tail(node_list);
      default: throw vm_exec_error("unknown list type in tail");
   }
#undef implement_tail
}

static inline void
execute_head(pcounter& pc, state& state)
{
   pcounter m = pc + HEAD_BASE;
   const instr_val cons(head_cons(pc));
   const instr_val dest(head_dest(pc));
   
#define implement_head(LIST, ELEMENT) {               \
   LIST *ls(get_op_function<LIST*>(cons, m, state));  \
   const ELEMENT val(ls->get_head());                 \
   set_op_function(m, dest, val, state);              \
   break;                                             \
}
   
   switch(head_type(pc)) {
      case FIELD_LIST_INT: implement_head(int_list, int_val);
      case FIELD_LIST_FLOAT: implement_head(float_list, float_val);
      case FIELD_LIST_NODE: implement_head(node_list, node_val);
      default: throw vm_exec_error("unknown list type in head");
   }
#undef implement_head
}

static inline void
execute_float(pcounter& pc, state& state)
{
   pcounter m = pc + FLOAT_BASE;
   const instr_val op(float_op(pc));
   const instr_val dest(float_dest(pc));
   const int_val val(get_op_function<int_val>(op, m, state));
   
   set_op_function(m, dest, static_cast<float_val>(val), state);
}

static inline pcounter
execute_select(pcounter pc, state& state)
{
   const pcounter hash_start(select_hash_start(pc));
   const code_size_t hashed(select_hash(hash_start, state.node->get_id()));
   
   if(hashed == 0) // no specific code
      return pc + select_size(pc);
   else
      return select_hash_code(hash_start, select_hash_size(pc), hashed);
}

static inline void
execute_colocated(pcounter pc, state& state)
{
   pcounter m = pc + COLOCATED_BASE;
   
   const instr_val first(colocated_first(pc));
   const instr_val second(colocated_second(pc));
   const reg_num dest(colocated_dest(pc));
 
   const node_val n1(get_op_function<node_val>(first, m, state));
   const node_val n2(get_op_function<node_val>(second, m, state));
   
   state.set_bool(dest, state::MACHINE->same_place(n1, n2));
}

static inline return_type
execute(pcounter pc, state& state)
{
   for(; ; pc = advance(pc))
   {
eval_loop:

      //instr_print_simple(pc, state.PROGRAM, cout);
      
      switch(fetch(pc)) {
         case RETURN_INSTR: return RETURN_OK;
         
         case NEXT_INSTR: return RETURN_NEXT;
         
         case RETURN_SELECT_INSTR:
            pc += return_select_jump(pc);
            goto eval_loop;
         
         case IF_INSTR:
            if(!state.get_bool(if_reg(pc))) {
               pc += if_jump(pc);
               goto eval_loop;
            }
            break;
         
         case ELSE_INSTR:
            throw vm_exec_error("ELSE instruction not supported");
         
         case ITER_INSTR: {
               auto_ptr<tuple_vector> tuples(
                     state.node->match_predicate(iter_predicate(pc)));

               if(!tuples->empty())
                  execute_iter(pc + ITER_BASE, advance(pc), state, tuples.get());
               
               pc += iter_jump(pc);
               goto eval_loop;
            }
            
         case REMOVE_INSTR:
            throw vm_exec_error("REMOVE instruction not supported");
         
         case MOVE_INSTR:
            execute_move(pc, state);
            break;
            
         case ALLOC_INSTR:
            execute_alloc(pc, state);
            break;
            
         case SEND_INSTR:
            execute_send(pc, state);
            break;
            
         case OP_INSTR:
            execute_op(pc, state);
            break;
            
         case NOT_INSTR:
            execute_not(pc, state);
            break;
            
         case MOVE_NIL_INSTR:
            execute_move_nil(pc, state);
            break;
            
         case CONS_INSTR:
            execute_cons(pc, state);
            break;
            
         case TEST_NIL_INSTR:
            execute_test_nil(pc, state);
            break;
            
         case TAIL_INSTR:
            execute_tail(pc, state);
            break;
            
         case HEAD_INSTR:
            execute_head(pc, state);
            break;
            
         case FLOAT_INSTR:
            execute_float(pc, state);
            break;
            
         case SELECT_INSTR:
            pc = execute_select(pc, state);
            goto eval_loop;
            
         case COLOCATED_INSTR:
            execute_colocated(pc, state);
            break;
            
         default: throw vm_exec_error("unsupported instruction");
      }
   }
}
   
void
execute_bytecode(byte_code code, state& state)
{
   execute((pcounter)code, state);
}

}