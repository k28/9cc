#include "9cc.h"

/*
 * 生成規則
 *
 * program: stmt program
 * program: ε
 *
 * stmt: assign ";"
 *
 * assign: add
 * assign: add "=" assign
 *
 * add: mul
 * add: add "+" mul
 * add: add "-" mul
 *
 * mul: term
 * mul: mul "*" term
 * mul: mul "/" term
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
        Node *node =  new_node(ND_IDENT, NULL, NULL);
        node->name = get_token(pos++)->input;
        return node;
    }

    error("数値でも開き括弧でもないトークンです: %s", get_token(pos)->input);
    return NULL;
}

// mul を生成 (掛け算, 割り算)
Node *mul() {
    Node *node = term();
    
    for (;;) {
        if (consume('*')) {
            node = new_node('*', node, term());
        } else if (consume('/')) {
            node = new_node('/', node, term());
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

// assignを生成 (代入)
Node *assign() {
    Node *node = add();

    for (;;) {
        if (consume(TK_ASSIGN)) {
            node = new_node(ND_ASSIGN, node, assign());
        } else {
            return node;
        }
    }
}

Node *stmt() {
    if (consume(TK_EOF)) {
        pos++;
        return NULL;
    }

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
    while(get_token(pos) != NULL) {
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

        // 変数(識別子)
        if ('a' <= *p && *p <= 'z') {
            Token *token = new_token(TK_IDENT, p);
            vec_push(vec, token);

            i++;
            p++;
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
