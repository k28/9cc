#!/bin/bash

try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ];then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 0 "0;"
try 42 "42;"
try 21 '5+20-4;'
try 41 " 12 + 34 - 5 ;"
try 12 "3 * 4;"
try 7  "49 / 7;"
try 9  "1 * 2 + (3 + 4);"
try 42 '42;'
try 14 'a = 3; b = 5 * 6 - 8; a + b / 2;'
try 17 'a = 5; z = 12; a + z;'

echo OK
