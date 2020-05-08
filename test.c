#include <stdio.h>
#include <stdlib.h>
// テスト用のファイル
// 9ccとリンクするけど、最終的には削除するかも
int foo() { printf("OK\n"); return 0; }
int bar(int x, int y) { printf("OK (%d)\n", (x + y)); return (x + y);}
int print_int(int x) {printf("%d\n",x); return 0;}
int print_str(char *x) {printf("%s",x); return 0;}
int func_TEST01() {return 7;}
void alloc4(int **p, int a, int b, int c, int d) {
    int *int_ptr = (int *)malloc(sizeof(int) * 4);
    int_ptr[0] = a;
    int_ptr[1] = b;
    int_ptr[2] = c;
    int_ptr[3] = d;

    *p = int_ptr;
}

