#include "9cc.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 生成規則
 * add: mul
 * add: add "+" mul
 * add: add "-" mul
 * mul: term
 * mul: mul "*" term
 * mul: mul "/" term
 * term: num
 * term: "(" add ")"
 *
 * num: digit
 * digit: "0" | "1" | "2" | "3" | "4" | "5 | "6" | "7" | "8" | "9"
 */

// トークナイズした結果のトークンを保持するベクター
Vector *tokens;

// 現在読んでいるトークンの場所
int pos = 0;

// Vectorを作成する
Vector *new_vector() {
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    return vec;
}

// vecに新しい要素elemを追加する
void vec_push(Vector *vec, void *elem) {
    if (vec->capacity == vec->len) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }

    vec->data[vec->len++] = elem;
}

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
        Node *node = add();
       if (!consume(')')) {
           error("開き括弧に対応する閉じ括弧がありません: %s", get_token(pos)->input);
       }

       return node;
    }

    if (get_token(pos)->ty == TK_NUM) {
        return new_node_num(get_token(pos++)->val);
    }

    error("数値でも開き括弧でもないトークンです: %s", get_token(pos)->input);
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

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            Token *token = new_token(*p, p);
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

// Nodeからスタックマシンを実現する
void gen(Node *node) {
    if (node->ty == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->ty) {
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

// エラー表示用関数
void error(char *message, char *s) {
    fprintf(stderr, message, s);
    exit(1);
}

void expect(int line, int expected, int actual) {
    if (expected == actual) {
        return;
    }

    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

int runtest() {
    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for (int i = 0; i < 100; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        vec_push(vec, (void *)val);
    }

    expect(__LINE__, 100, vec->len);
    expect(__LINE__, 0  , (int)*((int *)vec->data[0 ]));
    expect(__LINE__, 50 , (int)*((int *)vec->data[50]));
    expect(__LINE__, 99 , (int)*((int *)vec->data[99]));

    printf("OK\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // 引数 -test の時にテストコードを実行
    if (strcmp("-test", argv[1]) == 0) {
        runtest();
        return 0;
    }

    // トークナイズする
    tokenize(argv[1]);
    Node *node = add();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // 抽象構文木を下りながらコード生成
    gen(node);

    // スタックトップに式全体の値が残っているはずなので
    // それをRAXにロードして関数からの戻り値とする
    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}

