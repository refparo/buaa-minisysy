#include <iostream>
#include <string>

static bool is_nondigit(int c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
}

static bool is_digit(int c) {
  return c >= '0' && c <= '9';
}

// given the head of an identifier, consume the rest of it and output it
static int lex_ident(std::istream & in, std::ostream & out, std::string head) {
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
  out << "Ident(" << head << ")" << std::endl;
  return 0;
}

// consume `keyword` and output `token`;
// if failed, try to consume an identifier instead
//
// contract: in.peek() == keyword[0]
static int lex_keyword(std::istream & in, std::ostream & out, const char * keyword, const char * token) {
  std::string head;
  head.push_back(char(in.get()));
  for (keyword++; *keyword != '\0'; keyword++) {
    if (in.peek() == *keyword) {
      head.push_back(char(in.get()));
    } else {
      return lex_ident(in, out, std::move(head));
    }
  }
  // this would prevent "returna" from being parsed as Return\nIdent(a)
  // if (is_nondigit(in.peek())) {
  //   return lex_ident(in, out, std::move(head));
  // }
  out << token << std::endl;
  return 0;
}

// consume any space plus possibly one token; output that token
static int lex(std::istream & in, std::ostream & out) {
  while (true) {
    if (is_nondigit(in.peek())) {
      switch (in.peek()) {
      case 'i': return lex_keyword(in, out, "if", "If");
      case 'e': return lex_keyword(in, out, "else", "Else");
      case 'w': return lex_keyword(in, out, "while", "While");
      case 'b': return lex_keyword(in, out, "break", "Break");
      case 'c': return lex_keyword(in, out, "continue", "Continue");
      case 'r': return lex_keyword(in, out, "return", "Return");
      default: return lex_ident(in, out, {char(in.get())});
      }
    }
    if (is_digit(in.peek())) {
      out << "Number(" << char(in.get());
      while (is_digit(in.peek())) {
        out << char(in.get());
      }
      out << ")" << std::endl;
      return 0;
    }
    switch (in.peek()) {
    case '=':
      in.get();
      if (in.peek() == '=') {
        out << "Eq" << std::endl;
        in.get();
        return 0;
      } else {
        out << "Assign" << std::endl;
        return 0;
      }
    case ';':
      out << "Semicolon" << std::endl;
      in.get();
      return 0;
    case '(':
      out << "LPar" << std::endl;
      in.get();
      return 0;
    case ')':
      out << "RPar" << std::endl;
      in.get();
      return 0;
    case '{':
      out << "LBrace" << std::endl;
      in.get();
      return 0;
    case '}':
      out << "RBrace" << std::endl;
      in.get();
      return 0;
    case '+':
      out << "Plus" << std::endl;
      in.get();
      return 0;
    case '*':
      out << "Mult" << std::endl;
      in.get();
      return 0;
    case '/':
      out << "Div" << std::endl;
      in.get();
      return 0;
    case '<':
      out << "Lt" << std::endl;
      in.get();
      return 0;
    case '>':
      out << "Gt" << std::endl;
      in.get();
      return 0;
    case ' ':
    case '\r':
    case '\n':
      in.get();
      return 0;
    case std::char_traits<char>::eof():
      return 0;
    default:
      out << "Err" << std::endl;
      return 1;
    }
  }
}

int main() {
  int result = 0;
  while (std::cin.good()) {
    result = lex(std::cin, std::cout);
    if (result != 0) break;
  }
  return result;
}
