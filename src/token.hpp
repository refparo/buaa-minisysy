#include <string>

struct Token {
  enum Tag {
    IDENT,
    NUMBER,
    IF, ELSE, WHILE, BREAK, CONTINUE, RETURN, CONST,
    INT, VOID,
    ASSIGN,
    SEMICOLON,
    COMMA,
    LPAR, RPAR,
    LBRACE, RBRACE,
    PLUS, MINUS, MULT, DIV, MOD,
    LT, LTEQ, GT, GTEQ, EQ, NEQ,
    AND, OR, NOT,
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
