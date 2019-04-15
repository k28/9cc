#include "9cc.h"

// 等値の対応
void gen_equality(Node *node) {
    //
    // rdi, raxに比較対象の値が入っている状態でCallされる
    //
    printf("  cmp rdi, rax\n");
    if (       strcmp("==", node->name) == 0) {
        printf("  sete al\n");
    } else if (strcmp("!=", node->name) == 0) {
        printf("  setne al\n");
    } else {
        // ここには来ないはず
        error("不正な等値です.",node->name);
    }
    printf("  movzb rax, al\n");
    // printf("  push rax\n"); -> 呼び出し元で行う
}

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

// 引数をレジスタにセットする
void gen_arguments(char *func_name, int arg_count) {
    if (arg_count > 6) {
        error("引数が多すぎます %s", func_name);
    }
    for (int i = arg_count; i > 0; i--) {
        switch(i) {
            case 1:
                printf("  pop rdi\n");
                break;
            case 2:
                printf("  pop rsi\n");
                break;
            case 3:
                printf("  pop rdx\n");
                break;
            case 4:
                printf("  pop rcx\n");
                break;
            case 5:
                printf("  pop r8\n");
                break;
            case 6:
                printf("  pop r9\n");
                break;
            case 7:
                printf("  pop r10\n");
                break;
            case 8:
                printf("  pop r11\n");
                break;
        }
    }
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

    if (node->ty == ND_FUNCCALL) {
        // 引数がある場合は引数を評価
        if (node->rhs != NULL) {
            gen(node->rhs);
        }

        // 引数をレジスタにセット
        gen_arguments(node->name, node->val);

        // 関数名を指定して実行
        printf("  call %s\n", node->name);
        // 結果をスタックに入れる
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_ARGUMENT) {
        // 引数を評価
        gen(node->lhs);
        gen(node->rhs);
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

    if (node->ty == ND_IF) {
        // ifの時には、左辺を評価してから、結果を確認し
        // ラベルを作成する
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%03d\n",node->label);
        for (int i = 0; i < node->program->len; i++) {
            gen(node->program->data[i]);
        }

        // ジャンプ先を定義
        printf(".Lend%03d:\n",node->label);
        return;
    }

    if (node->ty == ND_WHILE) {
        // while文は初めにジャンプ先のラベルを設定
        printf(".Lbegin%03d:\n",node->label);

        // 条件を評価
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lend%03d\n",node->label);

        // whileのbodyを実行
        for (int i = 0; i < node->program->len; i++) {
            gen(node->program->data[i]);
        }

        // beginにジャンプ
        printf("  jmp .Lbegin%03d\n",node->label);

        // 終了時のジャンプ先を設定
        printf(".Lend%03d:\n",node->label);
        return;
    }

    if (node->ty == ND_FOR) {
        if (node->lhs != NULL) {
            // 初めに初期設定を行う
            gen(node->lhs);
        }

        // ループ時のジャンプ先を追加
        printf(".Lbegin%03d:\n",node->label);

        if (node->rhs != NULL) {
            // 条件を評価 (for文はrhsに条件が入っている)
            gen(node->rhs);
            printf("  pop rax\n");
            printf("  cmp rax, 0\n");
            printf("  je  .Lend%03d\n",node->label);
        }

        // forのbodyを実行
        for (int i = 0; i < node->program->len; i++) {
            gen(node->program->data[i]);
        }
        // beginにジャンプ
        printf("  jmp .Lbegin%03d\n",node->label);

        // 終了時のジャンプ先を設定
        printf(".Lend%03d:\n",node->label);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty) {
        case ND_EQUALITY:
            // 等値の比較を行う
            gen_equality(node);
            break;
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

// 関数の引数を設定する
void gen_function_variables(Function *function) {
    // Functionのargumentsに引数が入っている
    // 関数の引数はローカル変数と同じ扱いなので、値を設定しておくことで
    // 以降ローカル変数のように扱える
    // 変数はrgpからのオフセットでアクセスする
    for (int i = 0; i < function->arguments->len; i++) {
        Node *argnode = function->arguments->data[i];
        int *stach_offset = map_get(function->variables, argnode->name);
        int offset = (*stach_offset + 1) * SIZE_OF_ADDRESS;
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", offset);
        switch(i + 1) {
            case 1:
                printf("  mov [rax], rdi\n");
                break;
            case 2:
                printf("  mov [rax], rsi\n");
                break;
            case 3:
                printf("  mov [rax], rdx\n");
                break;
            case 4:
                printf("  mov [rax], rcx\n");
                break;
            case 5:
                printf("  mov [rax], r8\n");
                break;
            case 6:
                printf("  mov [rax], r9\n");
                break;
        }
    }
}

void gen_function(Function *function) {
    // 関数名
    printf("%s:\n", function->name);

    // ローカル変数のMapをグローバル領域にコピー
    variables = function->variables;

    // プロローグ
    // 変数の数分の領域を確保する
    // 関数呼び出しをする際にはRSPが16の倍数になっている必要があるとのこと
    // ココで調整しているけど、正しい?
    int size_of_variables = function->variables->keys->len * SIZE_OF_ADDRESS;
    int rsp_offset = size_of_variables % 16;
    size_of_variables += rsp_offset;
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, %d\n", size_of_variables);

    // 関数の引数に値を設定
    gen_function_variables(function);

    // コード生成
    for (int i = 0; i < function->code->len; i++) {
        gen(function->code->data[i]);
        // 式の評価結果としてスタックに1つの値が残っているはずなので
        // スタックが溢れないようにポップしておく
        printf("  pop rax\n");
    }

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが帰り値になる
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

