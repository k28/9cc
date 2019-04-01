// 9cc.h


// 可変長ベクタ
typedef struct {
    void **data;    // データ本体
    int capacity;   // バッファサイズ (data[0]からdata[capacity-1]がバッファの領域
    int len;        // ベクタに追加済みの要素の個数 (len == capacityの時にバッファがfull)
} Vector;

// トークンの型を表す値
enum {
    TK_NUM = 256,   // 整数トークン
    TK_IDENT,       // 識別子
    TK_ASSIGN,      // 代入 (=)
    TK_STMT,        // 式の終わり(;)
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
    ND_ASSIGN,      // =
};

typedef struct Node {
    int ty;             // 演算子がND_NUM
    struct Node *lhs;   // 左辺
    struct Node *rhs;   // 右辺
    int val;            // tyがND_NUMの場合のみ使う
    char *name;          // tyがND_IDENTの場合のみ使う
} Node;

// 関数のプロトタイプ宣言
Vector *new_vector();
void vec_push(Vector *vec, void *elem);

Token *new_token(int ty, char *input);
Token *new_token_num(char *input, int val);
Token *get_token(int pos);

Node *new_node(int ty, Node *lhs, Node *rhs);
Node *new_node_num(int val);

Node *term();
Node *mul();
Node *add();
Node *assign();
Node *stmt();
void program();

void tokenize(char *p);

void gen_lval(Node *node);
void gen(Node *node);
void error(char *message, char *s);

// テスト用のコード
void expect(int line, int expected, int actual);
int runtest();

