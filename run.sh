#!/bin/bash

test_code='int main(){int a; int i; a = 0; for(i = 0; i != 11; i = i + 1;){a = a + i;} a;}'
#test_code=' int test_111() { char x[3]; char y; x[0] = 7; x[1] = 2; y = 4; printf("x[0]:[%d] y:[%d]\n", x[0], y); int z; z = x[0] + y; return z; } int main() { return test_111(); } '
#test_code='int main(){char x[3]; char y; x[0] = -10; x[1] = 2; y = 4; return x[0] + y;}'
#test_code='int main(){int x[3]; int y; x[0] = -10; x[1] = 2; y = 4; return x[0] + y;}'
#test_code='int main(){char x[3]; char y; x[0] = -1; x[1] = 2; y = 4;  return x[0] + y; }'
#test_code='int main(){char x[3]; char y; x[0] = -1; x[1] = 2; y = 4; printf("x[0]:[%c] y:[%c]\n", x[0], y); return x[0] + y; }'

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

#./9cc -f test/test1.c > tmp.s
#gcc -static -o tmp tmp.s

./tmp
actual="$?"

#cat tmp.s
echo "result = $actual"

