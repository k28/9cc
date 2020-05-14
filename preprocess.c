// プリプロセッサ

#include "9cc.h"

/*
 * #define identifier replacement
 * #define identifier (paremeter) replacement
 *
 */

// マクロ定義をいれるMap
Map *macros_;

// vecからposの位置のTokenを取得する
// 取得できない時にはNULLを返す
Token *get_macro_token(Vector *vec, int pos) {
    if (vec->len <= pos) {
        fprintf(stderr, "error: index out of bounds %d. len[%d]", pos, vec->len);
        return NULL;
    }

    Token *token = (Token *)vec->data[pos];
    return token;
}

// define マクロを作成する
// p : identifirの先頭ポインタ (最後は必ず\nである必要がある)
void parse_macro(char *p) {
    Vector *vec = NULL;
    // 改行までをコピーする
    int macro_len = 1;
    char *pstart = p;
    while(*p != '\n') {
        p++;
        macro_len++;
    }

    char *copy_macro = malloc(macro_len + 1);   // + EOF
    memset(copy_macro, 0, macro_len + 1);
    strncpy(copy_macro, pstart, macro_len);

    // トークナイズ
    tokenize(copy_macro, &vec);

    // define マクロ定義として処理するためにパースしてMacroを作る
    int macropos = 0;
    Token *token = get_macro_token(vec, macropos++);
    if (token->ty != TK_IDENT) {
        error_at(p, "不正なマクロ定義です。 (identidier)");
    }

    Vector *arguments = NULL;
    if (get_macro_token(vec, macropos)->ty == '(') {
        macropos++;
        // 引数

        arguments = new_vector();
        while(get_macro_token(vec, macropos)->ty != ')') {
            Token *argument = get_macro_token(vec, macropos++);
            if (argument->ty == TK_COMMA) {
                macropos++;
                continue;
            }
            if (argument->ty != TK_IDENT && argument->ty != TK_NUM) {
                error_at(p, "不正なマクロ定義です。 (argument)");
            }
            vec_push(arguments, argument);
        }
    }

    // 残りは置き換え対象の文字
    Vector *val = new_vector();
    while (get_macro_token(vec, macropos)->ty != TK_EOF) {
        Token *token = get_macro_token(vec, macropos++);
        vec_push(val, token);
    }

    Macro *macro = malloc(sizeof(Macro));
    macro->ty        = MC_DEFINE;
    macro->name      = token->input;
    macro->arguments = arguments;
    macro->val       = val;

    map_put(macros_, macro->name, macro);
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
                parse_macro((token->loc + 7));
            }

            // Tokenからは削除しておく
            remove_token(pos);
            continue;
        }
        pos++;
    }
    // posを初期化しておく
    pos = 0;
}

