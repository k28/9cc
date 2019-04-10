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

try 0  "main(){0;}"
try 42 "main(){42;}"
try 21 'main(){5+20-4;}'
try 41 "main(){12 + 34 - 5 ;}"
try 12 "main(){3 * 4;}"
try 7  "main(){49 / 7;}"
try 9  "main(){1 * 2 + (3 + 4);}"
try 42 'main(){42;}'
try 7  'main(){a = 2; a = 1; a = 7; a;}'
try 5  'main(){a=1; b=2; b + 3;}'
try 14 'main(){a = 3; b = 5 * 6 - 8; a + b / 2;}'
try 17 'main(){a = 5; z = 12; a + z;}'
try 17 'main(){val = 5; z = 12; val + z;}'
try 7  'main(){val = 4; hoge = 3; foo = val; hoge + foo;}'
try 1  'main(){1 == 1;}'
try 0  'main(){1 == 3;}'
try 1  'main(){1 != 3;}'
try 0  'main(){1 != 1;}'
try 0  'main(){a = 1 == 1 + 2; a;}'
try 1  'main(){a = 4 == 1 + 2 * 2 - 1 == 1; a;}'
try 4  'hoge(){a = 3;} main(){a = 4; a;}'
try 3  'hoge(){a = 3;} main(){a = 4; a; hoge();}'

echo OK
