#include <iostream>

#include "parser.hpp"
#include "codegen.hpp"

int main() {
  try {
    Lexer lexer{std::cin};
    auto ast = parse(lexer);
    Codegen codegen;
    codegen.add_program(ast);
    auto program = std::move(codegen).get();
    foreach_func(program, assign_vregs);
    std::cout << program;
  } catch (const char * err) {
    std::cout << err << std::endl;
    return 1;
  }
  return 0;
}
