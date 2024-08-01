cmake_minimum_required(VERSION 3.29)
project(minisysy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(base STATIC
  src/codegen.cpp
  src/ir.cpp
  src/parser.cpp
  src/lexer.cpp
  src/token.cpp
)

add_executable(a.out
  src/main.cpp
)
target_link_libraries(a.out PRIVATE base)

add_executable(mem2reg
  src/mem2reg_main.cpp
  src/mem2reg.cpp
)
target_link_libraries(mem2reg PRIVATE base)

add_executable(lexer.out
  labLexer/lexer.cpp
)
