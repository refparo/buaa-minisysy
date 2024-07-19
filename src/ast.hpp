#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace ast {

using Ident = std::string;
using Number = int;

enum Type {
  INT, VOID
};

// Expr

using Expr = std::variant<
  struct Binary,
  struct Unary,
  struct FuncCall,
  Ident,
  Number
>;

struct Binary {
  enum Op {
    PLUS, MINUS, MULT, DIV, MOD,
    LT, LTEQ, GT, GTEQ, EQ, NEQ,
    AND, OR
  } op;
  std::unique_ptr<Expr> lhs;
  std::unique_ptr<Expr> rhs;
};

struct Unary {
  enum Op {
    POS, NEG, NOT
  } op;
  std::unique_ptr<Expr> operand;
};

struct FuncCall {
  Ident func;
  std::vector<Expr> args;
};

// Stmt

using Stmt = std::variant<
  // empty statement
  std::monostate,
  // may not be ended with ';'
  struct If,
  struct IfElse,
  struct While,
  struct Block,
  // must be ended with ';'
  struct Assign,
  struct Return,
  struct Break,
  struct Continue,
  Expr,
  struct VarDecl
>;

struct Assign {
  Ident var;
  Expr value;
};

struct Return {
  std::optional<Expr> retval;
};

struct Block : std::vector<Stmt> {};

struct If {
  Expr cond;
  std::unique_ptr<Stmt> true_body;
};

struct IfElse {
  Expr cond;
  std::unique_ptr<Stmt> true_body;
  std::unique_ptr<Stmt> false_body;
};

struct While {
  Expr cond;
  std::unique_ptr<Stmt> body;
};
struct Break {};
struct Continue {};

struct VarDef {
  Ident name;
  std::optional<Expr> init;
};

struct VarDecl {
  bool is_const;
  Type type;
  std::vector<VarDef> defs;
};

// Global

struct ArgDef {
  Type type;
  Ident name;
};

struct Func {
  Type rettype;
  Ident name;
  std::vector<ArgDef> args;
  Block body;
};

using Global = std::variant<Func, VarDecl>;

using Program = std::vector<Global>;

}
