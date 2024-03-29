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

// pが指している文字列をトークンに分割する
//  p : 文字列
//  dest: 分割したトークンの保存先
void tokenize(char *p, Vector **dest) {
    Vector *vec = new_vector();
    int i = 0;
    while (*p) {
        // マクロを取得
        if (*p == '#') {
            Token *token = new_token(*p, p, p);
            vec_push(vec, token);
            // TODO 複数行マクロに対応する
            while(*p != '\n')
                p++;
            continue;
        }

        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        // 行コメントをスキップ
        if (strncmp(p, "//", 2) == 0) {
            p += 2;
            while(*p != '\n')
                p++;
            continue;
        }

        // ブロックコメントをスキップ
        if (strncmp(p, "/*", 2) == 0) {
            // strstrは文字列を探して、見つかれば先頭ポインタ, 見つからなけばNULLを返す
            char *q = strstr(p + 2, "*/"); 
            if (!q)
                error_at(p, "コメントが閉じられていません");
            p = q + 2;
            continue;
        }

        // 演算子, 括弧
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')' || *p == '&' || *p == '[' || *p == ']') {
            Token *token = new_token(*p, p, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        // if 文
        if (strncmp(p, "if", 2) == 0) {
            Token *token = new_token(TK_IF, p, p);
            vec_push(vec, token);
            i++;
            p += 2; // 2文字分進める
            continue;
        }

        // else 文
        if (strncmp(p, "else", 4) == 0) {
            Token *token = new_token(TK_ELSE, p, p);
            vec_push(vec, token);
            i++;
            p += 4; // 4文字分進める
            continue;
        }

        // while 文
        if (strncmp(p, "while", 5) == 0) {
            Token *token = new_token(TK_WHILE, p, p);
            vec_push(vec, token);
            i++;
            p += 5; // 5文字分進める
            continue;
        }

        // for 文
        if (strncmp(p, "for", 3) == 0) {
            Token *token = new_token(TK_FOR, p, p);
            vec_push(vec, token);
            i++;
            p += 3; // 3文字分進める
            continue;
        }

        // return 文
        // TODO returnxなども許されてしまうので
        // returnのあとの文字がトークンを表す文字で無いことを確認する必要がある
        // (他の比較においても同様の事が言える)
        if (strncmp(p, "return", 6) == 0) {
            Token *token = new_token(TK_RETURN, p, p);
            vec_push(vec, token);
            i++;
            p += 6;
            continue;
        }

        // int
        if (strncmp(p, "int", 3) == 0) {
            Token *token = new_token(TK_INT, p, p);
            vec_push(vec, token);
            i++;
            p += 3;
            continue;
        }

        // char
        if (strncmp(p, "char", 4) == 0) {
            Token *token = new_token(TK_CHAR, p, p);
            vec_push(vec, token);
            i++;
            p += 4;
            continue;
        }

        // sizeof
        if (strncmp(p, "sizeof", 6) == 0) {
            Token *token = new_token(TK_SIZEOF, p, p);
            vec_push(vec, token);
            i++;
            p += 6;
            continue;
        }

        // 文字列
        if (*p == '"') {
            // 文字列トークン, 次のダブルクオートまで読み込む
            char *pstart = ++p;
            int char_len = 0;
            while(*p != '"') {
                p++;
                char_len++;
            }

            char *str = malloc(char_len + 1);
            memset(str, 0, char_len + 1);
            strncpy(str, pstart, char_len);
            Token *token = new_token(TK_STRING, str, pstart);
            vec_push(vec, token);
            i++;

            p += 1; // 最後の"を進める
            continue;
        }

        // 変数(識別子) もしくは 関数呼び出し
        // 複数変数の文字列に対応するため、TK_IDENTに変数名を入れる
        if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z')) {
            char *pstart = p;
            int char_len = 0;
            while (is_ident_word(p) == 0) {
                p++;
                char_len++;
            }
            char *variableName = malloc(char_len + 1);
            memset(variableName, 0, char_len + 1);
            strncpy(variableName, pstart, char_len);
            Token *token = new_token(TK_IDENT, variableName, pstart);
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
            Token *token = new_token(TK_RELATIONAL, relational_name, p);
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
            Token *token = new_token(TK_EQUALITY, equality_name, p);
            vec_push(vec, token);

            i++;
            p+=2;   // 2文字読んだので2進める
            continue;
        }

        // 代入
        if (*p == '=') {
            Token *token = new_token(TK_ASSIGN, p, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        // カンマ
        if (*p == ',') {
            Token *token = new_token(TK_COMMA, p, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        // 開きカッコ
        if (*p == '{') {
            Token *token = new_token(TK_LCBACKET, p, p);
            vec_push(vec, token);
            i++;
            p++;
            continue;
        }
        // 閉じカッコ
        if (*p == '}') {
            Token *token = new_token(TK_RCBACKET, p, p);
            vec_push(vec, token);
            i++;
            p++;
            continue;
        }

        // 式の終わり
        if (*p == ';') {
            Token *token = new_token(TK_STMT, p, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            Token *token = new_token_num(p, strtol(p, &p, 10), p);
            vec_push(vec, token);

            i++;
            continue;
        }

        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }
    Token *token = new_token(TK_EOF, p, p);
    vec_push(vec, token);

    // tokens_で値を保持する
    *dest = vec;
}
