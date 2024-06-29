#!/bin/bash

cd build
cmake --build . -j8
cd ..

failed=()

target=build/a.out
echo testing $target:
for in in tests/*.in; do
  echo test $in:
  out=${in%in}out
  if [ -f $out ]; then
    diff <($target < $in) <(< $out) && echo ok || failed+=($in)
  else
    $target < $in > /dev/null && failed+=($in) || echo ok
  fi
done

target=build/lexer.out
echo testing $target:
for in in labLexer/tests/*.in; do
  echo test $in:
  out=${in%in}out
  diff <($target < $in) <(< $out) && echo ok || failed+=($in)
done

echo failed tests:
for in in ${failed[*]}; do
  echo $in
done
