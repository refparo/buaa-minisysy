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
  int c; // NOLINT(cppcoreguidelines-init-variables)
  while (true) {
    c = in.peek();
    if (is_nondigit(c) || is_digit(c)) {
      head.push_back(char(c));
      in.get();
    } else {
      break;
    }
  }
  return std::move(head);
}

// consume `tail` and output `token`;
// if failed, try to consume an identifier instead
Token Lexer::lex_keyword(std::string && head, const char * tail, Token::Tag tok) {
  for (; *tail != '\0'; tail++) { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (in.peek() == *tail) {
      head.push_back(char(in.get()));
    } else {
      return lex_ident(std::move(head));
    }
  }
  // this would prevent "returna" from being parsed as Return\nIdent(a)
  if (is_nondigit(in.peek())) {
    return lex_ident(std::move(head));
  }
  return tok;
}

// consume any space plus possibly one token; output that token
Token Lexer::lex_incl_space() {
  int c = in.get();
  if (is_nondigit(c)) {
    std::string head{char(c)};
    switch (c) {
    case 'i':
      c = in.peek();
      if (!(is_nondigit(c) || is_digit(c))) return Token(std::move(head));
      in.get();
      head.push_back(char(c));
      switch (c) {
        case 'f': return Token::IF;
        case 'n': return lex_keyword(std::move(head), "t", Token::INT);
        default: return lex_ident(std::move(head));
      }
    case 'e': return lex_keyword(std::move(head), "lse", Token::ELSE);
    case 'w': return lex_keyword(std::move(head), "hile", Token::WHILE);
    case 'b': return lex_keyword(std::move(head), "reak", Token::BREAK);
    case 'c':
      c = in.peek();
      if (!(is_nondigit(c) || is_digit(c))) return Token(std::move(head));
      in.get();
      head.push_back(char(c));
      if (c != 'o') return lex_ident(std::move(head));
      c = in.peek();
      if (!(is_nondigit(c) || is_digit(c))) return Token(std::move(head));
      in.get();
      head.push_back(char(c));
      if (c != 'n') return lex_ident(std::move(head));
      c = in.peek();
      if (!(is_nondigit(c) || is_digit(c))) return Token(std::move(head));
      in.get();
      head.push_back(char(c));
      switch (c) {
        case 's': return lex_keyword(std::move(head), "t", Token::CONST);
        case 't': return lex_keyword(std::move(head), "inue", Token::CONTINUE);
        default: return lex_ident(std::move(head));
      }
    case 'r': return lex_keyword(std::move(head), "eturn", Token::RETURN);
    case 'v': return lex_keyword(std::move(head), "oid", Token::VOID);
    default: return lex_ident(std::move(head));
    }
  }
  if (is_digit(c)) {
    std::string num{char(c)};
    while (is_digit(in.peek())) {
      num.push_back(char(in.get()));
    }
    return std::stoi(num);
  }
  if (is_space(c)) {
    while (is_space(in.peek())) {
      in.get();
    }
    return Token::SPACE;
  }
  switch (c) {
  case '=':
    if (in.peek() == '=') {
      in.get();
      return Token::EQ;
    } else {
      return Token::ASSIGN;
    }
  case ';': return Token::SEMICOLON;
  case ',': return Token::COMMA;
  case '(': return Token::LPAR;
  case ')': return Token::RPAR;
  case '{': return Token::LBRACE;
  case '}': return Token::RBRACE;
  case '+': return Token::PLUS;
  case '-': return Token::MINUS;
  case '*': return Token::MULT;
  case '/':
    if (in.peek() == '/') {
      while (in.good()) {
        in.get();
        if (in.peek() == '\n') {
          in.get();
          break;
        }
      }
      return Token::COMMENT;
    } else if (in.peek() == '*') {
      while (in.good()) {
        in.get();
        if (in.peek() == '*') {
          in.get();
          if (in.peek() == '/') {
            in.get();
            return Token::COMMENT;
          }
        }
      }
      // Unterminated /* comment
      return Token::ERR;
    } else {
      return Token::DIV;
    }
  case '%': return Token::MOD;
  case '<':
    if (in.peek() == '=') {
      in.get();
      return Token::LTEQ;
    } else {
      return Token::LT;
    }
  case '>':
    if (in.peek() == '=') {
      in.get();
      return Token::GTEQ;
    } else {
      return Token::GT;
    }
  case '!':
    if (in.peek() == '=') {
      in.get();
      return Token::NEQ;
    } else {
      return Token::NOT;
    }
  case '&':
    if (in.get() == '&') {
      return Token::AND;
    } else {
      return Token::ERR;
    }
  case '|':
    if (in.get() == '|') {
      return Token::OR;
    } else {
      return Token::ERR;
    }
  // lexer shouldn't be called when `in` is eof
  default: return Token::ERR;
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
