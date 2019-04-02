#include "9cc.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

// トークナイズした結果のトークンを保持するベクター
Vector *tokens;
Node *code[100];

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

// 新しいMapを作成する
Map *new_map() {
    Map *map = malloc(sizeof(Map));
    map->keys = new_vector();
    map->vals = new_vector();
    return map;
}

// Mapに値を追加する
void map_put(Map *map, char *key, void *val) {
    vec_push(map->keys, key);
    vec_push(map->vals, val);
}

void *map_get(Map *map, char *key) {
    for (int i =  map->keys->len - 1; i >= 0; i--) {
        if (strcmp(key, map->keys->data[i]) == 0) {
            return map->vals->data[i];
        }
    }

    return NULL;
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

// 与えられたノードが変数を表しているときに、その変数のアドレスを計算してスタックにプッシュする
void gen_lval(Node *node) {
    if (node->ty != ND_IDENT) {
        error("代入の左辺値が変数ではありません","");
    }

    int offset = ('z' - *(node->name) + 1) * 8;
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", offset);
    printf("  push rax\n");
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

    if (node->ty == ND_IDENT) {
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    }

    if (node->ty == ND_ASSIGN) {
        // assignの時には
        // 左の変数に右の値を入れる
        // 左の変数をgen_lvalでスタックに入れておいてから
        // genで右辺を評価して結果を代入する
        gen_lval(node->lhs);
        gen(node->rhs);
        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
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

int test_vector() {
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

    printf("test_vector OK\n");
}

void test_map() {
    Map *map = new_map();
    expect(__LINE__, 0, (intptr_t)map_get(map, "foo"));
    
    map_put(map, "foo", (void *)2);
    expect(__LINE__, 2, (intptr_t)map_get(map, "foo"));
    
    map_put(map, "bar", (void *)4);
    expect(__LINE__, 4, (intptr_t)map_get(map, "bar"));
    
    map_put(map, "foo", (void *)6);
    expect(__LINE__, 6, (intptr_t)map_get(map, "foo"));

    printf("test_map OK\n");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
    }

    // 引数 -test の時にテストコードを実行
    if (strcmp("-test", argv[1]) == 0) {
        test_vector();
        test_map();
        return 0;
    }

    // トークナイズする
    tokenize(argv[1]);
    program();

    pos = 0;
    Node *node = add();

    // アセンブリの前半部分を出力
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    /*
     * 現在のコンパイラは関数1つのみの想定なので、
     * プロローグとエピローグが最初と最後に来ている
     */

    // プロローグ
    // 変数26個分の領域を確保する
    printf("  push rbp\n");
    printf("  mov rbp, rsp\n");
    printf("  sub rsp, 208\n");

    // 抽象構文木を下りながらコード生成
    for (int i = 0; code[i]; i++) {

        gen(code[i]);

        // 式の評価結果としてスタックに1つの値が残っているはずなので
        // スタックが溢れないようにポップしておく
        printf("  pop rax\n");

    }

    // エピローグ
    // 最後の式の結果がRAXに残っているのでそれが帰り値になる
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");

    return 0;
}

