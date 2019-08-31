#include "9cc.h"

// 引数の文字がIDENT/関数名として有効か返す
// 有効な場合は0を返す
int is_ident_word(char *p) {
    if ('a' <= *p && *p <= 'z') {
        return 0;
    }
    if ('A' <= *p && *p <= 'Z') {
        return 0;
    }
    if ('0' <= *p && *p <= '9') {
        return 0;
    }
    if (*p == '_') {
        return 0;
    }

    return  1;
}

// pが指している文字列をトークンに分割してtokensに保存する
void tokenize(char *p) {
    Vector *vec = new_vector();
    int i = 0;
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        // 演算子, 括弧
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '&' || *p == '[' || *p == ']') {
            Token *token = new_token(*p, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        // if 文
        if (strncmp(p, "if", 2) == 0) {
            Token *token = new_token(TK_IF, p);
            vec_push(vec, token);
            i++;
            p += 2; // 2文字分進める
            continue;
        }

        // while 文
        if (strncmp(p, "while", 5) == 0) {
            Token *token = new_token(TK_WHILE, p);
            vec_push(vec, token);
            i++;
            p += 5; // 5文字分進める
            continue;
        }

        // for 文
        if (strncmp(p, "for", 3) == 0) {
            Token *token = new_token(TK_FOR, p);
            vec_push(vec, token);
            i++;
            p += 3; // 3文字分進める
            continue;
        }

        // return 文
        if (strncmp(p, "return", 6) == 0) {
            Token *token = new_token(TK_RETURN, p);
            vec_push(vec, token);
            i++;
            p += 6;
            continue;
        }

        // int
        if (strncmp(p, "int", 3) == 0) {
            Token *token = new_token(TK_INT, p);
            vec_push(vec, token);
            i++;
            p += 3;
            continue;
        }

        // sizeof
        if (strncmp(p, "sizeof", 6) == 0) {
            Token *token = new_token(TK_SIZEOF, p);
            vec_push(vec, token);
            i++;
            p += 6;
            continue;
        }

        // 変数(識別子) もしくは 関数呼び出し
        // 複数変数の文字列に対応するため、TK_IDENTに変数名を入れる
        if ('a' <= *p && *p <= 'z') {
            char *pstart = p;
            int char_len = 0;
            while (is_ident_word(p) == 0) {
                p++;
                char_len++;
            }
            char *variableName = malloc(char_len + 1);
            memset(variableName, 0, char_len + 1);
            strncpy(variableName, pstart, char_len);
            Token *token = new_token(TK_IDENT, variableName);
            vec_push(vec, token);

            i++;
            continue;
        }

        // 大小比較 (< <= > >=)
        if ((*p =='<') || (*p == '>')) {
            int relational_name_size = 1;
            if (*(p+1) == '=') {
                relational_name_size++;
            }
            char *relational_name = malloc(relational_name_size + 1);
            memset(relational_name, 0, relational_name_size + 1);
            strncpy(relational_name, p, relational_name_size);
            Token *token = new_token(TK_RELATIONAL, relational_name);
            vec_push(vec, token);

            i++;
            p += relational_name_size;
            continue;
        }

        // 等値
        if ((*p == '=' && *(p+1) == '=') ||
            (*p == '!' && *(p+1) == '=') ) {
            char *equality_name = malloc(3);
            memset(equality_name, 0, 3);
            strncpy(equality_name, p, 2);
            Token *token = new_token(TK_EQUALITY, equality_name);
            vec_push(vec, token);

            i++;
            p+=2;   // 2文字読んだので2進める
            continue;
        }

        // 代入
        if (*p == '=') {
            Token *token = new_token(TK_ASSIGN, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        // カンマ
        if (*p == ',') {
            Token *token = new_token(TK_COMMA, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        // 開きカッコ
        if (*p == '{') {
            Token *token = new_token(TK_LCBACKET, p);
            vec_push(vec, token);
            i++;
            p++;
            continue;
        }
        // 閉じカッコ
        if (*p == '}') {
            Token *token = new_token(TK_RCBACKET, p);
            vec_push(vec, token);
            i++;
            p++;
            continue;
        }

        // 式の終わり
        if (*p == ';') {
            Token *token = new_token(TK_STMT, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            Token *token = new_token_num(p, strtol(p, &p, 10));
            vec_push(vec, token);

            i++;
            continue;
        }

        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }
    Token *token = new_token(TK_EOF, p);
    vec_push(vec, token);

    // tokensで値を保持する
    tokens = vec;
}
