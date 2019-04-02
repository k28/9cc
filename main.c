#include "9cc.h"

// トークナイズした結果のトークンを保持するベクター
Vector *tokens;
Node **code;

// 現在読んでいるトークンの場所
int pos = 0;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // 引数 -test の時にテストコードを実行
    if (strcmp("-test", argv[1]) == 0) {
        test_vector();
        test_map();
        return 0;
    }

    code = malloc(sizeof(void *) * MAX_CODE_NUM);

    // トークナイズする
    tokenize(argv[1]);
    program();

    // jpos = 0;
    // jNode *node = add();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    /*
     * 現在のコンパイラは関数1つのみの想定なので、
     * プロローグとエピローグが最初と最後に来ている
     */

    // プロローグ
    // 変数26個分の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    // 抽象構文木を下りながらコード生成
    for (int i = 0; code[i] != NULL; i++) {

        gen(code[i]);

        // 式の評価結果としてスタックに1つの値が残っているはずなので
        // スタックが溢れないようにポップしておく
        printf("  pop rax\n");

    }

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが帰り値になる
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");

    return 0;
}

