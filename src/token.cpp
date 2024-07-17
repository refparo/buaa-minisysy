#include <ostream>

#include "token.hpp"

Token::Token() : tag(ERR) {}
Token::Token(std::string && ident) : tag(IDENT), ident(std::move(ident)) {}
Token::Token(int number) : tag(NUMBER), number(number) {}
Token::Token(Tag tag_) : tag(tag_) {}

Token::Token(Token && other) : tag(other.tag) {
  switch (other.tag) {
  case IDENT: new(&ident) std::string{std::move(other.ident)}; break;
  case NUMBER: number = other.number; break;
  default: break;
  }
}

Token & Token::operator=(Token && other) {
  this->~Token();
  new(this) Token(std::move(other));
  return *this;
}

Token::~Token() {
  if (tag == IDENT) {
    ident.std::string::~string();
  }
}

std::ostream & operator<<(std::ostream & out, const Token & tok) {
  switch (tok.tag) {
  case Token::IDENT: out << "Ident(" << tok.ident << ")"; break;
  case Token::NUMBER: out << "Number(" << tok.number << ")"; break;
  case Token::IF: out << "If"; break;
  case Token::ELSE: out << "Else"; break;
  case Token::WHILE: out << "While"; break;
  case Token::BREAK: out << "Break"; break;
  case Token::CONTINUE: out << "Continue"; break;
  case Token::RETURN: out << "Return"; break;
  case Token::CONST: out << "Const"; break;
  case Token::INT: out << "Int"; break;
  case Token::VOID: out << "Void"; break;
  case Token::ASSIGN: out << "Assign"; break;
  case Token::SEMICOLON: out << "Semicolon"; break;
  case Token::COMMA: out << "Comma"; break;
  case Token::LPAR: out << "LPar"; break;
  case Token::RPAR: out << "RPar"; break;
  case Token::LBRACE: out << "LBrace"; break;
  case Token::RBRACE: out << "RBrace"; break;
  case Token::PLUS: out << "Plus"; break;
  case Token::MINUS: out << "Minus"; break;
  case Token::MULT: out << "Mult"; break;
  case Token::DIV: out << "Div"; break;
  case Token::MOD: out << "Mod"; break;
  case Token::LT: out << "Lt"; break;
  case Token::LTEQ: out << "LtEq"; break;
  case Token::GT: out << "Gt"; break;
  case Token::GTEQ: out << "GtEq"; break;
  case Token::EQ: out << "Eq"; break;
  case Token::NEQ: out << "NEq"; break;
  case Token::AND: out << "And"; break;
  case Token::OR: out << "Or"; break;
  case Token::NOT: out << "Not"; break;
  case Token::COMMENT: out << "Comment"; break;
  case Token::SPACE: out << "Space"; break;
  case Token::ERR: out << "Err"; break;
  }
  return out;
}
