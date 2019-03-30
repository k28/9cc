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
 *
 */

// トークンの型を表す値
enum {
    TK_NUM = 256,   // 整数トークン
    TK_EOF,         // 入力の終わりを表すトークン
};

// トークンの型
typedef struct {
    int ty;         // トークンの型
    int val;        // tyがTK_NUMの場合、その値
    char *input;    // トークン文字列(エラーメッセージ用)
} Token;

enum {
    ND_NUM = 256,   // 整数のノードの型
};

typedef struct Node {
    int ty;             // 演算子がND_NUM
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int val;            // tyがND_NUMの場合のみ使う
} Node;

// 関数のプロトタイプ宣言
Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);
Node *term();
Node *mul();
Node *add();
void tokenize(char *p);
void gen(Node *node);
void error(char *message, char *s);


// トークナイズした結果のトークン列はこの配列に保存する
// 100個以上のトークンはこないものとする
Token tokens[100];

// 現在読んでいるトークンの場所
int pos = 0;

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
    if (tokens[pos].ty != ty) {
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
           error("開き括弧に対応する閉じ括弧がありません: %s", tokens[pos].input);
       }

       return node;
    }

    if (tokens[pos].ty == TK_NUM) {
        return new_node_num(tokens[pos++].val);
    }

    error("数値でも開き括弧でもないトークンです: %s", tokens[pos].input);
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
    int i = 0;
    while (*p) {
        // 空白文字をスキップ
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
            tokens[i].ty = *p;
            tokens[i].input = p;
            i++;
            p++;
            continue;
        }

        if (isdigit(*p)) {
            tokens[i].ty = TK_NUM;
            tokens[i].input = p;
            tokens[i].val = strtol(p, &p, 10);
            i++;
            continue;
        }

        fprintf(stderr, "トークナイズできません: %s\n", p);
        exit(1);
    }

    tokens[i].ty = TK_EOF;
    tokens[i].input = p;
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

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "引数の個数が正しくありません\n");
        return 1;
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

