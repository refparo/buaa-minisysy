#include "ast.hpp"
#include "ir.hpp"

ir::Program build(ast::Program ast);
ir::Func build(ast::Func ast);
ir::Type build(ast::Ident ast);
void build_into(ast::Stmt & ast, ir::Func & func, ir::Block & block);
ir::Operand build_into(ast::Expr & ast, ir::Func & func, ir::Block & block);
