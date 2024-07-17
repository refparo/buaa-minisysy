#include <iostream>

#include "parser.hpp"
#include "codegen.hpp"

int main() {
  try {
    Lexer lexer{std::cin};
    auto ast = parse(lexer);
    Codegen codegen;
    codegen.add_program(ast);
    std::cout << std::move(codegen).get();
  } catch (const char * err) {
    std::cout << err << std::endl;
    return 1;
  }
  return 0;
}
