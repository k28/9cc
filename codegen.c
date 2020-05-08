#include "9cc.h"

int    return_label_;        // return文の飛び先ラベル

int size_of_variale(Variable *variable) {
    Type *type = variable->type;

    switch (type->ty) {
        case INT:
           return SIZE_OF_INT;
        case CHAR:
           return SIZE_OF_CHAR;
        case PTR:
           return SIZE_OF_ADDRESS;
        case ARRAY:
           // TODO 配列の型を見る必要がある
           return type->array_size * SIZE_OF_INT;
        case UNKNOWN:
           // ここにきたらおかしい
           error("不明な変数型です.", "");
           return 0;
    }

    error("不明な変数型です.", "");
    return 0;
}

int address_offset(Variable *variable, char* name) {
    return variable->stack_offset;
}

// 大小比較
void gen_relational(Node *node) {
    // rdi, raxに比較対象の値が入っている状態でCallされる
    printf("  cmp rdi, rax\n");
    if (       strcmp("<",  node->name) == 0) {
        printf("  setg al\n");
    } else if (strcmp("<=", node->name) == 0) {
        printf("  setge al\n");
    } else if (strcmp(">",  node->name) == 0) {
        printf("  setl al\n");
    } else if (strcmp(">=", node->name) == 0) {
        printf("  setle al\n");
    } else {
        error("不正な大小比較です.",node->name);
    }

    printf("  movzb rax, al\n");
}

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
    printf("# GEN LVAL [%s]\n", node->name);
    if (node->ty == ND_DEREFERENCE) {
        // 右辺値をコンパイルする
        gen(node->rhs);
        return;
    }

    if (node->ty == ND_GLOBAL_VAL) {
        // グローバル変数の値を入れる
        // Variable *val_info = map_get(global_variables_, node->name);
        // TODO 他の型, 配列に対応するでも対応できるようにする
        //printf("  mov eax, DWORD PTR %s[rip]\n", node->name);
        printf("  lea rax, %s[rip]\n", node->name);
        printf("  push rax\n");
        return;
    }

    if (node->ty != ND_IDENT) {
        error("代入の左辺値が変数ではありません","");
    }

    // variablesには変数の型とスタックオフセットが入っている
    Variable *val_info = map_get(variables, node->name);
    int offset = address_offset(val_info, node->name);
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
        printf("# ND_FUNCCALL\n");
        // 引数がある場合は引数を評価
        if (node->rhs != NULL) {
            gen(node->rhs);
        }

        // 引数をレジスタにセット
        gen_arguments(node->name, node->val);

        // 現在のrspをrbxに保存(後から元に戻す)
        printf("  mov rbx, rsp\n");
        // rspを16バイトの倍数に調整
        printf("  and rsp, ~0x0f\n");

        // 関数名を指定して実行
        printf("  call %s\n", node->name);

        // rspを元に戻して、結果をスタックにプッシュ
        printf("  mov rsp, rbx\n");
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_ARGUMENT) {
        // 引数を評価
        gen(node->lhs);
        gen(node->rhs);
        return;
    }

    if (node->ty == ND_GLOBAL_VAL) {
        // gen_lval(node);
        printf("# ND_GLOBAL_VAL\n");
        Variable *val_info = map_get(global_variables_, node->name);
        if (val_info->type->ty == INT) {
            printf("  mov eax, DWORD PTR %s[rip]\n", node->name);
        } else if (val_info->type->ty == CHAR) {
            printf("  movsx eax, BYTE PTR %s[rip]\n", node->name);
        }
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_IDENT) {
        printf("# ND_IDENT [%s]\n", node->name);
        gen_lval(node);
        printf("  pop rax\n");

        Variable *val_info = map_get(variables, node->name);
        if (val_info->type->ty == INT) {
            printf("  mov eax, DWORD PTR [rax]\n");
            printf("  and rax, 0xFFFF\n");
        } else if (val_info->type->ty == CHAR) {
            printf("  movsx eax, BYTE PTR [rax]\n");
            printf("  and rax, 0xFFFF\n");
        } else if (val_info->type->ty == PTR) {
            printf("  mov rax, [rax]\n");
        } else if (val_info->type->ty == ARRAY) {
            // 配列の時は、アドレスをそのまま返す
        }

        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_STRING) {
        printf("# ND_STRING [%s]\n", node->name);
        printf("  lea rax, .L.str%d\n", node->val);
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_DEREFERENCE) {
        printf("# ND_DEREFERENCE\n");
        // デリファレンス演算子(*)が、左辺値として定義された場合
        // 右辺値をそのままコンパイルする
        gen(node->rhs);
        // 変数のアドレスが示すものをスタックに入れる
        printf("  pop rax\n");
        Variable *val_info = map_get(variables, node->rhs->name);
        if (val_info == NULL || val_info->type->ty == INT) {
            // TODO *(p + 1) とかの対応が必要 現状int型しか対応できていない
            printf("  mov eax, DWORD PTR [rax]\n");
            printf("  and rax, 0xFFFF\n");
        } else if (val_info->type->ty == CHAR) {
            printf("  movsx eax, BYTE PTR [rax]\n");
            printf("  and rax, 0xFFFF\n");
        } else if (val_info->type->ty == PTR) {
            Type *pointer_type = val_info->type->ptrof;
            if (pointer_type->ty == INT) {
                printf("  mov eax, DWORD PTR [rax]\n");
                printf("  and rax, 0xFFFF\n");
            } else if (pointer_type->ty == CHAR) {
                printf("  movsx eax, BYTE PTR [rax]\n");
                printf("  and rax, 0xFFFF\n");
            }
        } else if (val_info->type->ty == ARRAY) {
            printf("  mov eax, DWORD PTR [rax]\n");
            printf("  and rax, 0xFFFF\n");
        } else {
            error("DEREFERENCE not support format. ", "");
        }
        printf("  push rax\n");

        return;
    }

    if (node->ty == ND_REFERENCE) {
        // リファレンス演算子(&), 右辺の変数のアドレスをスタックに入れる
        gen_lval(node->rhs);
        return;
    }

    if (node->ty == ND_ASSIGN) {
        printf("# ND_ASSIGN VAL [%s]\n", node->lhs->name);
        // assignの時には
        // 左の変数に右の値を入れる
        // 左の変数をgen_lvalでスタックに入れておいてから
        // genで右辺を評価して結果を代入する
        gen_lval(node->lhs);

        // 変数の場合は、ポインター演算か確認する
        Variable *val_info = map_get(variables, node->lhs->name);
        int type = INT;
        if (val_info != NULL) {
            type = val_info->type->ty;
        } else if (node->lhs->ty == ND_DEREFERENCE) {
            // 何のポインターなのか調べる TODO semaなどで事前に調べておけない?
            Variable *val_info = map_get(variables, node->lhs->rhs->name);
            if(val_info != NULL) {
                type = val_info->type->ptrof->ty;
            }
        }

        gen(node->rhs);
        printf("  pop rdi\n");  // 右辺
        printf("  pop rax\n");  // 左辺
        if (type == INT) {
            printf("  and rdi, 0xFFFF\n");
            printf("  mov DWORD PTR [rax], edi\n");
        } else if (type == CHAR) {
            printf("  and rdi, 0x00FF\n");
            printf("  mov BYTE PTR [rax], dil\n");
        } else if (type == PTR) {
            // ポインターへの代入
            Type *pointer_type = val_info->type->ptrof;
            if (pointer_type->ty == INT) {
                printf("  mov [rax], rdi\n");
            } else if (pointer_type->ty == CHAR) {
                printf("  mov [rax], rdi\n");
            }
        } else {
            // TODO 必要?
            error("ASSIGN not support format. ", "");
            printf("  mov [rax], rdi\n");
        }
        printf("  push rdi\n");
        return;
    }

    if (node->ty == ND_BLOCK) {
        // programに入っているコードを順に生成
        for (int i = 0; i < node->program->len; i++) {
            gen(node->program->data[i]);
        }

        return;
    }

    if (node->ty == ND_IF) {
        printf("# ND_IF\n");
        // ifの時には、左辺を評価してから、結果を確認し
        // ラベルを作成する
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je  .Lelse%03d\n",node->label);
        for (int i = 0; i < node->program->len; i++) {
            gen(node->program->data[i]);
        }
        printf("  je  .Lend%03d\n",node->label);

        printf(".Lelse%03d:\n",node->label);
        if (node->rhs) {
            gen(node->rhs);
        }

        // ジャンプ先を定義
        printf(".Lend%03d:\n",node->label);
        return;
    }

    if (node->ty == ND_WHILE) {
        printf("# ND_WHILE\n");
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
        printf("# ND_FOR\n");
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

    if (node->ty == ND_RETURN) {
        printf("# ND_RETURN\n");
        // 右辺を評価
        if (node->rhs != NULL) {
            gen(node->rhs);
        }

        // 結果をraxに入れる
        printf("  pop rax\n");
        // return最後にジャンプ
        printf("  jmp .Lreturn%03d\n",return_label_);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty) {
        case ND_RELATIONAL:
            // 大小比較
            gen_relational(node);
            break;
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
            printf("  imul rdi\n");
            break;
        case '/':
            // divは暗黙のうちにRDX, RAXを取ってそれを連結した物を128bit整数とみなして
            // それを引数のレジスタの64ビットの値で割って商をRAXに, 余りをRDXにセットする
            // cqoをcallするとRAXに入っている64ビットの値を128ビットに伸ばしてRDA, RAXにセットすることができる
            printf("  cqo\n");
            printf("  idiv rdi\n");
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
        Variable *val_info = map_get(function->variables, argnode->name);
        int offset = address_offset(val_info, argnode->name);
        printf("  mov rax, rbp\n");
        printf("  sub rax, %d\n", offset);
        switch(i + 1) {
            case 1:
                if (val_info->type->ty == INT) {
                    printf("  mov DWORD PTR [rax], edi\n");
                } else if (val_info->type->ty == CHAR) {
                    printf("  mov BYTE PTR [rax], dil\n");
                } else {
                    printf("  mov [rax], rdi\n");
                }
                break;
            case 2:
                if (val_info->type->ty == INT) {
                    printf("  mov DWORD PTR [rax], esi\n");
                } else if (val_info->type->ty == CHAR) {
                    printf("  mov BYTE PTR [rax], sil\n");
                } else {
                    printf("  mov [rax], rsi\n");
                }
                break;
            case 3:
                if (val_info->type->ty == INT) {
                    printf("  mov DWORD PTR [rax], edx\n");
                } else if (val_info->type->ty == CHAR) {
                    printf("  mov BYTE PTR [rax], dl\n");
                } else {
                    printf("  mov [rax], rdx\n");
                }
                break;
            case 4:
                if (val_info->type->ty == INT) {
                    printf("  mov DWORD PTR [rax], ecx\n");
                } else if (val_info->type->ty == CHAR) {
                    printf("  mov BYTE PTR [rax], cl\n");
                } else {
                    printf("  mov [rax], rcx\n");
                }
                break;
            case 5:
                if (val_info->type->ty == INT) {
                    printf("  mov DWORD PTR [rax], r8d\n");
                } else if (val_info->type->ty == CHAR) {
                    printf("  mov BYTE PTR [rax], r8b\n");
                } else {
                    printf("  mov [rax], r8\n");
                }
                break;
            case 6:
                if (val_info->type->ty == INT) {
                    printf("  mov DWORD PTR [rax], r9d\n");
                } else if (val_info->type->ty == CHAR) {
                    printf("  mov BYTE PTR [rax], r9b\n");
                } else {
                    printf("  mov [rax], r9\n");
                }
                break;
        }
    }
}

void gen_function(Function *function) {
    // 関数名
    printf("%s:\n", function->name);

    // ローカル変数のMapをグローバル領域にコピー
    variables = function->variables;
    // return文用のラベルを設定
    return_label_ = function->label;

    // プロローグ
    // 変数の数分の領域を確保する
    // 変数のタイプによって確保するメモリ領域を変更する
    // FIXME 変数のスタック領域の確保に問題がある => 前後にsize_of_variablesするのはおかしいし、最後に+=8するのもおかしい
    int size_of_variables = 0;
    int variable_count = get_map_size(function->variables);
    for (int i = 0; i < variable_count; i++) {
        Variable *val_info = map_get_at_index(function->variables, i);
        size_of_variables += size_of_variale(val_info);
        val_info->stack_offset = size_of_variables;
        // size_of_variables += size_of_variale(val_info);
    }
    size_of_variables += 8;

    // RSPを16の倍数になるように調整
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

    // return文の飛び先タグを作成
    printf(".Lreturn%03d:\n",function->label);

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが返り値になる
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
}

