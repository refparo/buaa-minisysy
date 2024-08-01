#include <map>

#include "ast.hpp"
#include "ir.hpp"

struct Symbol {
  enum Kind {
    CONST, VAR, FUNC
  } kind;
  ir::Type type; // int or void
  int argc; // currently only int args are supported
  ir::Operand ir;
};

using Scope = std::map<std::string, Symbol>;

struct LoopContext {
  // begin of loop.
  ir::Label loop_begin;
  // blocks that need to be added a break
  std::vector<ir::Label> breaks;
};

struct TypedOperand {
  ir::Type type;
  ir::Operand inner;
};

struct Codegen {
private:
  std::vector<Scope> scopes;
  std::vector<LoopContext> loop_contexts;
  ir::Program ir;

public:
  Codegen();

  ir::Program get() &&;
  void add_program(const ast::Program & program);

private:
  void add_func(const ast::Func & func);
  void add_var_decl(const ast::VarDecl & decl);
  void add_stmt(const ast::Stmt & stmt);
  TypedOperand add_expr(const ast::Expr & expr);
  ir::Operand cast(const TypedOperand && operand, ir::Type type);

  inline Scope & get_scope() {
    return this->scopes.back();
  }
  inline LoopContext & get_loop_context() {
    if (this->loop_contexts.empty()) throw "break or continue used outside loop";
    return this->loop_contexts.back();
  }
  inline ir::Func & get_func() {
    return std::get<ir::Func>(this->ir.back());
  }
  inline ir::Label get_block() {
    return &get_func().blocks.back();
  }
  const Symbol & get_symbol(const ast::Ident & ident);
  int eval_constexpr(const ast::Expr & expr);
};
