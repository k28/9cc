// 9cc.h
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_CODE_NUM    (100)
#define SIZE_OF_ADDRESS (8)

// 可変長ベクタ
typedef struct {
    void **data;    // データ本体
    int capacity;   // バッファサイズ (data[0]からdata[capacity-1]がバッファの領域
    int len;        // ベクタに追加済みの要素の個数 (len == capacityの時にバッファがfull)
} Vector;

// 連想配列
typedef struct {
    Vector *keys;
    Vector *vals;
} Map;

// トークンの型を表す値
enum {
    TK_NUM = 256,   // 整数トークン
    TK_IDENT,       // 識別子
    TK_EQUALITY,    // 等値
    TK_ASSIGN,      // 代入 (=)
    TK_COMMA,       // カンマ
    TK_STMT,        // 式の終わり(;)
    TK_LCBACKET,    // { 開く
    TK_RCBACKET,    // } 閉じる
    TK_IF,          // if 文
    TK_WHILE,       // while 文
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
    ND_IDENT,       // 識別子
    ND_FUNCCALL,    // 関数呼び出し
    ND_ARGUMENT,    // 関数の引数
    ND_ASSIGN,      // =
    ND_DEF_ARGUMENT,// 関数の引数定義
    ND_EQUALITY,    // 等値
    ND_IF,          // if文
    ND_WHILE,       // while文
};

typedef struct Node {
    int ty;             // 演算子がND_NUM
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int val;            // tyがND_NUMの場合その値, tyがND_FUNCCALLの場合引数の数
    char *name;         // tyがND_IDENT,ND_FUNCCALLの場合, 等値の場合に値が入る
    Vector *program;    // tyがND_IFの場合条件にマッチした時の処理が入る
    int    label;       // tyがND_IFの場合に使うラベル
} Node;

typedef struct Function {
    char    *name;      // 関数名
    Vector  *arguments; // 引数 (Node)
    Vector  *code;      // 関数の先頭コード
    Map     *variables; // ローカル変数
} Function;

// トークナイズした結果のトークンを保持するベクター
extern Vector *tokens;
extern Vector *functions;       // 関数を保持するためのベクター
extern Map    *variables;       // ローカル変数の種類を保持するためのMap
extern int    label_;            // if文などで使用するラベル番号

// 現在読んでいるトークンの場所
extern int pos;


// 関数のプロトタイプ宣言
Vector *new_vector();
void vec_push(Vector *vec, void *elem);

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);

Token *new_token(int ty, char *input);
Token *new_token_num(char *input, int val);
Token *get_token(int pos);

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);

int consume_not_add(int index, int ty);
int consume(int ty);

Node *term();
Node *func();
Node *mul();
Node *add();
Node *equality();
Vector *func_body();
Node *argument(int *count_of_arguments);
Node *assign();
Node *ifstmt();
Node *while_stmt();
Node *stmt();
Vector *def_argument(char *func_name);
Function *def_function();
void program();

void tokenize(char *p);

void gen_equality(Node *node);
void gen_lval(Node *node);
void gen(Node *node);
void gen_function_variables(Function *function);
void gen_function(Function *function);
void error(char *message, char *s);

// テスト用のコード
void expect(int line, int expected, int actual);
void test_vector();
void test_map();

