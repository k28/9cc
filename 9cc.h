// 9cc.h
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

#define SIZE_OF_ADDRESS (8)
#define SIZE_OF_INT     (4)
#define SIZE_OF_CHAR    (1)

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
    TK_RELATIONAL,  // 大小比較
    TK_EQUALITY,    // 等値
    TK_ASSIGN,      // 代入 (=)
    TK_COMMA,       // カンマ
    TK_STMT,        // 式の終わり(;)
    TK_LCBACKET,    // { 開く
    TK_RCBACKET,    // } 閉じる
    TK_IF,          // if 文
    TK_ELSE,        // else 文
    TK_WHILE,       // while 文
    TK_FOR,         // for 文
    TK_RETURN,      // return 文
    TK_INT,         // int 定義 
    TK_CHAR,        // char 定義 
    TK_SIZEOF,      // sizeof 演算子
    TK_STRING,      // 文字列トークン
    TK_EOF,         // 入力の終わりを表すトークン
};

// トークンの型
typedef struct {
    int ty;         // トークンの型
    int val;        // tyがTK_NUMの場合、その値
    char *input;    // トークン文字列A
    char *loc;      // エラー表示用の文字の場所
} Token;

enum {
    ND_NUM = 256,   // 整数のノードの型
    ND_IDENT,       // 識別子
    ND_GLOBAL_VAL,  // グローバル変数
    ND_FUNCCALL,    // 関数呼び出し
    ND_ARGUMENT,    // 関数の引数
    ND_ASSIGN,      // =
    ND_DEF_ARGUMENT,// 関数の引数定義
    ND_RELATIONAL,  // 大小比較
    ND_EQUALITY,    // 等値
    ND_IF,          // if文
    ND_WHILE,       // while文
    ND_FOR,         // for文
    ND_RETURN,      // return文
    ND_DEREFERENCE, // デリファレンス演算子
    ND_REFERENCE,   // リファレンス演算子 (&)
    ND_STRING,      // 文字列リテラル
    ND_BLOCK,       // ブロック
};

typedef struct Node {
    int ty;             // 演算子がND_NUM
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int val;            // tyがND_NUMの場合その値, tyがND_FUNCCALLの場合引数の数, ty がND_STRINGの時Vectorのindex
    char *name;         // tyがND_IDENT,ND_FUNCCALLの場合, 等値の場合に値が入る
    Vector *program;    // tyがif,for,whileの場合条件にマッチした時の処理が入る
    int    label;       // tyがif,for,whileの場合に使うラベル
    char *loc;          // エラー表示用
} Node;

typedef struct Function {
    char    *name;      // 関数名
    Vector  *arguments; // 引数 (Node)
    Vector  *code;      // 関数の先頭コード
    Map     *variables; // ローカル変数
    int     label;      // return文用のラベル
} Function;

// 型を表す構造体
typedef struct Type {
    enum { UNKNOWN, INT, CHAR, PTR, ARRAY } ty;
    struct Type *ptrof;
    size_t array_size;  // 配列の場合に配列のサイズ
} Type;

// 変数定義を表す構造体
typedef struct Variable {
    struct Type *type;
    int stack_offset;
} Variable;

// 文字列リテラルを表す構造体
typedef struct String {
    int index;  // Index
    char *val;  // 文字列
} String;

// トークナイズした結果のトークンを保持するベクター
extern Vector *tokens;
extern Vector *functions;           // 関数を保持するためのベクター
extern Vector *strings_;            // 文字列リテラルをいれるベクター
extern Map    *variables;           // ローカル変数の種類を保持するためのMap
extern Map    *global_variables_;   // グローバル変数を保持するMap
extern int    label_;               // if文などで使用するラベル番号
extern int    return_label_;        // return文の飛び先ラベル
extern int    current_pointer_offset_;// 現在のポインタ演算のオフセット(ただの演算の場合は1になる)
extern char   *source_;             // 入力プログラム
extern char   *filename_;            // 入力プログラムのファイル名

// 現在読んでいるトークンの場所
extern int pos;


// 関数のプロトタイプ宣言
Vector *new_vector();
Variable *new_variable(Type *type, int offset); // container.c
void vec_push(Vector *vec, void *elem);

Map *new_map();
void map_put(Map *map, char *key, void *val);
void *map_get(Map *map, char *key);
void *map_get_at_index(Map *map, int index);
char *map_get_key_at_index(Map *map, int index);
int get_map_size(Map *map);

// parse.c
Token *new_token(int ty, char *input, char *pos);
Token *new_token_num(char *input, int val, char *pos);
Token *get_token(int pos);

Node *new_node(int ty, Node *lhs, Node *rhs, char *loc);
Node *new_node_num(int val, char *loc);

Type *new_type(int ty, Type *ptrof);


int consume_not_add(int index, int ty);
int consume(int ty);

int sizeof_node(Node *node);
Node *term();
Node *unary();
Node *func();
Node *mul();
Node *add();
Node *relational();
Node *equality();
Node *assign();
Node *return_node();
Node *argument(int *count_of_arguments);
Node *ifstmt();
Node *while_stmt();
Node *for_stmt();
Node *def_variable(int ty);
Node *blockstmt();
Node *expr();
Node *stmt();
Vector *func_body();
Vector *def_argument(char *func_name);
Function *def_function();
void def_global_variable();
void parse();

int is_ident_word(char *p);
void tokenize(char *p);

int size_of_variale(Variable *variable);
int address_offset(Variable *variable, char* name);

void gen_relational(Node *node);
void gen_equality(Node *node);
void gen_lval(Node *node);
void gen(Node *node);
void gen_function_variables(Function *function);
void gen_function(Function *function);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

// sema
int offset_of_variable(Variable *val_info);
void walk(Node *node);
void sema();

// file
char *read_file(char *path);

// テスト用のコード
void expect(int line, int expected, int actual);
void test_vector();
void test_map();
void test_type();

