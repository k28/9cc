#!/bin/bash

test_code='int main(){int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 2);}'

./9cc "$test_code" > tmp.s
gcc -o tmp tmp.s test.o
./tmp
actual="$?"

cat tmp.s
echo "result = $actual"

