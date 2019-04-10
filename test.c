#include <stdio.h>
// テスト用のファイル
// 9ccとリンクするけど、最終的には削除するかも
int foo() { printf("OK\n"); return 0; }
int bar(int x, int y) { printf("OK (%d)\n", (x + y)); return (x + y);}
int print_int(int x) {printf("%d\n",x); return 0;}
