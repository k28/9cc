#include "9cc.h"

/*
 * 生成規則
 *
 * program: stmt program
 * program: ε
 *
 * stmt: assign ";"
 *
 * assign: equality
 * assign: equality "=" assign
 *
 * equality: add
 * equality: add "==" equality
 * equality: add "!=" equality
 *
 * add: mul
 * add: add "+" mul
 * add: add "-" mul
 *
 * mul: func
 * mul: mul "*" func
 * mul: mul "/" func
 *
 * func: ident "(" argument ")"
 * func: term
 *
 * argument: equality
 * argument: equality "," equality
 * argument: ε
 *
 * term: num
 * term: ident
 * term: "(" assign ")"
 *
 * digit: "0" | "1" | "2" | "3" | "4" | "5 | "6" | "7" | "8" | "9"
 * ident: "a" - "z"
 */

// 文字Tokenを作成する
Token *new_token(int ty, char *input) {
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->input = input;
    return token;
}

// 数値Tokenを作成する
Token *new_token_num(char *input, int val) {
    Token *token = malloc(sizeof(Token));
    token->ty = TK_NUM;
    token->input = input;
    token->val = val;
    return token;
}

// vectorからTokenを取得する, 取得できない時にはNULLを返す
Token *get_token(int pos) {
    if (tokens->len == pos) {
        fprintf(stderr, "error : index out of bounds %d", pos);
        return NULL;
    }

    Token *token = (Token *) tokens->data[pos];
    return token;
}

// Nodeを作成する
Node *new_node(int ty, Node *lhs, Node *rhs) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 数値Nodeを作成する
Node *new_node_num(int val) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    return node;
}

// posのNodeが期待したNodeか確認する
int consume_not_add(int index, int ty) {
    Token *token = get_token(index);
    if (token == NULL)   return 0; 
    if (token->ty != ty) return 0;

    return 1;
}

// 期待したNodeか確認する. 有効ならposを1つ進める
int consume(int ty) {
    if (get_token(pos)->ty != ty) {
        return 0;
    }

    pos++;
    return 1;
}

// termを生成 (括弧)
Node *term() {
    if (consume('(')) {
        Node *node = assign();
       if (!consume(')')) {
           error("開き括弧に対応する閉じ括弧がありません: %s", get_token(pos)->input);
       }

       return node;
    }

    if (get_token(pos)->ty == TK_NUM) {
        return new_node_num(get_token(pos++)->val);
    }

    if (get_token(pos)->ty == TK_IDENT) {
        // 名称を取得しておく
        char *name = get_token(pos++)->input;
        // 変数
        Node *node = new_node(ND_IDENT, NULL, NULL);
        node->name = name;
        // 変数の数を数えるためにMapに値を入れる
        // variablesに登録されていない変数はスタックに積む必要がある
        // valsに変数のIndexを入れる
        int *count_of_value = map_get(variables, node->name);
        if (count_of_value == NULL) {
            count_of_value = malloc(sizeof(int));
            // 変数の出現順をIndexにする
            *count_of_value = variables->keys->len;
            map_put(variables, node->name, count_of_value);
        }
        return node;
    }

    error("数値でも開き括弧でもないトークンです: %s", get_token(pos)->input);
    return NULL;
}

// funcを生成 (関数呼び出し)
Node *func() {
    if (get_token(pos)->ty == TK_IDENT && consume_not_add(pos + 1, '(')) {
        // 名称を取得
        char *name = get_token(pos++)->input;
        pos++;  // 前カッコ "(" 分進める
        // 引数の数を計測するための変数をpush
        int count_of_arguments = 0;
        Node *node = new_node(ND_FUNCTION, NULL, argument(&count_of_arguments));
        node->name = name;
        // valに引数の数を入れる
        node->val = count_of_arguments;

        if (!consume(')')) {
            error("関数呼び出しに対応する 開き括弧に対応する閉じ括弧がありません: %s", get_token(pos)->input);
        }
        return node;
    }

    return term();
}

// mul を生成 (掛け算, 割り算)
Node *mul() {
    Node *node = func();
    
    for (;;) {
        if (consume('*')) {
            node = new_node('*', node, func());
        } else if (consume('/')) {
            node = new_node('/', node, func());
        } else {
            return node;
        }
    }
}

// addを生成 足し算 / 引き算
Node *add() {
    Node *node = mul();

    for (;;) {
        if (consume('+')) {
            node = new_node('+', node, mul());
        } else if (consume('-')) {
            node = new_node('-', node, mul());
        } else {
            return node;
        }
    }
}

// 等値
Node *equality() {
    Node *node = add();

    for (;;) {
        if (get_token(pos)->ty == TK_EQUALITY) {
            char *name = get_token(pos++)->input;
            node = new_node(ND_EQUALITY, node, add());
            node->name = name;
        } else {
            return node;
        }
    }
}

// assignを生成 (代入)
Node *assign() {
    Node *node = equality();

    for (;;) {
        if (consume(TK_ASSIGN)) {
            node = new_node(ND_ASSIGN, node, equality());
        } else {
            return node;
        }
    }
}

Node *argument(int *count_of_arguments) {
    if (consume_not_add(pos, ')')) {
        // エラーチェックのため 上位で閉じ括弧の有無をチェックする
        return NULL;
    }

    *count_of_arguments += 1;
    Node *node = equality();
    for (;;) {
        if (consume(TK_COMMA)) {
            *count_of_arguments += 1;
            node = new_node(ND_ARGUMENT, node, equality());
        } else {
            return node;
        }
    }
}

Node *stmt() {
    // assignを評価
    Node *node = assign();

    // 最後はTK_STMT(;)のはず
    if (!consume(TK_STMT)) {
        error("式の終わりが不正です. \";\"でありません: %s", get_token(pos)->input);
    }

    return node;
}

void program() {
    int i = 0;
    while(get_token(pos)->ty != TK_EOF) {
        code[i++] = stmt();
    }

    code[i] = NULL;
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
        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            Token *token = new_token(*p, p);
            vec_push(vec, token);

            i++;
            p++;
            continue;
        }

        // 変数(識別子) もしくは 関数呼び出し
        // 複数変数の文字列に対応するため、TK_IDENTに変数名を入れる
        // 変数名,関数名はアルファベット小文字のみとする
        // TODO 変数名の定義に数字や_-なども使えるようにする
        if ('a' <= *p && *p <= 'z') {
            char *pstart = p;
            int char_len = 0;
            while ('a' <= *p && *p <= 'z') {
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

