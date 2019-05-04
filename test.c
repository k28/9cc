#include <stdio.h>
// テスト用のファイル
// 9ccとリンクするけど、最終的には削除するかも
int foo() { printf("OK\n"); return 0; }
int bar(int x, int y) { printf("OK (%d)\n", (x + y)); return (x + y);}
int print_int(int x) {printf("%d\n",x); return 0;}
void alloc4(int **p, int a, int b, int c, int d) {
    *p = (int *)malloc(sizeof(int) * 4);
    *p[0] = a;
    *p[1] = b;
    *p[2] = c;
    *p[3] = d;
}
