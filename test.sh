#!/bin/bash

cd build
cmake --build . -j8
cd ..

ir_failed=()
out_failed=()

target=build/a.out
echo testing $target:
for in in tests/*.in; do
  [[ $in == *.ll.in ]] && continue
  echo test $in:
  ll=${in%in}ll
  if [ -f $ll ]; then
    $target < $in > build/a.ll
    diff build/a.ll $ll && echo ir ok || ir_failed+=($in)
    llvm-link build/a.ll libsysy/libsysy.ll -S -o build/a.ll
    llret=${in%in}ll.ret
    if [ -f $llret ]; then
      lli build/a.ll
      [ $? -eq $(cat $llret) ] && echo ret ok || out_failed+=($in)
    fi
    llout=${in%in}ll.out
    if [ -f $llout ]; then
      diff -b <(lli build/a.ll < ${in%in}ll.in) <(< $llout) && echo out ok || out_failed+=($in)
    fi
    rm build/a.ll
  else
    $target < $in > /dev/null && ir_failed+=($in) || echo ok
  fi
done

target=build/mem2reg
echo testing $target:
for in in tests/*.in; do
  [[ $in == *.ll.in ]] && continue
  echo test $in:
  ll=${in%in}ll
  if [ -f $ll ]; then
    $target < $in > build/a.ll
    llvm-link build/a.ll libsysy/libsysy.ll -S -o build/a.ll
    llret=${in%in}ll.ret
    if [ -f $llret ]; then
      lli build/a.ll
      [ $? -eq $(cat $llret) ] && echo ret ok || out_failed+=($in)
    fi
    llout=${in%in}ll.out
    if [ -f $llout ]; then
      diff -b <(lli build/a.ll < ${in%in}ll.in) <(< $llout) && echo out ok || out_failed+=($in)
    fi
    rm build/a.ll
  fi
done

target=build/lexer.out
echo testing $target:
for in in labLexer/tests/*.in; do
  echo test $in:
  out=${in%in}out
  diff <($target < $in) <(< $out) && echo ok || ir_failed+=($in)
done

echo ir failed tests:
for in in ${ir_failed[*]}; do
  echo $in
done

echo out failed tests:
for in in ${out_failed[*]}; do
  echo $in
done
