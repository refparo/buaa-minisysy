#include <iostream>

#include "parser.hpp"

int main() {
  Parser parser{Lexer{std::cin}};
  try {
    parser.parse();
  } catch (const char * err) {
    std::cout << err << std::endl;
    return 1;
  }
  return 0;
}
