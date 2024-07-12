#include "lexer.hpp"
#include "ast.hpp"

struct Parser {
  Lexer lexer;

  ast::Program parse();

private:
  ast::Func parse_func();
  ast::Block parse_block();
  ast::Stmt parse_stmt();
  ast::Expr parse_expr(int prec = 0);
  ast::Expr parse_unary_expr();
  ast::Expr parse_par_expr_or_number();
};
