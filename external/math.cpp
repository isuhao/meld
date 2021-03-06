
#include <cmath>
#include <random>
#include <ctime>

#include "runtime/objs.hpp"
#include "external/math.hpp"
#include "vm/all.hpp"

using namespace std;
using namespace runtime;

namespace vm
{
namespace external
{
   
argument
sigmoid(EXTERNAL_ARG(x))
{
   DECLARE_FLOAT(x);
   
   const float_val ret(1.0/(1.0 + exp(-x)));
   
	RETURN_FLOAT(ret);
}

argument
normalizestruct(EXTERNAL_ARG(x), EXTERNAL_ARG(ty))
{
   DECLARE_STRUCT(x);
   DECLARE_TYPE(ty);
   vm::struct_type *st((struct_type*)ty);

   float_val max_value(x->get_data(0).float_field);
   for(size_t i(1), size(st->get_size()); i < size; ++i) {
      if(x->get_data(i).float_field > max_value)
         max_value = x->get_data(i).float_field;
   }

   float_val Z(0.0);
   
   for(size_t i(0), size(st->get_size()); i < size; ++i) {
      const float_val val(x->get_data(i).float_field);
      Z += std::exp(val - max_value);
   }

   const float_val logZ(std::log(Z));
   struct1 *ret(struct1::create(st));
   assert(ret);

   for(size_t i(0), size(st->get_size()); i < size; ++i) {
      const float_val f(x->get_data(i).float_field - max_value - logZ);
      SET_FIELD_FLOAT(*(ret->get_ptr(i)), f);
   }

   RETURN_STRUCT(ret);
}

argument
normalize(EXTERNAL_ARG(x))
{
	DECLARE_LIST(x);
   runtime::cons *ptr((runtime::cons*)x);
   
   if(runtime::cons::is_null(ptr)) {
		RETURN_LIST(x);
   }
   
   /* find max value */
   float_val max_value(ptr->get_head().float_field);
   ptr = ptr->get_tail();
   while(!runtime::cons::is_null(ptr)) {
      const tuple_field data(ptr->get_head());
      const float_val val(data.float_field);

      assert(!std::isnan(val));
   
      if(val > max_value)
         max_value = val;
         
      ptr = ptr->get_tail();
   }
   
   float_val Z(0.0);
   ptr = (runtime::cons*)x;
   
   while(!runtime::cons::is_null(ptr)) {
      const float_val val(ptr->get_head().float_field);
      Z += std::exp(val - max_value);
      ptr = ptr->get_tail();
   }
   
   const float_val logZ(std::log(Z));
   ptr = (runtime::cons*)x;
   stack_float_list vals;
   while(!runtime::cons::is_null(ptr)) {
      vals.push(ptr->get_head().float_field - max_value - logZ);
      ptr = ptr->get_tail();  
   }
   
   runtime::cons *ls(from_float_stack_to_list(vals));
   
	RETURN_LIST(ls);
}

argument
dampstruct(EXTERNAL_ARG(s1), EXTERNAL_ARG(s2), EXTERNAL_ARG(fact), EXTERNAL_ARG(ty))
{
   DECLARE_STRUCT(s1);
   DECLARE_STRUCT(s2);
   DECLARE_FLOAT(fact);
   DECLARE_TYPE(ty);
   vm::struct_type *st((struct_type*)ty);

   struct1 *ret(struct1::create(st));
   for(size_t i(0), size(st->get_size()); i < size; ++i) {
      const float_val h1(s1->get_data(i).float_field);
      const float_val h2(s2->get_data(i).float_field);
      const float_val c(std::log(fact * std::exp(h2) +
         (1.0 - fact) * std::exp(h1)));
      
      ret->get_ptr(i)->float_field = c;
   }

   RETURN_STRUCT(ret);
}

argument
damp(EXTERNAL_ARG(ls1), EXTERNAL_ARG(ls2), EXTERNAL_ARG(fact))
{
   DECLARE_LIST(ls1);
   DECLARE_LIST(ls2);
   DECLARE_FLOAT(fact);
   runtime::cons *ptr1((runtime::cons*)ls1);
   runtime::cons *ptr2((runtime::cons*)ls2);
   
   runtime::cons *nil(runtime::cons::null_list());
   
   if(runtime::cons::is_null(ptr1) || runtime::cons::is_null(ptr2)) {
		RETURN_LIST(nil);
   }
   
   stack_float_list vals;
   
   while(!runtime::cons::is_null(ptr1) && !runtime::cons::is_null(ptr2)) {
      const float_val h1(ptr1->get_head().float_field);
      const float_val h2(ptr2->get_head().float_field);
      const float_val c(std::log(fact * std::exp(h2) +
         (1.0 - fact) * std::exp(h1)));
      
      vals.push(c);
      
      ptr1 = ptr1->get_tail();
      ptr2 = ptr2->get_tail();
   }
   
   runtime::cons *ptr(from_float_stack_to_list(vals));
   
	RETURN_LIST(ptr);
}

argument
dividestruct(EXTERNAL_ARG(s1), EXTERNAL_ARG(s2), EXTERNAL_ARG(ty))
{
   DECLARE_STRUCT(s1);
   DECLARE_STRUCT(s2);
   DECLARE_TYPE(ty);
   vm::struct_type *st((struct_type*)ty);

   struct1 *ret(struct1::create(st));
   for(size_t i(0), size(st->get_size()); i < size; ++i)
      ret->get_ptr(i)->float_field = s1->get_data(i).float_field - s2->get_data(i).float_field;

   RETURN_STRUCT(ret);
}

argument
divide(EXTERNAL_ARG(ls1), EXTERNAL_ARG(ls2))
{
   DECLARE_LIST(ls1);
   DECLARE_LIST(ls2);
   
   runtime::cons *ptr1((runtime::cons*)ls1);
   runtime::cons *ptr2((runtime::cons*)ls2);
   
   stack_float_list vals;
   
   while(!runtime::cons::is_null(ptr1) && !runtime::cons::is_null(ptr2)) {
      assert(!std::isnan(ptr1->get_head().float_field));
      assert(!std::isnan(ptr2->get_head().float_field));
      vals.push(ptr1->get_head().float_field - ptr2->get_head().float_field);
      
      ptr1 = ptr1->get_tail();
      ptr2 = ptr2->get_tail();
   }
   
   runtime::cons *ptr(from_float_stack_to_list(vals));
      
	RETURN_LIST(ptr);
}

argument
convolvestruct(EXTERNAL_ARG(bin_fact), EXTERNAL_ARG(s), EXTERNAL_ARG(ty))
{
   DECLARE_STRUCT(bin_fact);
   DECLARE_STRUCT(s);
   DECLARE_TYPE(ty);
   vm::struct_type *st((struct_type*)ty);
   const size_t length(st->get_size());

   struct1 *ret(struct1::create(st));
   for(size_t x(0); x < length; ++x) {
      float_val sum(0.0);
      for(size_t y(0); y < length; ++y) {
         const float_val other(s->get_data(y).float_field);
         const float_val val_bin(bin_fact->get_data(x + y * length).float_field);

         assert(!std::isnan(other));
         assert(!std::isnan(val_bin));
         sum += std::exp(val_bin + other);
      }

      if(sum == 0) sum = std::numeric_limits<float_val>::min();
      sum = std::log(sum);
      ret->get_ptr(x)->float_field = sum;
   }

   RETURN_STRUCT(ret);
}

argument
convolve(EXTERNAL_ARG(bin_fact), EXTERNAL_ARG(ls))
{
   DECLARE_LIST(bin_fact);
   DECLARE_LIST(ls);
   const size_t length(runtime::cons::length((runtime::cons*)ls));
   
   stack_float_list vals;
   tuple_field def;
   def.float_field = 0.0;
   
   for(size_t x(0); x < length; ++x) {
      float_val sum(0.0);
      runtime::cons *ptr((runtime::cons*)ls);
      
      size_t y(0);
      while(!runtime::cons::is_null(ptr)) {
         const float_val other(ptr->get_head().float_field);
         const float_val val_bin(runtime::cons::get((runtime::cons*)bin_fact, x + y * length, def).float_field);

         assert(!std::isnan(other));
         assert(!std::isnan(val_bin));
         sum += std::exp(val_bin + other);
         
         ptr = ptr->get_tail();
         ++y;
      }
      
      if(sum == 0) sum = std::numeric_limits<float_val>::min();
      
      sum = std::log(sum);
      
      vals.push(sum);
   }
   
   runtime::cons *ptr(from_float_stack_to_list(vals));
      
	RETURN_LIST(ptr);
}

argument
addfloatstructs(EXTERNAL_ARG(s1), EXTERNAL_ARG(s2), EXTERNAL_ARG(ty))
{
   DECLARE_STRUCT(s1);
   DECLARE_STRUCT(s2);
   DECLARE_TYPE(ty);
   vm::struct_type *st((struct_type*)ty);

   struct1 *ret(struct1::create(st));

   for(size_t i(0), size(st->get_size()); i < size; ++i)
      ret->get_ptr(i)->float_field = s1->get_data(i).float_field + s2->get_data(i).float_field;

   RETURN_STRUCT(ret);
}

argument
addfloatlists(EXTERNAL_ARG(ls1), EXTERNAL_ARG(ls2))
{
   DECLARE_LIST(ls1);
   DECLARE_LIST(ls2);
   
   runtime::cons *ptr1((runtime::cons*)ls1);
   runtime::cons *ptr2((runtime::cons*)ls2);
   
   stack_float_list vals;
   
   while(!runtime::cons::is_null(ptr1) && !runtime::cons::is_null(ptr2)) {
      assert(!std::isnan(ptr1->get_head().float_field));
      assert(!std::isnan(ptr2->get_head().float_field));
      vals.push(ptr1->get_head().float_field + ptr2->get_head().float_field);
      
      ptr1 = ptr1->get_tail();
      ptr2 = ptr2->get_tail();
   }
   
   runtime::cons *ptr(from_float_stack_to_list(vals));
      
   RETURN_LIST(ptr);
}

argument
residualstruct(EXTERNAL_ARG(s1), EXTERNAL_ARG(s2), EXTERNAL_ARG(ty))
{
   DECLARE_STRUCT(s1);
   DECLARE_STRUCT(s2);
   DECLARE_TYPE(ty);
   vm::struct_type *st((struct_type*)ty);

   float_val residual(0.0);
   const size_t size(st->get_size());
   for(size_t i(0); i < size; ++i)
      residual += std::abs(std::exp(s1->get_data(i).float_field) - std::exp(s2->get_data(i).float_field));

   residual /= (double)size;

   RETURN_FLOAT(residual);
}

argument
residual(EXTERNAL_ARG(l1), EXTERNAL_ARG(l2))
{
   DECLARE_LIST(l1);
   DECLARE_LIST(l2);
   runtime::cons *ls1((runtime::cons*)l1);
   runtime::cons *ls2((runtime::cons*)l2);

   double residual(0.0);
   size_t size(0);

   while(!runtime::cons::is_null(ls1) && !runtime::cons::is_null(ls2)) {
      assert(!std::isnan(ls1->get_head().float_field));
      assert(!std::isnan(ls2->get_head().float_field));
      residual += std::abs(std::exp(ls1->get_head().float_field) - std::exp(ls2->get_head().float_field));
      ls1 = ls1->get_tail();
      ls2 = ls2->get_tail();
      ++size;
   }

   residual /= (double)size;

   RETURN_FLOAT(residual);
}

argument
intpower(EXTERNAL_ARG(n1), EXTERNAL_ARG(n2))
{
   DECLARE_INT(n1);
   DECLARE_INT(n2);

   int_val result(1);
   for(int i(0); i < n2; ++i)
      result *= n1;

   RETURN_INT(result);
}

static std::mt19937 *generator = nullptr;
static std::uniform_real_distribution<double> *uni_dist = nullptr;

static void
de_finish(void)
{
   delete uni_dist;
   delete generator;
}

static bool
de_init(void)
{
   generator = new mt19937((std::time_t)0);
   uni_dist = new uniform_real_distribution<>(0, 1);
   atexit(de_finish);
   return true;
}

static bool de_start(de_init());

static int
create_random_bm(int size_bitmask)
{
   (void)de_start;
   int j;
   double cur_random = uni_dist->operator()(*generator);
   double threshold = 0;
   for(j = 0; j < size_bitmask - 1; ++j) {
      threshold += pow(2.0, -1 * j - 1);

      if(cur_random < threshold)
         break;
   }

   return j;
}

argument
degeneratevector(EXTERNAL_ARG(k), EXTERNAL_ARG(bits))
{
   DECLARE_INT(k);
   DECLARE_INT(bits);

   int finalBitCount = bits - 1;
   long rndVal;
   stack_int_list vals;

   for(int j(0); j != k; ++j) {
      rndVal = create_random_bm(finalBitCount);
      vals.push((1 << ((bits - 2) - rndVal)));
   }

   runtime::cons *ptr(from_int_stack_to_list(vals));

   RETURN_LIST(ptr);
}

argument
demergemessages(EXTERNAL_ARG(v1), EXTERNAL_ARG(v2))
{
   DECLARE_LIST(v1);
   DECLARE_LIST(v2);
   runtime::cons *ls1((runtime::cons*)v1);
   runtime::cons *ls2((runtime::cons*)v2);

   stack_int_list s;

   while(!runtime::cons::is_null(ls1)) {
      assert(!runtime::cons::is_null(ls2));
      s.push(FIELD_INT(ls1->get_head()) | FIELD_INT(ls2->get_head()));
      ls1 = ls1->get_tail();
      ls2 = ls2->get_tail();
   }

   runtime::cons *ret(from_int_stack_to_reverse_list(s));

   RETURN_LIST(ret);
}

argument
myfabs(EXTERNAL_ARG(f))
{
   DECLARE_FLOAT(f);

   RETURN_FLOAT(fabs(f));
}

}
}

