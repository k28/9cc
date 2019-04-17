#!/bin/bash

try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s test.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ];then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 0   "main(){0;}"
try 42  "main(){42;}"
try 21  'main(){5+20-4;}'
try 41  "main(){12 + 34 - 5 ;}"
try 12  "main(){3 * 4;}"
try 7   "main(){49 / 7;}"
try 9   "main(){1 * 2 + (3 + 4);}"
try 42  'main(){42;}'
try 7   'main(){a = 2; a = 1; a = 7; a;}'
try 5   'main(){a=1; b=2; b + 3;}'
try 14  'main(){a = 3; b = 5 * 6 - 8; a + b / 2;}'
try 17  'main(){a = 5; z = 12; a + z;}'
try 17  'main(){val = 5; z = 12; val + z;}'
try 7   'main(){val = 4; hoge = 3; foo = val; hoge + foo;}'
try 1   'main(){1 == 1;}'
try 0   'main(){1 == 3;}'
try 1   'main(){1 != 3;}'
try 0   'main(){1 != 1;}'
try 0   'main(){a = 1 == 1 + 2; a;}'
try 1   'main(){a = 4 == 1 + 2 * 2 - 1 == 1; a;}'
try 10  'main(){bar(3, 7);}'
try 4   'hoge(){a = 3;} main(){a = 4; a;}'
try 3   'hoge(){a = 3;} main(){a = 4; a; hoge();}'
try 3   'hoge(x, y){x + y;} main(){hoge(1, 2);}'
try 13  'hoge(x, y){x + y;} main(){hoge(3, hoge(7 , 3));}'
try 17  'hoge(x, y){x + y;} main(){a = 6; hoge(hoge(1, a), hoge(7 , 3));}'
try 1   'hoge(a,b,c,d,e,f){a;} main(){hoge(1,2,3,4,5,6);}'
try 2   'hoge(a,b,c,d,e,f){b;} main(){hoge(1,2,3,4,5,6);}'
try 3   'hoge(a,b,c,d,e,f){c;} main(){hoge(1,2,3,4,5,6);}'
try 4   'hoge(a,b,c,d,e,f){d;} main(){hoge(1,2,3,4,5,6);}'
try 5   'hoge(a,b,c,d,e,f){e;} main(){hoge(1,2,3,4,5,6);}'
try 6   'hoge(a,b,c,d,e,f){f;} main(){hoge(1,2,3,4,5,6);}'
try 2   'main(){a = 3; if(a){a = 2;} a;}'
try 0   'main(){a = 0; if(a){a = 2;} a;}'
try 2   'hoge(){1;} main(){a = 0; if(hoge()){a = 2;} a;}'
try 0   'hoge(){0;} main(){a = 0; if(hoge()){a = 2;} a;}'
try 2   'main(){a = 0; if(a == 0){a = 2;} a;}'
try 55  'main(){a = 10; b = 0; while (a != 0) {b = b + a; a = a - 1;} b;}'
try 10  'hoge() {1;} main(){a = 10; b = 0; while (a != 0) {b = b + hoge(); a = a - 1;} b;}'
try 55  'main(){a = 0; for(i = 0; i != 11; i = i + 1;){a = a + i;} a;}'
try 10  'main(){a = 0; i = 10; for(; i != 11; i = i + 1;){a = a + i;} a;}'
try 10  'main(){a = 0; i = 10; for(; i != 11;;){a = a + i; i = i + 1;} a;}'
try 10  'main(){return 10;}'
try 0   'main(){for(i = 0; i != 10; i = i + 1;){return i;} return 10;}'
try 5   'main(){for(i = 0; i != 10; i = i + 1;){ if (i == 5) return i;} return 10;}'
try 5   'hoge(x) {if (x == 0) return 5; return 10;} main(){return hoge(0);}'
try 10  'hoge(x) {if (x == 0) return 5; return 10;} main(){return hoge(1);}'

echo OK
