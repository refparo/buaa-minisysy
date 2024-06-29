#include <string>

struct Token {
  enum Tag {
    IDENT,
    NUMBER,
    IF, ELSE, WHILE, BREAK, CONTINUE, RETURN,
    ASSIGN,
    SEMICOLON,
    LPAR, RPAR,
    LBRACE, RBRACE,
    PLUS, MINUS, MULT, DIV, MOD,
    LT, GT, EQ,
    COMMENT,
    SPACE,
    ERR
  } tag;
  union {
    std::string ident;
    int number;
  };

  Token();
  Token(std::string && ident);
  Token(int number);
  Token(Tag tag_);

  Token(Token && other);

  ~Token();

  Token & operator=(Token && other);
};

std::ostream & operator<<(std::ostream & out, const Token & tok);
