#include "token.hpp"

struct Lexer {
  std::istream & in;
private:
  bool has_next;
  Token next;

public:
  Lexer(std::istream & in);

  Token get();
  const Token & peek();

private:
  Token lex_ident(std::string && head);
  Token lex_keyword(const char * keyword, Token::Tag tok);
  Token lex_incl_space();
};