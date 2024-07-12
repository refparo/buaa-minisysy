#include <map>

#include "ast.hpp"
#include "ir.hpp"

struct Symbol {
  enum {
    CONST, VAR, FUNC
  } kind;
  ir::Operand ir;
};

using Context = std::map<std::string, Symbol>;

struct Codegen {
private:
  std::vector<Context> symtable;
  ir::Program ir;

public:
  Codegen();

  void add_program(const ast::Program & program);
  ir::Program get() const &&;

private:
  void add_func(const ast::Func & func);
};
