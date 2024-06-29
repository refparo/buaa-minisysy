#include <memory>
#include <string>
#include <variant>

namespace ast {

using Ident = std::string;

using Number = int;

using Expr = std::variant<
  struct Binary,
  struct Unary,
  Number
>;

struct Binary {
  enum Op {
    PLUS, MINUS, MULT, DIV, MOD
  } op;
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
};

struct Unary {
  enum Op {
    POS, NEG
  } op;
  std::unique_ptr<Expr> operand;
};

struct Return {
  Expr retval;
};

using Stmt = std::variant<Return>;

struct Block {
  Stmt stmt;
};

struct Func {
  Ident rettype;
  Ident name;
  Block body;
};

struct Program {
  Func func;
};

}
