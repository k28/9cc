#!/bin/bash

test_code='int main(){char *a; a = "Hello World!\n"; print_str(a); print_str("All Work and no pkay makes Jack a dull boy.\n"); return 0;}'
#test_code='int hoge; int main(){hoge = 2; "abc"; return 0;}'
#test_code='int main(){char *y; char x; y = &x; *y = 10; return *y;}'
#test_code='int main(){char x; char *y; y = &x; *y = 10; return *y;}'
#test_code='int main(){char *b; char a; b = &a; *b = 10; return *b;}'
#test_code='int main(){char *y; char x; y = &x; *y = 10; x = 120; return *y;}'
#test_code='int main(){char x; char *y; y = &x; x = 10; return *y;}'
#test_code='int main(){char *y; char x; y = &x; x = 10; return *y;}'
#test_code='int main(){int *y; int x; y = &x; x = 10; return *y;}'
#test_code='int main(){char x; char *y; x = 10; y = &x; return *y;}'
#test_code='int main(){int x; int *y; x = 10; y = &x; return *y;}'
#test_code="int main(){char *b; char a; b = &a; *b = 10; return *b;}"
#test_code="char x; int main(){x = 2; return 1;}"
#test_code="int main(){int a; a = 7; return a;}"
#test_code="int hoge; int main(){hoge = 7; return 1;}"
#test_code='int hoge(){int a; a = 3;} int main(){int a; a = 4; a;}'
#test_code='int hoge(){return 3;} int main(){return hoge();}'

#./9cc "${test_code}" > tmp.s
./9cc -f test/test.c > tmp.s
gcc -static -o tmp tmp.s test-tmp.o

./tmp
actual="$?"

#cat tmp.s
echo "result = $actual"

