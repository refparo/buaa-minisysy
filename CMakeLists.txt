cmake_minimum_required(VERSION 3.29)
project(minisysy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(a.out
  src/main.cpp
  src/vreg_assigner.cpp
  src/ir_builder.cpp
  src/ir.cpp
  src/parser.cpp
  src/lexer.cpp
  src/token.cpp
)

add_executable(lexer.out
  labLexer/lexer.cpp
)
