#!/bin/bash

try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -static -o tmp tmp.s test-tmp.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ];then
        echo "$input => $actual"
    else
        echo ""
        echo "$input"
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try_file() {
    expected="$1"
    input="$2"

    ./9cc -f "$input" > tmp.s
    gcc -static -o tmp tmp.s test-tmp.o
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ];then
        echo "$input => $actual"
    else
        echo ""
        echo "$input"
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 1   'int main(){int x; x = 1; if (x){return 1;} return 2;}'
try 2   'int main(){int x; x = 0; if (x){return 1;} return 2;}'
try 2   'int main(){int x; x = 1; if (x){ x = x + 1;} else {x = x + 2;} return x;}'
try 1   'int main(){int x; x = 1; if (x){return 1;} else {return 3;}}'

try 0   "int main(){0;}"
try 42  "int main(){42;}"
try 21  'int main(){5+20-4;}'
try 41  "int main(){12 + 34 - 5 ;}"
try 12  "int main(){3 * 4;}"
try 7   "int main(){49 / 7;}"
try 9   "int main(){1 * 2 + (3 + 4);}"
try 42  'int main(){42;}'
try 7   'int main(){int a; a = 2; a = 1; a = 7; a;}'
try 5   'int main(){int a; int b; a=1; b=2; b + 3;}'
try 1   'int main(){int a; int b; int c; a=1; b=2; a;}'
try 1   'int main(){int a; int b; a=1; b = 2;  a;}'
try 22  'int main(){int a; int b; a = 3; b = 5 * 6 - 8; return b;}'
try 11  'int main(){int a; int b; a = 3; b = 5 * 6 - 8; b / 2;}'
try 3   'int main(){int a; int b; a = 3; b = 5 * 6 - 8; return a;}'
try 14  'int main(){int a; int b; a = 3; b = 5 * 6 - 8; a + b / 2;}'
try 17  'int main(){int a; int z; a = 5; z = 12; a + z;}'
try 17  'int main(){int val; int z; val = 5; z = 12; val + z;}'
try 7   'int main(){int val; int hoge; int foo; val = 4; hoge = 3; foo = val; hoge + foo;}'
try 1   'int main(){1 == 1;}'
try 0   'int main(){1 == 3;}'
try 1   'int main(){1 != 3;}'
try 0   'int main(){1 != 1;}'
try 0   'int main(){int a; a = 1 == 1 + 2; a;}'
try 1   'int main(){int a; a = 4 == 1 + 2 * 2 - 1 == 1; a;}'
try 10  'int main(){bar(3, 7);}'
try 4   'int hoge(){int a; a = 3;} int main(){int a; a = 4; a;}'
try 7   'int hoge(){int a; int b; a = 7; return a;} int main(){return hoge();}'
try 3   'int hoge(){int a; a = 3;} int main(){int a; a = 4; a; hoge();}'
try 3   'int hoge(int x, int y){x + y;} int main(){hoge(1, 2);}'
try 13  'int hoge(int x, int y){x + y;} int main(){hoge(3, hoge(7 , 3));}'
try 17  'int hoge(int x, int y){x + y;} int main(){int a; a = 6; hoge(hoge(1, a), hoge(7 , 3));}'
try 1   'int hoge(int a,int b,int c,int d,int e,int f){a;} int main(){hoge(1,2,3,4,5,6);}'
try 2   'int hoge(int a,int b,int c,int d,int e,int f){b;} int main(){hoge(1,2,3,4,5,6);}'
try 3   'int hoge(int a,int b,int c,int d,int e,int f){c;} int main(){hoge(1,2,3,4,5,6);}'
try 4   'int hoge(int a,int b,int c,int d,int e,int f){d;} int main(){hoge(1,2,3,4,5,6);}'
try 5   'int hoge(int a,int b,int c,int d,int e,int f){e;} int main(){hoge(1,2,3,4,5,6);}'
try 6   'int hoge(int a,int b,int c,int d,int e,int f){f;} int main(){hoge(1,2,3,4,5,6);}'

try 2   'int main(){int a; a = 3; if(a){a = 2;} a;}'
try 0   'int main(){int a; a = 0; if(a){a = 2;} a;}'
try 7   'int main(){int a; a = 0; if(a){a = 2;}else{a = 7;} a;}'
try 7   'int main(){int a; a = 0; if(a){a = 2;}else a = 7; a;}'
try 0   'int main(){int a; a = 0; if(a){a = 2;}else if (a == 2){a = 5;} a;}'
try 2   'int main(){int a; a = 2; if(a){a = 2;}else if (a == 0){a = 5;} a;}'
try 3   'int main(){int a; a = 2; if(a == 1){a = 2;}else if (a == 0){a = 5;} else {a = 3;}  a;}'
try 2   'int main(){int a; a = 2; if(a){a = 2;}else if (a == 0){a = 5;} else {a = 3; return 7;}  a;}'
try 2   'int hoge(){1;} int main(){int a; a = 0; if(hoge()){a = 2;} a;}'
try 0   'int hoge(){0;} int main(){int a; a = 0; if(hoge()){a = 2;} a;}'
try 2   'int main(){int a; a = 0; if(a == 0){a = 2;} a;}'
try 55  'int main(){int a; int b; a = 10; b = 0; while (a != 0) {b = b + a; a = a - 1;} b;}'
try 10  'int hoge() {1;} int main(){int a; int b; a = 10; b = 0; while (a != 0) {b = b + hoge(); a = a - 1;} b;}'
try 55  'int main(){int a; int i; a = 0; for(i = 0; i != 11; i = i + 1){a = a + i;} a;}'
try 10  'int main(){int a; a = 0; int i; i = 10; for(; i != 11; i = i + 1){a = a + i;} a;}'
try 10  'int main(){int a; int i; a = 0; i = 10; for(; i != 11;){a = a + i; i = i + 1;} a;}'
try 10  'int main(){return 10;}'
try 0   'int main(){int i; for(i = 0; i != 10; i = i + 1){return i;} return 10;}'
try 5   'int main(){int i; for(i = 0; i != 10; i = i + 1){ if (i == 5) return i;} return 10;}'
try 5   'int hoge(int x) {if (x == 0) return 5; return 10;} int main(){return hoge(0);}'
try 10  'int hoge(int x) {if (x == 0) return 5; return 10;} int main(){return hoge(1);}'
# pointer
try 10  'int main(){int a; int *b; a = 10; return a;}'
try 1   'int main(){int x; int *y; x = 1; y = &x; return x;}'
try 10  'int main(){int x; int *y; x = 10; y = &x; return *y;}'
try 10  'int main(){int x; int c; x = 10; int *y; y = &x; return *y;}'
try 13  'int main(){int x; int c; x = 13; int *y; y = &x; return x;}'
try 13  'int main(){int x; int c; x = 13; int *y; y = &x; return *y;}'
try 13  'int main(){int x; int *y; int c; x = 13; y = &x; c = 0; return *y;}'
try 13  'int main(){int x; int c; x = 13; int *y; y = &x; c = 0; return *y;}'
try 7   'int main(){int x; int c; x = 10; c = 7; return c;}'
try 10  'int main(){int x; int c; x = 10; c = 7; return x;}'
try 10  'int main(){int x; int *c; x = 10; int *y; y = &x; return *y;}'
try 7   'int main(){int x; int c; x = 10; c = 7; int *y; return c;}'
try 7   'int main(){int x; int c; x = 10; c = 7; int *y; y = &x; return c;}'
try 7   'int main(){int x; int c; x = 10; int *y; y = &x; *y = 7; return *y;}'
try 10  'int main(){int x; int *y; x = 10; y = &x; return *y;}'
try 13  'int main(){int x; x = 3; int *y; y = &x; x = 13; return *y;}'
try 10  'int main(){int *b; int a; b = &a; *b = 10; return *b;}'
try 7   'int main(){int a; a = func_TEST01(); return a;}'
try 1   'int main(){int *p; alloc4(&p, 1, 2, 4, 8); return 1;}'
try 4   'int main(){int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 2; return *q;}'
try 8   'int main(){int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 2; *q; q = p + 3; return *q;}'
try 4   'int main(){int *p; alloc4(&p, 1, 2, 4, 8); p = p + 2; return *p;}'
try 4   'int main(){int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 2);}'
try 0   'int main(){int x; x = -6; return (6 + x);}'
try 0   'int main(){int x; x = -5; return (5 + x);}'
try 0   'int main(){int x; x = 3; int y; y = -(x + 5); return (8 + y);}'
try 0   'int main(){int x; x = +3; int y; y = -(x + 5); return (8 + y);}'
try 4   'int main(){int x; x = sizeof(1); return x;}'
try 4   'int main(){return sizeof(sizeof(1));}'
try 4   'int main(){int r; int x; int *y; r = sizeof(x); return r;}'
try 8   'int main(){int r; int x; int *y; r = sizeof(y); return r;}'
try 4   'int main(){int r; int x; int *y; r = sizeof(x + 3); return r;}'
try 8   'int main(){int r; int x; int *y; r = sizeof(y + 3); return r;}'
try 4   'int main(){int r; int x; int *y; r = sizeof(*y); return r;}'
try 5   'int main(){int r; int x; int *y; r = sizeof(*y) + 1; return r;}'
try 7   'int main(){int a[10]; int b; b = 7; return b;}'
try 7   'int main(){int x; int a[10]; int b; x = 7; b = 13; return x;}'
try 13  'int main(){int x; int a[10]; int b; x = 7; b = 13; return b;}'
try 10  'int main(){int a[10]; return sizeof(a);}'
try 1   'int main(){int a[2]; *a = 1; int *p; p = a; return *p;}'
try 3   'int main(){int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);}'
try 2   'int main(){int a[2]; *a = 1; a[1] = 2; return a[1];}'
try 3   'int main(){int a[2]; *a = 1; a[1] = 2; return *a + a[1];}'
try 7   'int main(){int a[5]; a[3] = 7; return 3[a];}'
try 10  'int main(){int a[2]; *a = 1; a[1] = 7; a[0] = 3; return a[0] + a[1];}'
try 1   'int main(){2 > 1;}'
try 0   'int main(){1 > 2;}'
try 1   'int main(){1 < 2;}'
try 0   'int main(){2 < 1;}'
try 0   'int main(){2 <= 1;}'
try 1   'int main(){2 <= 2;}'
try 1   'int main(){2 <= 3;}'
try 0   'int main(){3 <= 2;}'
try 1   'int main(){1 >= 0;}'
try 1   'int main(){1 >= 1;}'
try 0   'int main(){0 >= 1;}'
try 7   'int main(){ int a; {a = 7; int b; b = 2;} return a;}'
try 8   'int main(){ int a; {a = 5; int b; b = 1; {a = a + b;}} {a = a + 2;} return a;}'
try 7   'int main(){ int a; {a = 5; int b; b = 1; if(0){a = a + b;}} {a = a + 2;} return a;}'
try 1   "int x; int bar; int main(){return 1;}"
try 1   "int x; int main(){x = 2; return 1;}"
try 2   "int x; int main(){x = 2; return x;}"
try 7   "int x; int y; int hoge() {y = 4; return y;} int main(){x = 2; int y; y = 7; hoge(); return y;}"
try 4   "int x; int y; int hoge() {y = 4; return y;} int main(){x = 2; hoge(); return y;}"
try 7   'int main(){char a; a = 7; a;}'
try 7   'int main(){char a; a = 2; a = 1; a = 7; a;}'
try 5   'int main(){char a; char b; a=1; b=2; b + 3;}'
try 1   'int main(){char a; char b; char c; a=1; b=2; a;}'
try 1   "char x; int main(){x = 2; return 1;}"
try 2   "char x; int main(){x = 2; return x;}"
try 9   "char x; int main(){x = 2; char y; y = 7; x = y + 2; return x;}"
try 7   "char x; char y; int hoge() {y = 4; return y;} int main(){x = 2; char y; y = 7; hoge(); return y;}"
echo "char pointer"
try 10  'int main(){char a; char *b; a = 10; return a;}'
try 1   'int main(){char x; char *y; x = 1; y = &x; return x;}'
try 10  'int main(){char x; char *y; x = 10; y = &x; return *y;}'
try 10  'int main(){char x; char c; x = 10; char *y; y = &x; return *y;}'
try 13  'int main(){char x; char c; x = 13; char *y; y = &x; return x;}'
try 13  'int main(){char x; char c; x = 13; char *y; y = &x; return *y;}'
try 13  'int main(){char x; char *y; char c; x = 13; y = &x; c = 0; return *y;}'
try 13  'int main(){char x; char c; x = 13; char *y; y = &x; c = 0; return *y;}'
try 7   'int main(){char x; char c; x = 10; c = 7; return c;}'
try 10  'int main(){char x; char c; x = 10; c = 7; return x;}'
try 10  'int main(){char x; char *c; x = 10; char *y; y = &x; return *y;}'
try 7   'int main(){char x; char c; x = 10; c = 7; char *y; return c;}'
try 7   'int main(){char x; char c; x = 10; c = 7; char *y; y = &x; return c;}'
try 7   'int main(){char x; char c; x = 10; char *y; y = &x; *y = 7; return *y;}'
try 11  'int main(){char x; char *y; x = 11; y = &x; return *y;}'
try 13  'int main(){char x; x = 3; char *y; y = &x; x = 13; return *y;}'
try 10  'int main(){char *b; char a; b = &a; *b = 10; return *b;}'
try 4   'char hoge(){char a; a = 3;} char main(){char a; a = 4; a;}'
try 2   'int main(){char a[2]; *a = 1; a[1] = 2; return a[1];}'
try 5   'char hoge(char x) {if (x == 0) return 5; return 10;} char main(){return hoge(0);}'
try 1   'char hoge(char a,char b,char c,char d,char e,char f){a;} char main(){hoge(1,2,3,4,5,6);}'
try 2   'char hoge(char a,char b,char c,char d,char e,char f){b;} char main(){hoge(1,2,3,4,5,6);}'
try 3   'char hoge(char a,char b,char c,char d,char e,char f){c;} char main(){hoge(1,2,3,4,5,6);}'
try 4   'char hoge(char a,char b,char c,char d,char e,char f){d;} char main(){hoge(1,2,3,4,5,6);}'
try 5   'char hoge(char a,char b,char c,char d,char e,char f){e;} char main(){hoge(1,2,3,4,5,6);}'
try 6   'char hoge(char a,char b,char c,char d,char e,char f){f;} char main(){hoge(1,2,3,4,5,6);}'
try 1   'int main(){int r; char x; int *y; r = sizeof(x); return r;}'
try 10  'int main(){char a[10]; return sizeof(a);}'
try 8   'int main(){char r; char x; char *y; r = sizeof(y); return r;}'
try 1   'int main(){char r; char x; char *y; r = sizeof(x + 3); return r;}'
try 8   'int main(){char r; char x; char *y; r = sizeof(y + 3); return r;}'
try 3   'int main(){char x[3]; x[0] = -1; x[1] = 2; int y; y = 4; return x[0] + y;}'
try 7   'int main(){print_str("Hello World!\n"); return 7;}'

echo "test: source code from file."
try_file 5 'test/test1.c'


echo OK
