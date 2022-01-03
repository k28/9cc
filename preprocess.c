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
        if (get_macro_token(vec, macropos)->ty != ')') {
            error_at(p, "とじ括弧がありません。");
        }
        macropos++;
    }

    // 残りは置き換え対象の文字
    Vector *val = new_vector();
    while (get_macro_token(vec, macropos)->ty != TK_EOF) {
        Token *token = get_macro_token(vec, macropos++);
        vec_push(val, token);
    }

    // valがない場合は, argumentsが置き換え対象"()"で定義された置き換えマクロ
    if (val->len == 0) {
        val = arguments;
        arguments = NULL;
    }

    Macro *macro = malloc(sizeof(Macro));
    macro->ty        = MC_DEFINE;
    macro->name      = token->input;
    macro->arguments = arguments;
    macro->val       = val;

    map_put(macros_, macro->name, macro);
}

// defineマクロでトークンを置き換える
void replace_define(Token *token, int pos, Macro *macro) {
    if (macro->ty != MC_DEFINE)
        error_at(token->loc, "マクロの使い方が不正です。(defineマクロでありません)");

    if (macro->arguments != NULL) {
        // 関数マクロなので、tokenから引数を取得する
        error_at(token->loc, "ごめんなさい。 関数マクロには対応できていません。");
    }

    if (macro->arguments == NULL) {
        vec_remove(tokens_, pos);
        Vector *vals = macro->val;
        for (int i = 0; i < vals->len; i++) {
            Token *insert = vals->data[i];
            vec_insert(tokens_, insert, pos);
        }
    }
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

        if (token->ty == TK_IDENT) {
            // macroのマップにあるかチェックする
            Macro *macro = map_get(macros_, token->input);
            if (macro == NULL) {
                pos++;
                continue;
            }
            // マップに定義があるので、関連するトークンを置き換える
            replace_define(token, pos, macro);
        }
        pos++;
    }
    // posを初期化しておく
    pos = 0;
}

