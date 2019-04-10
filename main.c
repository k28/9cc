#include "9cc.h"

Vector *tokens;
Vector *functions;

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

    // 関数定義を入れるVecotr
    functions = new_vector();

    // トークナイズする
    tokenize(argv[1]);
    program();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    // 抽象構文木を下りながらコード生成
    for (int i = 0; i < functions->len; i++) {
        Function *function = (Function *)functions->data[i];
        gen_function(function);
    }

    return 0;
}

