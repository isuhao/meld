
#include <iostream>
#include <assert.h>

#include "db/node.hpp"
#include "vm/state.hpp"

using namespace db;
using namespace std;
using namespace vm;

namespace db
{

tuple_trie*
node::get_storage(const predicate* pred)
{
   simple_tuple_map::iterator it(tuples.find(pred->get_id()));
   
   if(it == tuples.end()) {
      //cout << "New trie for " << *pred << endl;
      tuple_trie *tr(new tuple_trie(pred));
      tuples[pred->get_id()] = tr;
      return tr;
   } else
      return it->second;
}

bool
node::add_tuple(vm::tuple *tpl, ref_count many)
{
   const predicate* pred(tpl->get_predicate());
   tuple_trie *tr(get_storage(pred));
   
   return tr->insert_tuple(tpl, many);
}

node::delete_info
node::delete_tuple(vm::tuple *tuple, ref_count many)
{
   const predicate *pred(tuple->get_predicate());
   tuple_trie *tr(get_storage(pred));
   
   return tr->delete_tuple(tuple, many);
}

agg_configuration*
node::add_agg_tuple(vm::tuple *tuple, const ref_count many)
{
   const predicate *pred(tuple->get_predicate());
   predicate_id id(pred->get_id());
   aggregate_map::iterator it(aggs.find(id));
   tuple_aggregate *agg;
   
   if(it == aggs.end()) {
      // add new
      agg = new tuple_aggregate(pred);
      aggs[id] = agg;
   } else
      agg = it->second;

   return agg->add_to_set(tuple, many);
}

void
node::remove_agg_tuple(vm::tuple *tuple, const ref_count many)
{
   add_agg_tuple(tuple, -many);
}

simple_tuple_list
node::end_iteration(void)
{
   // reset counters
   for(size_t i(0); i < state::NUM_PREDICATES; ++i) {
      auto_generated[i] = 0;
      to_proc[i] = 0;
   }
   
   // generate possible aggregates
   simple_tuple_list ret;
   
   for(aggregate_map::iterator it(aggs.begin());
      it != aggs.end();
      ++it)
   {
      tuple_aggregate *agg(it->second);
      
      simple_tuple_list ls(agg->generate());
      
      ret.insert(ret.end(), ls.begin(), ls.end());
   }
   
   assert_end();
   
   return ret;
}

void
node::match_predicate(const predicate_id id, tuple_vector& vec) const
{
   simple_tuple_map::const_iterator it(tuples.find(id));
   
   if(it == tuples.end())
      return;
   
   const tuple_trie *tr(it->second);
   
   tr->match_predicate(vec);
}

void
node::match_predicate(const predicate_id id, const match& m, tuple_vector& vec) const
{
   simple_tuple_map::const_iterator it(tuples.find(id));
   
   if(it == tuples.end())
      return;
   
   const tuple_trie *tr(it->second);
   
   tr->match_predicate(m, vec);
}

void
node::delete_all(const predicate* pred)
{
   assert(false);
}

void
node::delete_by_first_int_arg(const predicate *pred, const int_val arg)
{
   tuple_trie *tr(get_storage(pred));
   
   tr->delete_by_first_int_arg(arg);
   
   aggregate_map::iterator it(aggs.find(pred->get_id()));
   
   if(it != aggs.end()) {
      tuple_aggregate *agg(it->second);
      agg->delete_by_first_int_arg(arg);
   }
}

const size_t
node::count_total(const predicate_id id) const
{
   simple_tuple_map::const_iterator it(tuples.find(id));
   
   if(it == tuples.end())
      return 0;
      
   const tuple_trie *tr(it->second);
   
   return tr->size();
}

void
node::assert_end(void) const
{
   for(aggregate_map::const_iterator it(aggs.begin());
      it != aggs.end();
      ++it)
   {
      assert(it->second->no_changes());
   }
}

node::node(const node_id _id, const node_id _trans):
   id(_id), translation(_trans)
{
   for(size_t i(0); i < state::NUM_PREDICATES; ++i) {
      to_proc.push_back(0);
      auto_generated.push_back(0);
   }
}

node::~node(void)
{
   for(size_t i(0); i < state::NUM_PREDICATES; ++i) {
      assert(auto_generated[i] == 0);
      assert(to_proc[i] == 0);
   }
}

void
node::dump(ostream& cout) const
{
   cout << get_id() << endl;
   
   for(simple_tuple_map::const_iterator it(tuples.begin());
      it != tuples.end();
      ++it)
   {
      it->second->dump(cout);
   }
}

void
node::print(ostream& cout) const
{
   cout << "--> node " << get_translated_id() << "/" << get_id()
        << " (" << this << ") <--" << endl;
   
   for(simple_tuple_map::const_iterator it(tuples.begin());
      it != tuples.end();
      ++it)
   {
      it->second->print(cout);
   }
}

ostream&
operator<<(ostream& cout, const node& node)
{
   node.print(cout);
   return cout;
}

}
