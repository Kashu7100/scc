#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./scc "$input" > tmp.s
  gcc -static -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" == "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

try 0 0
try 21 21
try 10 "12+4-6"
try 31 "10 + 25 - 4"
try 36 '1+2+3+4+5+6+7+8'
try 55 '1+2+3+4+5+6+7+8+9+10'

echo OK
