#ifndef COMPILER_AST_HPP
#define COMPILER_AST_HPP

#include <typeinfo>
#include <iostream>

#include "mem/base.hpp"
#include "vm/types.hpp"
#include "vm/predicate.hpp"
#include "vm/program.hpp"
#include "compiler/lexer.hpp"
#include "vm/external.hpp"

namespace compiler
{

class expr;

class type_error : public std::runtime_error
{
   private:

      const expr *e;
      const std::string msg;

   public:

      explicit type_error(const expr *, const std::string&);
};

class expr: public mem::base
{
   protected:

      const token tok;
      vm::type *type{nullptr};

   public:

      virtual std::string str() const = 0;

      vm::type *get_type() const { return type; }

      void typecheck(vm::type *t = nullptr) {
         if(type == nullptr)
            type = t;
         else if(t == nullptr)
            ; // nothing
         else if(!type->equal(t))
            throw type_error(this, "invalid type.");
         dotypecheck();
      };

      virtual void dotypecheck() {
         std::cerr << "Typechecking not implemented for " << typeid(*this).name() << std::endl;
         abort();
      }

      vm::type* bottom_typecheck() {
         dobottom_typecheck();
         return type;
      }

      virtual void dobottom_typecheck() {
         std::cerr << "Bottom-up typechecking not implemented for " << typeid(*this).name() << std::endl;
         abort();
      }

      inline const token get_token() const { return tok; }

      explicit expr(const token& _tok): tok(_tok) {}

      friend class abstract_syntax_tree;
};

typedef enum {
   PLUS, MULT, MINUS, DIVIDE, MOD,
   EQUAL, NOT_EQUAL, AND, OR,
   GREATER, GREATER_EQUAL,
   LESSER, LESSER_EQUAL,
   NO_OP
} Op;

static inline std::string op2str(const Op op)
{
   switch(op) {
      case Op::PLUS: return "+";
      case Op::MULT: return "*";
      case Op::MINUS: return "-";
      case Op::DIVIDE: return "/";
      case Op::MOD: return "%";
      case Op::EQUAL: return "=";
      case Op::NOT_EQUAL: return "<>";
      case Op::NO_OP: return "";
      case Op::AND: return "&&";
      case Op::OR: return "||";
      case Op::GREATER: return ">";
      case Op::GREATER_EQUAL: return ">=";
      case Op::LESSER: return "<";
      case Op::LESSER_EQUAL: return "<=";
   }
}

class predicate_definition: public expr
{
   private:

      const size_t id;
      const std::string name;
      std::vector<vm::type*> types;
      const bool linear;
      const bool route;
      const bool action;

   public:

      inline size_t num_fields() const { return types.size(); }

      inline vm::type *get_field(const size_t i) const { return types[i]; }

      inline bool is_persistent() const { return !linear; }

      virtual std::string str() const {
         std::string str = "type ";
         if(linear)
            str += "linear ";
         if(route)
            str += "route ";
         if(action)
            str += "action ";
         str += name + "(";
         for(size_t i(0); i < types.size(); ++i) {
            str += types[i]->string();
            if(i < types.size()-1)
               str += ", ";
         }

         return str + ") [" + utils::to_string(id) + "]";
      }

      explicit predicate_definition(const token& _tokname,
            const size_t _id,
            std::vector<vm::type*>&& _types,
            const bool _linear,
            const bool _route,
            const bool _action):
         expr(_tokname),
         id(_id), name(_tokname.str), types(std::move(_types)),
         linear(_linear), route(_route),
         action(_action)
      {}
};

class binary_expr: public expr
{
   private:

      expr *e1;
      expr *e2;
      Op op;

   public:

      virtual void dotypecheck();

      virtual std::string str() const {
         return e1->str() + " " + op2str(op) + " " + e2->str();
      }

      explicit binary_expr(const token& tok, expr *_e1, expr *_e2, const Op _op):
         expr(tok), e1(_e1), e2(_e2), op(_op)
      {
         assert(e2);
      }
};

class var_expr: public expr
{
   public:

      virtual std::string str() const {
         return tok.str;
      }

      virtual void dotypecheck();
      virtual void dobottom_typecheck();

      std::string get_name() const { return tok.str; }

      explicit var_expr(const token& tok):
         expr(tok)
      {}
};

class nil_expr: public expr
{
   public:

      virtual std::string str() const {
         return "[]";
      }

      explicit nil_expr(const token& tok):
         expr(tok)
      {}
};

class struct_expr: public expr
{
   private:

      std::vector<expr*> exprs;

   public:

      virtual void dotypecheck();

      virtual std::string str() const {
         std::string str = ":(";

         for(size_t i(0); i < exprs.size(); ++i) {
            str += exprs[i]->str();
            if(i < exprs.size()-1)
               str += ", ";
         }

         return str + ")";
      }

      explicit struct_expr(const token& start,
            std::vector<expr*>&& _exprs):
         expr(start), exprs(std::move(_exprs))
      {
      }
};

class list_expr: public expr
{
   private:

      std::vector<expr*> head;
      expr *tail;

   public:

      virtual void dotypecheck();

      virtual std::string str() const {
         std::string str = "[";

         for(size_t i(0); i < head.size(); ++i) {
            str += head[i]->str();
            if(i < head.size()-1)
               str += ", ";
         }

         if(tail)
            str += " | " + tail->str();

         return str + "]";
      }

      explicit list_expr(const token& tok,
            std::vector<expr*>&& _head,
            expr *_tail = nullptr):
         expr(tok),
         head(std::move(_head)),
         tail(_tail)
      {
      }
};

class number_expr: public expr
{
   private:

      inline bool is_float() const
      {
         return tok.str.find(".") != std::string::npos;
      }

   public:

      virtual void dotypecheck();
      virtual void dobottom_typecheck() { }

      virtual std::string str() const {
         return tok.str;
      }

      explicit number_expr(const token& tok):
         expr(tok)
      {
      }
};

class world_expr: public expr
{
   public:

      virtual void dotypecheck() {
         if(!type->equal(vm::TYPE_INT))
            throw type_error(this, "expecting int type.");
      }

      virtual std::string str() const { return "@world"; }

      explicit world_expr(const token& tok):
         expr(tok)
      {}
};

class address_expr: public expr
{
   public:

      virtual void dotypecheck();

      virtual std::string str() const {
         return std::string("@") + tok.str;
      }

      explicit address_expr(const token& tok):
         expr(tok)
      {
      }
};

class if_expr: public expr
{
   private:

      expr *cmp;
      expr *e1;
      expr *e2;

   public:

      virtual void dotypecheck();

      virtual std::string str() const
      {
         return std::string("if ") + cmp->str() + " then " + e1->str() + " else " + e2->str() + " end";
      }

      explicit if_expr(const token& tok,
            expr *_cmp, expr *_e1, expr *_e2):
         expr(tok), cmp(_cmp), e1(_e1), e2(_e2)
      {
      }
};

class function: public expr
{
   private:

      std::vector<var_expr*> vars;
      std::vector<vm::type*> types;
      vm::type *ret_type;
      expr *body;

   public:

      size_t num_args() const { return types.size(); }
      vm::type *get_ret_type() const { return ret_type; }
      vm::type *get_arg_type(const size_t i) const { return types[i]; }

      virtual void dotypecheck();

      virtual std::string str() const
      {
         std::string str = std::string("fun ") + tok.str + "(";

         for(size_t i(0); i < vars.size(); ++i) {
            str += types[i]->string() + " " + vars[i]->str();
            if(i < vars.size()-1)
               str += ", ";
         }
         return str + ") " + ret_type->string() + " = " + body->str() + ".";
      }

      explicit function(const token& funname,
            std::vector<var_expr*>&& _vars,
            std::vector<vm::type*>&& _types,
            vm::type *_ret_type,
            expr *_body):
         expr(funname),
         vars(_vars),
         types(_types),
         ret_type(_ret_type),
         body(_body)
      {
      }
};

class funcall_expr: public expr
{
   private:

      std::vector<expr*> exprs;
      vm::external_function *fexternal;
      function *finternal;

   public:

      virtual void dotypecheck();

      virtual std::string str() const {
         std::string ret = tok.str + "(";

         for(size_t i(0); i < exprs.size(); ++i) {
            ret += exprs[i]->str();
            if(i != exprs.size() - 1)
               ret += ", ";
         }

         return ret + ")";
      }

      std::string name() const { return tok.str; }

      explicit funcall_expr(const token& tok, std::vector<expr*>&& _exprs,
            vm::external_function *_fexternal, function *_finternal):
         expr(tok), exprs(_exprs),
         fexternal(_fexternal), finternal(_finternal)
      {
      }
};

class fact: public expr
{
   private:

      predicate_definition *pred;
      std::vector<expr*> exprs;

   public:

      virtual void dotypecheck();

      virtual std::string str() const
      {
         std::string str = "";
         if(pred->is_persistent())
            str += "!";
         str += tok.str + "(";

         for(size_t i(0); i < exprs.size(); ++i) {
            str += exprs[i]->str();
            if(i < exprs.size()-1)
               str += ", ";
         }

         return str + ")";
      }

      explicit fact(const token& tok,
            predicate_definition *_pred,
            std::vector<expr*>&& _exprs):
         expr(tok), pred(_pred), exprs(_exprs)
      {
      }
};

class constant_definition_expr: public expr
{
   private:

      expr *e;

   public:

      virtual void dotypecheck() {
         e->typecheck(type);
      }

      virtual std::string str() const
      {
         return std::string("const ") + tok.str + " = " + e->str() + ".";
      }

      explicit constant_definition_expr(const token& name,
            expr *_e):
         expr(name), e(_e)
      {}
};

class constant_expr: public expr
{
   private:

      constant_definition_expr *def;

   public:

      virtual void dotypecheck();

      virtual std::string str() const
      {
         return std::string("const(") + tok.str + ")";
      }

      explicit constant_expr(const token& tok, constant_definition_expr *_def):
         expr(tok), def(_def)
      {}
};

class construct: public expr
{
   public:

      explicit construct(const token& tok):
         expr(tok)
      {}
};

class exists_construct: public construct
{
   private:

      std::vector<var_expr*> vars;
      std::vector<fact*> facts;

   public:

      virtual std::string str() const
      {
         std::string str = "exists ";
         for(size_t i(0); i < vars.size(); ++i) {
            str += vars[i]->str();
            if(i < vars.size()-1)
               str += ", ";
         }

         str += ". (";
         for(size_t i(0); i < facts.size(); ++i) {
            str += facts[i]->str();
            if(i < facts.size()-1)
               str += ", ";
         }

         return str + ")";
      }

      explicit exists_construct(const token& tok,
            std::vector<var_expr*>&& _vars,
            std::vector<fact*>&& _facts):
         construct(tok),
         vars(_vars),
         facts(_facts)
      {
      }
};

class aggregate_spec: public expr
{
   private:

      var_expr *var;

   public:

      virtual std::string str() const
      {
         return tok.str + " => " + var->str();
      }

      explicit aggregate_spec(const token& mod,
            var_expr *_var):
         expr(mod), var(_var)
      {
      }
};

class aggregate_construct: public construct
{
   private:

      std::vector<aggregate_spec*> specs;
      std::vector<var_expr*> vars;
      std::vector<expr*> conditions;
      std::vector<fact*> body_facts;
      std::vector<fact*> head_facts;

   public:

      virtual std::string str() const
      {
         std::string str("[");
         for(size_t i(0); i < specs.size(); i++) {
            str += specs[i]->str();
            if(i < specs.size()-1)
               str += ", ";
         }

         str += " | ";

         for(size_t i(0); i < vars.size(); i++) {
            str += vars[i]->str();
            if(i < vars.size()-1)
               str += ", ";
         }

         str += " | ";

         for(size_t i(0); i < body_facts.size(); ++i) {
            str += body_facts[i]->str();
            if(i < body_facts.size()-1)
               str += ", ";
         }
         for(size_t i(0); i < conditions.size(); ++i) {
            str += ", " + conditions[i]->str();
         }
         str += " | ";
         for(size_t i(0); i < head_facts.size(); ++i) {
            str += head_facts[i]->str();
            if(i < head_facts.size()-1)
               str += ", ";
         }

         return str + "}";
      }
      explicit aggregate_construct(const token& tok,
            std::vector<aggregate_spec*>&& _specs,
            std::vector<var_expr*>&& _vars,
            std::vector<expr*>&& _conditions,
            std::vector<fact*>&& _body,
            std::vector<fact*>&& _head):
         construct(tok),
         specs(_specs),
         vars(_vars),
         conditions(_conditions),
         body_facts(_body),
         head_facts(_head)
      {}
};

class comprehension_construct: public construct
{
   private:

      std::vector<var_expr*> vars;
      std::vector<expr*> conditions;
      std::vector<fact*> body_facts;
      std::vector<fact*> head_facts;

   public:

      virtual std::string str() const
      {
         std::string str("{");
         for(size_t i(0); i < vars.size(); i++) {
            str += vars[i]->str();
            if(i < vars.size()-1)
               str += ", ";
         }

         str += " | ";

         for(size_t i(0); i < body_facts.size(); ++i) {
            str += body_facts[i]->str();
            if(i < body_facts.size()-1)
               str += ", ";
         }
         for(size_t i(0); i < conditions.size(); ++i) {
            str += ", " + conditions[i]->str();
         }
         str += " | ";
         for(size_t i(0); i < head_facts.size(); ++i) {
            str += head_facts[i]->str();
            if(i < head_facts.size()-1)
               str += ", ";
         }

         return str + "}";
      }

      explicit comprehension_construct(const token& tok,
            std::vector<var_expr*>&& _vars,
            std::vector<expr*>&& _conditions,
            std::vector<fact*>&& _body,
            std::vector<fact*>&& _head):
         construct(tok),
         vars(_vars),
         conditions(_conditions),
         body_facts(_body),
         head_facts(_head)
      {}

      explicit comprehension_construct(const token& tok,
            std::vector<var_expr*>&& _vars,
            std::vector<expr*>&& _conditions,
            std::vector<fact*>&& _body):
         construct(tok),
         vars(_vars),
         conditions(_conditions),
         body_facts(_body)
      {}
};


class rule: public expr
{
   private:

      std::vector<expr*> body_conditions;
      std::vector<fact*> body_facts;
      std::vector<fact*> head_facts;
      std::vector<construct*> head_constructs;

   public:

      virtual void dotypecheck();

      virtual std::string str() const {
         std::string ret;
         for(size_t i(0); i < body_conditions.size(); ++i) {
            ret += body_conditions[i]->str();
            if(i < body_conditions.size()-1)
               ret += ", ";
         }
         if(!body_conditions.empty() && !body_facts.empty())
            ret += ", ";
         for(size_t i(0); i < body_facts.size(); ++i) {
            ret += body_facts[i]->str();
            if(i < body_facts.size()-1)
               ret += ", ";
         }
         ret += " -o ";
         for(size_t i(0); i < head_facts.size(); ++i) {
            ret += head_facts[i]->str();
            if(i < head_facts.size()-1)
               ret += ", ";
         }
         if(!head_facts.empty() && !head_constructs.empty())
            ret += ", ";
         for(size_t i(0); i < head_constructs.size(); ++i) {
            ret += head_constructs[i]->str();
            if(i < head_constructs.size()-1)
               ret += ", ";
         }
         if(head_facts.empty() && head_constructs.empty())
            ret += "1";
         return ret + ".";
      }

      explicit rule(const token& lolli,
            std::vector<expr*>&& _conditions,
            std::vector<fact*>&& _body,
            std::vector<fact*>&& _head,
            std::vector<construct*>&& _constructs):
         expr(lolli), body_conditions(_conditions),
         body_facts(_body), head_facts(_head),
         head_constructs(_constructs)
      {}
};

class axiom: public expr
{
   public:

      explicit axiom(const token& tok): expr(tok) {}
};

class basic_axiom: public axiom
{
   private:

      fact *f;

   public:

      virtual void dotypecheck();

      virtual std::string str() const {
         return f->str();
      }

      // no conditions.
      explicit basic_axiom(fact *_f): axiom(_f->get_token()), f(_f) {}
};

class conditional_axiom: public axiom
{
   private:

      std::vector<expr*> conditions;
      std::vector<fact*> head_facts;

   public:

      virtual std::string str() const {
         std::string ret;
         for(size_t i(0); i < conditions.size(); ++i) {
            ret += conditions[i]->str();
            if(i < conditions.size()-1)
               ret += ", ";
         }
         ret += " -o ";
         for(size_t i(0); i < head_facts.size(); ++i) {
            ret += head_facts[i]->str();
            if(i < head_facts.size()-1)
               ret += ", ";
         }
         return ret;
      }

      explicit conditional_axiom(const token& lolli,
            std::vector<expr*>&& _conditions,
            std::vector<fact*>&& _facts):
         axiom(lolli), conditions(_conditions),
         head_facts(_facts)
      {}
};

class abstract_syntax_tree: public mem::base
{
   private:

      std::unordered_map<std::string, constant_definition_expr*> constants;
      std::unordered_map<std::string, predicate_definition*> predicates;
      std::unordered_map<std::string, function*> functions;
      std::vector<axiom*> axioms;
      std::vector<rule*> rules;
      vm::priority_type prio_type;

      void initial_predicate1(const std::string& name, const bool linear, const bool action,
            vm::type *t1)
      {
         std::vector<vm::type*> types = {t1};
         const token tok(token::Token::NAME, "init.meld", 0, 0, name);
         predicates[name] = new predicate_definition(tok, predicates.size(),
               std::move(types), linear, false, action);
      }

      void initial_predicate4(const std::string& name, const bool linear, const bool action,
            vm::type *t1, vm::type *t2, vm::type *t3, vm::type *t4)
      {
         std::vector<vm::type*> types = {t1, t2, t3, t4};
         const token tok(token::Token::NAME, "init.meld", 0, 0, name);
         predicates[name] = new predicate_definition(tok, predicates.size(),
               std::move(types), linear, false, action);
      }

      void initial_predicate2(const std::string& name, const bool linear, const bool action,
            vm::type *t1, vm::type *t2)
      {
         std::vector<vm::type*> types = {t1, t2};
         const token tok(token::Token::NAME, "init.meld", 0, 0, name);
         predicates[name] = new predicate_definition(tok, predicates.size(),
               std::move(types), linear, false, action);
      }

      void initial_predicate3(const std::string& name, const bool linear, const bool action,
            vm::type *t1, vm::type *t2, vm::type *t3)
      {
         std::vector<vm::type*> types = {t1, t2, t3};
         const token tok(token::Token::NAME, "init.meld", 0, 0, name);
         predicates[name] = new predicate_definition(tok, predicates.size(),
               std::move(types), linear, false, action);
      }

   public:

      inline bool has_predicate(const std::string& str) const
      {
         auto it(predicates.find(str));
         return it != predicates.end();
      }

      void add_predicate(const token& tok, const bool is_linear,
            std::vector<vm::type*>&& types)
      {
         predicates[tok.str] = new predicate_definition(tok, predicates.size(), std::move(types),
               is_linear, false, false);
      }

      void print(std::ostream& out) const
      {
         if(!constants.empty()) {
            out << "CONSTS:\n";
            for(auto it(constants.begin()); it != constants.end(); it++)
               out << it->second->str() << "\n";
         }
         if(!functions.empty()) {
            out << "FUNCTIONS:\n";
            for(auto it(functions.begin()); it != functions.end(); it++)
               out << it->second->str() << "\n";
         }
         if(!axioms.empty()) {
            out << "AXIOMS: \n";
            for(size_t i(0); i < axioms.size(); ++i)
               out << axioms[i]->str() << "\n";
         }
         if(!rules.empty()) {
            out << "\nRULES: \n";
            for(size_t i(0); i < rules.size(); ++i) {
               assert(rules[i] != nullptr);
               out << rules[i]->str() << "\n";
            }
         }
      }

      void typecheck();

      explicit abstract_syntax_tree();

      friend class parser;
};

}

#endif