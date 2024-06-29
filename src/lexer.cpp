#include <istream>

#include "lexer.hpp"

Lexer::Lexer(std::istream & in_) : in(in_), has_next(false) {}

static bool is_nondigit(int c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static bool is_digit(int c) {
  return c >= '0' && c <= '9';
}

static bool is_space(int c) {
  return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

// given the head of an identifier, consume the rest of it and output it
Token Lexer::lex_ident(std::string && head) {
  int c;
  while (true) {
    c = in.peek();
    if (is_nondigit(c) || is_digit(c)) {
      head.push_back(char(c));
      in.get();
    } else {
      break;
    }
  }
  return Token{std::move(head)};
}

// consume `keyword` and output `token`;
// if failed, try to consume an identifier instead
//
// contract: in.peek() == keyword[0]
Token Lexer::lex_keyword(const char * keyword, Token::Tag tok) {
  std::string head{char(in.get())};
  for (keyword++; *keyword != '\0'; keyword++) {
    if (in.peek() == *keyword) {
      head.push_back(char(in.get()));
    } else {
      return lex_ident(std::move(head));
    }
  }
  // this would prevent "returna" from being parsed as Return\nIdent(a)
  // if (is_nondigit(in.peek())) {
  //   return lex_ident(std::move(head));
  // }
  return Token{tok};
}

// consume any space plus possibly one token; output that token
Token Lexer::lex_incl_space() {
  int c = in.peek();
  if (is_nondigit(c)) {
    switch (c) {
    case 'i': return lex_keyword("if", Token::IF);
    case 'e': return lex_keyword("else", Token::ELSE);
    case 'w': return lex_keyword("while", Token::WHILE);
    case 'b': return lex_keyword("break", Token::BREAK);
    case 'c': return lex_keyword("continue", Token::CONTINUE);
    case 'r': return lex_keyword("return", Token::RETURN);
    default: return lex_ident(std::string{char(in.get())});
    }
  }
  in.get();
  if (is_digit(c)) {
    std::string num{char(c)};
    while (is_digit(in.peek())) {
      num.push_back(char(in.get()));
    }
    return Token{std::stoi(num)};
  }
  if (is_space(c)) {
    while (is_space(in.peek())) {
      in.get();
    }
    return Token{Token::SPACE};
  }
  switch (c) {
  case '=':
    if (in.peek() == '=') {
      in.get();
      return Token{Token::EQ};
    } else {
      return Token{Token::ASSIGN};
    }
  case ';': return Token{Token::SEMICOLON};
  case '(': return Token{Token::LPAR};
  case ')': return Token{Token::RPAR};
  case '{': return Token{Token::LBRACE};
  case '}': return Token{Token::RBRACE};
  case '+': return Token{Token::PLUS};
  case '-': return Token{Token::MINUS};
  case '*': return Token{Token::MULT};
  case '/':
    if (in.peek() == '/') {
      while (in.good()) {
        in.get();
        if (in.peek() == '\n') {
          in.get();
          break;
        }
      }
      return Token{Token::COMMENT};
    } else if (in.peek() == '*') {
      while (in.good()) {
        in.get();
        if (in.peek() == '*') {
          in.get();
          if (in.peek() == '/') {
            in.get();
            return Token{Token::COMMENT};
          }
        }
      }
      // Unterminated /* comment
      return Token{Token::ERR};
    } else {
      return Token{Token::DIV};
    }
  case '%': return Token{Token::MOD};
  case '<': return Token{Token::LT};
  case '>': return Token{Token::GT};
  // lexer shouldn't be called when `in` is eof
  default: return Token{Token::ERR};
  }
}

Token Lexer::get() {
  if (has_next) {
    has_next = false;
    return std::move(next);
  }
  Token tok = lex_incl_space();
  while (tok.tag == Token::SPACE || tok.tag == Token::COMMENT) {
    tok = lex_incl_space();
  }
  return tok;
}

const Token & Lexer::peek() {
  if (has_next) {
    return next;
  }
  next = lex_incl_space();
  while (next.tag == Token::SPACE || next.tag == Token::COMMENT) {
    next = lex_incl_space();
  }
  has_next = true;
  return next;
}
