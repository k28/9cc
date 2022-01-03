#include "9cc.h"

Vector *tokens_;
Vector *functions_;
Vector *strings_;
Map    *global_variables_;
int label_ = 0;
char *source_;
char *filename_;

// 現在読んでいるトークンの場所
int pos = 0;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // 引数 -test の時にテストコードを実行
    if (strcmp("-test", argv[1]) == 0) {
        test_vector();
        test_map();
        test_type();
        return 0;
    }

    // 引数 -f の時にファイルを読み込んでコードを実行する
    source_ = NULL;
    if (strcmp("-f", argv[1]) == 0) {
        char *file_path = argv[2];
        source_ = read_file(file_path);
        filename_ = file_path;
    } else {
        source_ = argv[1];
        filename_ = NULL;
    }

    // 関数定義を入れるVecotr
    functions_ = new_vector();

    // 文字列リテラルをいれるVector
    strings_ = new_vector();

    // グローバル変数を入れるMap
    global_variables_ = new_map();

    // トークナイズ
    tokenize(source_, &tokens_);
    // プリプロセス
    preprocess();
    // 構文解析
    parse();
    // 値の調整
    sema();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");

    // グローバル変数を定義
    int global_val_count = get_map_size(global_variables_);
    for (int i = 0; i < global_val_count; i++) {
        char *key = map_get_key_at_index(global_variables_, i);
        Variable *val_info = map_get_at_index(global_variables_, i);
        int val_size = size_of_variale(val_info);
        // printf("%s:\n", key);
        // printf("  .zero %d\n\n", val_size);
        // TODO 変数定義部分を別関数に外出しする
        switch (val_info->type->ty) {
            case INT:
                {
                    int initval = 0;
                    if (val_info->initial_value_ != NULL) {
                        initval = *((int *)val_info->initial_value_);
                    }
                    printf("%s:\n", key);
                    printf("  .long  %d\n", *((int *)(val_info->initial_value_)));
                    break;
                }
            default:
                printf(".comm  %s, %d, %d\n", key, val_size, val_size);
        }
    }

    // 文字列リテラルを定義
    for (int i = 0; i < strings_->len; i++) {
        String *string = (String *)strings_->data[i];
        // printf("aaaa: db \"aaaa\"\n");
        printf(".data\n");
        printf(".L.str%d:\n", string->index);
        printf("  .string \"%s\"\n", string->val);
    }


    printf(".global main\n");
    // 抽象構文木を下りながらコード生成
    for (int i = 0; i < functions_->len; i++) {
        Function *function = (Function *)functions_->data[i];
        gen_function(function);
    }

    return 0;
}

