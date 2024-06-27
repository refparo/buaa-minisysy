#!/bin/bash
clang++ $(< compile_flags.txt) labLexer/lexer.cpp
for i in labLexer/tests/*.in; do
  echo test $i:
  diff <(./a.out < $i) <(cat ${i%in}out) && echo ok
done
