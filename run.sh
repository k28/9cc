#!/bin/bash

test_code='int main(){int a[2]; *a = 1; a[1] = 2; return a[1];}'
##test_code='int main(){int a[2]; *a = 5; int *p; p = a; return *p;}'
##test_code='int main(){int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);}'
#test_code='int main(){int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *(p + 1);}'
#test_code='int main(){int a[2]; *(a + 1) = 5; int *p; p = a; return *(p + 1);}'
#test_code='int main(){int a[3]; *(a + 2) = 1; return *(a + 2);}'
#test_code='int main(){int a[3]; *(a + 0) = 1; return *(a + 0);}'
#test_code='int main(){int a[3]; *a = 1; return *a;}'
#test_code='int main(){int a[32]; *a = 7; return *a;}'
#test_code='int main(){int b; int *a; a = &b; *a = 128; return *a;}'
#test_code='int main(){int a; a = 1; return a;}'

./9cc "$test_code" > tmp.s
gcc -o tmp tmp.s test.o
./tmp
actual="$?"

cat tmp.s
echo "result = $actual"

