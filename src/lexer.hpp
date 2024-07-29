#include "token.hpp"

struct Lexer {
private:
  std::istream & in; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
  bool has_next;
  Token next;

public:
  Lexer(std::istream & in);

  Token get();
  const Token & peek();

private:
  Token lex_ident(std::string && head);
  Token lex_keyword(std::string && head, const char * tail, Token::Tag tok);
  Token lex_incl_space();
};
