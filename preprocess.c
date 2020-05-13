// プリプロセッサ

#include "9cc.h"

// マクロ定義をいれるMap
Map *macros_;

// define マクロを作成する
// p : identifirの先頭ポインタ (最後は必ず\nである必要がある)
void create_define_macro(char *p) {
    Vector *vec = new_vector();
    // 改行までをコピーする
    int macro_len = 1;
    while(*p != '\n') {
        p++;
        macro_len++;
    }

    char *copy_macro = malloc(macro_len + 1);   // + EOF
    memset(copy_macro, 0, macro_len + 1);
    strncpy(copy_macro, p, macro_len);
    copy_macro[macro_len] = EOF;

}

void preprocess() {
    // posを初期化
    pos = 0;

    // マクロを保持するMapを初期化
    macros_ = new_map();

    while(get_token(pos)->ty != TK_EOF) {
        Token *token = get_token(pos);
        if (token->ty == '#') {
            // マクロ定義をMapにいれる
            if (strncmp(token->loc, "#define", 7) == 0) {
                create_define_macro((token->loc + 7));
            }

            // Tokenからは削除しておく
            remove_token(pos);
        }
        pos++;
    }
    // posを初期化しておく
    pos = 0;
}

