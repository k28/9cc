#include "9cc.h"

// 与えられたノードが変数を表しているときに、その変数のアドレスを計算してスタックにプッシュする
void gen_lval(Node *node) {
    if (node->ty != ND_IDENT) {
        error("代入の左辺値が変数ではありません","");
    }

    // variablesに変数とオフセットが0オリジンで入っている
    int *stack_offset = map_get(variables, node->name);
    int offset = (*stack_offset + 1) * SIZE_OF_ADDRESS;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
}

// Nodeからスタックマシンを実現する
//
// メモリから値をロード  mov dst, [src]
// メモリに値をストア    mov [dst], src
void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    if (node->ty == ND_IDENT) {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_ASSIGN) {
        // assignの時には
        // 左の変数に右の値を入れる
        // 左の変数をgen_lvalでスタックに入れておいてから
        // genで右辺を評価して結果を代入する
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty) {
        case '+':
            printf("  add rax, rdi\n");
            break;
        case '-':
            printf("  sub rax, rdi\n");
            break;
        case '*':
            // x86-64ではmulは暗黙のうちにRAXを取って引数のレジスタの値にかけてRAXにセットする
            printf("  mul rdi\n");
            break;
        case '/':
            // divは暗黙のうちにRDX, RAXを取ってそれを連結した物を128bit整数とみなして
            // それを引数のレジスタの64ビットの値で割ってRAXにセットする
            printf("  mov rdx, 0\n");
            printf("  div rdi\n");
            break;
    }

    printf("  push rax\n");
}

