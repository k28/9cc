#include "9cc.h"

/*
 * 生成規則
 *
 * program: def_func
 * program: def_func def_func
 * program: ε
 *
 * def_func: model ident "(" def_argument ")" "{" func_body "}"
 *
 * def_argument: model ident
 * def_argument: model ident "," def_argument
 * def_argument: ε
 *
 * func_body: stmt func_body
 * func_body: ε
 *
 * stmt: return ";"
 * stmt: def_variable ";"
 * stmt: if
 * stmt: while
 * stmt: for
 *
 * return: assign
 * return: "return" assign
 *
 * assign: equality
 * assign: equality "=" assign
 *
 * equality: relational
 * equality: relational "==" equality
 * equality: relational "!=" equality
 *
 * relational: add
 * relational: "<"  add
 * relational: "<=" add
 * relational: ">"  add
 * relational: ">=" add
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
 * func: unary
 *
 * unary: ("sizeof") unary | ("+" | "-")? term | term "[" assign "]"
 *
 * argument: equality
 * argument: equality "," equality
 * argument: ε
 *
 * term: num
 * term: ident
 * term: "&" term
 * term: "*" term
 * term: "(" assign ")"
 * term: string
 *
 * digit: "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
 * ident: "a" - "z"
 * model: "int", "char"
 *
 * def_variable: model "*"* ident ";"
 * def_variable: model ident "[" num "]" ";"
 *
 * if ( assign ) {
 *     func_body
 * }
 *
 * while ( assign ) {
 *     func_body
 * }
 *
 * for (assign; equality; assign) {
 *     func_body
 * }
 *
 */

// 現在パース中の関数のローカル変数を保持する
Map *variables;

// 文字Tokenを作成する
//
// ty: Type
// input: 入力
// loc: エラー出力用
Token *new_token(int ty, char *input, char *loc) {
    Token *token = malloc(sizeof(Token));
    token->ty = ty;
    token->input = input;
    token->loc = loc;
    return token;
}

// 数値Tokenを作成する
Token *new_token_num(char *input, int val, char *loc) {
    Token *token = malloc(sizeof(Token));
    token->ty = TK_NUM;
    token->input = input;
    token->val = val;
    token->loc = loc;
    return token;
}

// vectorからTokenを取得する, 取得できない時にはNULLを返す
Token *get_token(int pos) {
    if (tokens_->len == pos) {
        fprintf(stderr, "error : index out of bounds %d", pos);
        return NULL;
    }

    Token *token = (Token *) tokens_->data[pos];
    return token;
}

// vectorからTokenを削除する
void remove_token(int pos) {
    vec_remove(tokens_, pos);
}

// Nodeを作成する
Node *new_node(int ty, Node *lhs, Node *rhs, char *loc) {
    Node *node = malloc(sizeof(Node));
    node->ty = ty;
    node->lhs = lhs;
    node->rhs = rhs;
    node->loc = loc;
    return node;
}

// 数値Nodeを作成する
Node *new_node_num(int val, char *loc) {
    Node *node = malloc(sizeof(Node));
    node->ty = ND_NUM;
    node->val = val;
    node->loc = loc;
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

// Nodeの型を見てsizeofで返す値を決める
int sizeof_node(Node *node) {
    if (node == NULL) {
        error_at(node->loc, "定義されていないデータ型です");
    }

    if (node->ty == ND_NUM) return SIZE_OF_INT;

    if (node->ty == ND_IDENT) {
        // 変数の型を確認する
        Variable *val_info = map_get(variables, node->name);
        if (val_info->type->ty == CHAR)  return SIZE_OF_CHAR;
        if (val_info->type->ty == INT)   return SIZE_OF_INT;
        if (val_info->type->ty == PTR)   return SIZE_OF_ADDRESS;
        if (val_info->type->ty == ARRAY) return val_info->type->array_size;

        // 多分ここにきたら実装もれ
        error_at(node->loc, "定義されていない変数型です");
    }

    if (node->ty == ND_DEREFERENCE) return SIZE_OF_INT;
    if (node->ty == ND_REFERENCE)   return SIZE_OF_ADDRESS;

    // 計算の場合は左辺の値を採用
    return sizeof_node(node->lhs);
}

// termを生成 (括弧)
Node *term() {
    if (consume('(')) {
        Node *node = assign();
       if (!consume(')')) {
           error_at(get_token(pos)->loc, "開き括弧に対応する閉じ括弧がありません");
       }

       return node;
    }

    Token *token = get_token(pos);
    if (token->ty == '*') {
        // デリファレンス演算子
        // 中の構文を右辺値としてコンパイルする
        pos++;
        Node *rhs_node = term();
        Node *node = new_node(ND_DEREFERENCE, NULL, rhs_node, token->loc);
        return node;
    }

    if (token->ty == '&') {
        // リファレンス演算子
        // 中の構文を右辺値としてコンパイルする
        pos++;
        Node *rhs_node = term();
        Node *node = new_node(ND_REFERENCE, NULL, rhs_node, token->loc);
        return node;
    }

    if (token->ty == TK_NUM) {
        return new_node_num(get_token(pos++)->val, token->loc);
    }

    if (token->ty == TK_STRING) {
        // 文字リテラル
        // 文字列リテラルを入れておくVectorに全て入れておく
        char *val = get_token(pos++)->input;
        String *string = (String *)malloc(sizeof(String));
        string->index = strings_->len;
        string->val   = val;
        vec_push(strings_, string);

        // NodeにはVectorに入れたStringのIndexを設定しておく
        Node *node = new_node(ND_STRING, NULL, NULL, token->loc);
        node->name = val;
        node->val  = string->index;
        return node;
    }

    if (token->ty == TK_IDENT) {
        // 名称を取得しておく
        char *name = get_token(pos++)->input;
        // IDENTの変数定義を取得
        Variable *val_info = map_get(variables, name);
        if (val_info == NULL) {
            // グローバル変数に定義されているか確認する
            val_info = map_get(global_variables_, name);
            if (val_info) {
                // グローバル変数ノードとして追加
                Node *node = new_node(ND_GLOBAL_VAL, NULL, NULL, token->loc);
                node->name = name;
                return node;
            }
            error_at(token->loc, "定義されていない変数です");
        }

        // 変数
        Node *node = new_node(ND_IDENT, NULL, NULL, token->loc);
        node->name = name;
        return node;
    }

    error_at(get_token(pos)->loc, "数値でも開き括弧でもないトークンです");
    return NULL;
}

Node *unary() {

    if (consume('+')) {
        // +x を x に置き換え
        return term();
    }

    if (consume('-')) {
        // -x を 0 - x に置き換え
        char *loc = get_token(pos-1)->loc;
        return new_node('-', new_node_num(0, loc), term(), loc);
    }

    if (consume(TK_SIZEOF)) {
        // sizeof演算子
        Node *node = unary();
        // Nodeの型を見て、値を数値に置き換える
        int size = sizeof_node(node);
        char *loc = get_token(pos-1)->loc;
        return new_node_num(size, loc);
    }

    Token *token = get_token(pos);
    if ((token->ty == TK_NUM || token->ty == TK_IDENT)
        && get_token(pos+1)->ty == '[') {
        //  配列の添字は以下のように変換する
        //  x[y] => *(x + y)
        Node *first = term();
        pos++;
        Node *second = assign();
        if(!consume(']')) {
            error_at(get_token(pos)->loc, "配列の添字の定義が不正です");
        }

        Node *add_node = new_node('+', first, second, token->loc);
        Node *node = new_node(ND_DEREFERENCE, NULL, add_node, token->loc);
        return node;
    }

    return term();
}

// funcを生成 (関数呼び出し)
Node *func() {
    Token *token = get_token(pos);
    if (token->ty == TK_IDENT && consume_not_add(pos + 1, '(')) {
        // 名称を取得
        char *name = get_token(pos++)->input;
        pos++;  // 前カッコ "(" 分進める
        // 引数の数を計測するための変数をpush
        int count_of_arguments = 0;
        Node *node = new_node(ND_FUNCCALL, NULL, argument(&count_of_arguments), token->loc);
        node->name = name;
        // valに引数の数を入れる
        node->val = count_of_arguments;

        if (!consume(')')) {
            error_at(get_token(pos)->loc, "関数呼び出しに対応する 開き括弧に対応する閉じ括弧がありません");
        }
        return node;
    }

    return unary();
}

// mul を生成 (掛け算, 割り算)
Node *mul() {
    Node *node = func();
    
    for (;;) {
        Token *token = get_token(pos);
        if (consume('*')) {
            node = new_node('*', node, func(), token->loc);
        } else if (consume('/')) {
            node = new_node('/', node, func(), token->loc);
        } else {
            return node;
        }
    }
}

// addを生成 足し算 / 引き算
Node *add() {
    Node *node = mul();

    for (;;) {
        Token *token = get_token(pos);
        if (consume('+')) {
            node = new_node('+', node, mul(), token->loc);
        } else if (consume('-')) {
            node = new_node('-', node, mul(), token->loc);
        } else {
            return node;
        }
    }
}

// 大小比較
Node *relational() {
    Node *node = add();

    for (;;) {
        Token *token = get_token(pos);
        if (token->ty == TK_RELATIONAL) {
            char *name = get_token(pos++)->input;
            node = new_node(ND_RELATIONAL, node, add(), token->loc);
            node->name = name;
        } else {
            return node;
        }
    }
}

// 等値
Node *equality() {
    Node *node = relational();

    for (;;) {
        Token *token = get_token(pos);
        if (token->ty == TK_EQUALITY) {
            char *name = get_token(pos++)->input;
            node = new_node(ND_EQUALITY, node, relational(), token->loc);
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
        Token *token = get_token(pos);
        if (consume(TK_ASSIGN)) {
            node = new_node(ND_ASSIGN, node, equality(), token->loc);
        } else {
            return node;
        }
    }
}

Node *return_node() {
    Token *token = get_token(pos);
    if (consume(TK_RETURN)) {
        // return statement
        Node *node = new_node(ND_RETURN, NULL, assign(), token->loc);
        return node;
    }

    return assign();
}

Node *argument(int *count_of_arguments) {
    if (consume_not_add(pos, ')')) {
        // エラーチェックのため 上位で閉じ括弧の有無をチェックする
        return NULL;
    }

    *count_of_arguments += 1;
    Node *node = equality();
    for (;;) {
        Token *token = get_token(pos);
        if (consume(TK_COMMA)) {
            *count_of_arguments += 1;
            node = new_node(ND_ARGUMENT, node, equality(), token->loc);
        } else {
            return node;
        }
    }
}

// if文のNodeを生成
Node *ifstmt() {
    Token *token = get_token(pos);
    if (!consume('(')) {
        error_at(get_token(pos)->loc, "if文の開き括弧がありません.");
    }

    // 条件式
    // TODO while, forなどでもexprを使った方が良い
    Node *condition_node = expr();

    if (!consume(')')) {
        error_at(get_token(pos)->loc, "if文の閉じ括弧がありません.");
    }

    Vector *body_vec = NULL;

    // { があるか確認
    if (consume(TK_LCBACKET)) {
        body_vec = func_body();
        if (!consume(TK_RCBACKET)) {
            error_at(get_token(pos)->loc, "if文の終わりが不正です. \"}\"でありません");
        }
    } else {
        // 1文のみのBody
        Node *assign_node = return_node();
        body_vec = new_vector();
        vec_push(body_vec, assign_node);
        // 最後はTK_STMT(;)のはず
        if (!consume(TK_STMT)) {
            error_at(get_token(pos)->loc, "if文の終わりが不正です. \";\"でありません");
        }
    }

    // else 句があるか確認
    Node *else_node = NULL;
    if (consume(TK_ELSE)) {
        else_node = stmt();
    }

    // IFのNodeを作成
    Node *node = new_node(ND_IF, condition_node, else_node, token->loc);
    node->program = body_vec;
    node->label = label_++;

    return node;
}

// while文のノードを作成
Node *while_stmt() {
    Token *token = get_token(pos);
    if (!consume('(')) {
        error_at(get_token(pos)->loc, "while文の開き括弧がありません.");
    }

    // 条件式
    Node *condition_node = equality();

    if (!consume(')')) {
        error_at(get_token(pos)->loc,"while文の閉じ括弧がありません.");
    }

    Vector *body_vec = NULL;

    // { があるか確認
    if (consume(TK_LCBACKET)) {
        body_vec = func_body();
        if (!consume(TK_RCBACKET)) {
            error_at(get_token(pos)->loc, "while文の終わりが不正です. \"}\"でありません");
        }
    } else {
        // 1文のみのBody
        Node *assign_node = return_node();
        body_vec = new_vector();
        vec_push(body_vec, assign_node);
        // 最後はTK_STMT(;)のはず
        if (!consume(TK_STMT)) {
            error_at(get_token(pos)->loc, "while文の終わりが不正です. \";\"でありません");
        }
    }

    // WHILEのNodeを作成
    Node *node = new_node(ND_WHILE, condition_node, NULL, token->loc);
    node->program = body_vec;
    node->label = label_++;

    return node;
}

Node *for_stmt() {
    Token *token = get_token(pos);
    if (!consume('(')) {
        error_at(get_token(pos)->loc, "for文の開き括弧がありません.");
    }

    Node *first_assign = NULL;
    Node *condition_node = NULL;
    Node *last_assign = NULL;

    // for文の括弧内の定義を取得
    if (!consume(TK_STMT)) {
        // assign
        first_assign = assign();
        if (!consume(TK_STMT)) {
            error_at(get_token(pos)->loc, "for文 括弧内の定義が不正です.");
        }
    }

    if (!consume(TK_STMT)) {
        // 条件式
        condition_node = equality();
        if (!consume(TK_STMT)) {
            error_at(get_token(pos)->loc, "for文 括弧内の定義が不正です.");
        }
    }

    if (!consume(TK_STMT) && !consume_not_add(pos, ')')) {
        // add
        last_assign = assign();
        // if (!consume(TK_STMT)) {
        //     error_at(get_token(pos)->loc, "for文 括弧内の定義が不正です.");
        // }
    }

    if (!consume(')')) {
        error_at(get_token(pos)->loc, "for文の閉じ括弧がありません.");
    }

    // bodyをパース
    Vector *body_vec = NULL;
    if (consume(TK_LCBACKET)) {
        body_vec = func_body();
        if (!consume(TK_RCBACKET)) {
            error_at(get_token(pos)->loc, "for文の終わりが不正です. \"}\"でありません");
        }
    }

    if (last_assign != NULL) {
        // last_assignはbodyの最後に実行すれば良いので
        // bodyの最後に追加する
        vec_push(body_vec, last_assign);
    }

    // FORのNodeを作成
    // FORのNodeは左辺に初期設定, 右辺に条件を入れる
    Node *node = new_node(ND_FOR, first_assign, condition_node, token->loc);
    node->program = body_vec;
    node->label = label_++;

    return node;
}

Node *def_variable(int ty) {
    // int hoge;

    // このデータの型
    Type *type = new_type(ty, NULL);
    // *があればtypeに足していく
    while (consume('*')) {
        Type *ptr_type = new_type(PTR, type);
        type = ptr_type;
    }

    Token *token = get_token(pos);
    if (token->ty == TK_IDENT) {
        // 名称を取得しておく
        char *name = token->input;
        // 名称分posを進める
        pos++;
        // 変数
        Node *node = new_node(ND_IDENT, NULL, NULL, token->loc);
        node->name = name;

        if (consume('[')) {
            // 配列定義
            Token *num_token = get_token(pos);
            if (num_token->ty != TK_NUM) {
                error_at(get_token(pos)->loc, "配列の定義が不正です.");
            }
            Type *ptr_type = new_type(ARRAY, type);
            ptr_type->array_size = num_token->val;  // 配列のサイズを設定
            type = ptr_type;
            pos++;
            if (consume(']') == 0) {
                error_at(get_token(pos)->loc, "配列の定義が不正です. 閉じ括弧がありません.");
            }
        }

        // 変数の数を数えるためにMapに値を入れる
        // variablesに登録されていない変数はスタックに積む必要がある
        Variable *val_info = map_get(variables, node->name);
        if (val_info == NULL) {
            // 変数の出現順に1足して、スタック上のオフセットにする
            int stack_offset = variables->keys->len + 1;
            val_info = new_variable(type, stack_offset);
            map_put(variables, node->name, val_info);
        } else {
            // 2重に定義されている...
        }

        // 最後はTK_STMT(;)のはず
        if (!consume(TK_STMT)) {
            error_at(get_token(pos)->loc, "変数定義の終わりが不正です. \";\"でありません.");
        }

        return node;
    }

    error_at(get_token(pos)->loc, "変数定義が不正です.");
    return NULL;
}

// block分のNodeを生成
Node *blockstmt() {
    Token *token = get_token(pos);
    Vector *body_vec = func_body();
    if (!consume(TK_RCBACKET)) {
        error_at(get_token(pos)->loc, "if文の終わりが不正です. \"}\"でありません.");
    }

    Node *node = new_node(ND_BLOCK, NULL, NULL, token->loc);
    node->program = body_vec;

    return node;
}

Node *expr() {
    return assign();
}

Node *stmt() {

    if (consume(TK_LCBACKET)) {
        // ブロック { .. }
        return blockstmt();
    }

    if (consume(TK_IF)) {
        // if文
        return ifstmt();
    }

    if (consume(TK_WHILE)) {
        // while文
        return while_stmt();
    }

    if (consume(TK_FOR)) {
        // while文
        return for_stmt();
    }

    if (consume(TK_INT)) {
        // 変数定義
        return def_variable(INT);
    }

    if (consume(TK_CHAR)) {
        // 変数定義
        return def_variable(CHAR);
    }


    // Node *node;
    // if (consume(TK_RETURN)) {
    //     node = new_node(ND_RETURN, expr(), NULL);
    // } else {
    //     node = expr();
    // }

    // returnを評価
    Node *node = return_node();

    // 最後はTK_STMT(;)のはず
    if (!consume(TK_STMT)) {
        error_at(get_token(pos)->loc, "式の終わりが不正です. \";\"でありません.");
    }

    return node;
}

Vector *func_body() {
    Vector *code = new_vector();
    while(get_token(pos)->ty != TK_RCBACKET) {
        Node *node = stmt();
        if (node) {
            vec_push(code, node);
        }
    }

    return code;
}

// 引数の数を数えて返す
Vector *def_argument(char *func_name) {
    Vector *arguments = new_vector();

    for (;;) {
        int arg_type = UNKNOWN;
        if (consume(TK_INT)) {
            arg_type = INT;
        } else if (consume(TK_CHAR)) {
            arg_type = CHAR;
        }
        if (arg_type != UNKNOWN) {
            // このデータの型
            Type *type = new_type(arg_type, NULL);
            // *があればtypeに足していく
            while (consume('*')) {
                Type *ptr_type = new_type(PTR, type);
                type = ptr_type;
            }

            Token *token = get_token(pos);
            if(token->ty == TK_IDENT) {
                // 変数名
                char *name = get_token(pos++)->input;
                // 関数定義の変数はローカル変数と同じ扱いにする
                Variable *val_info = map_get(variables, name);
                if (val_info == NULL) {
                    // 変数の出現順に1足して、スタック上のオフセットにする
                    int stack_offset = variables->keys->len + 1;
                    val_info = new_variable(type, stack_offset);
                    map_put(variables, name, val_info);

                    Node *node = new_node(ND_IDENT, NULL, NULL, token->loc);
                    node->name = name;
                    vec_push(arguments, node);
                }
                continue;
            }
        }

        if (consume(TK_COMMA)) {
            continue;
        }

        if (consume(')')) {
            break;
        }

        // ここに来たら関数の引数定義としておかしい
        error_at(get_token(pos)->loc, "引数定義が不正です。");
    }

    return arguments;
}

// 関数定義
Function *def_function() {
    // if (consume(TK_INT) && get_token(pos)->ty == TK_IDENT && consume_not_add(pos + 1, '(')) {
        // 関数名を取得
        char *name = get_token(pos++)->input;
        pos++;  // 前カッコ "(" 分進める

        // グローバル変数に
        // 現在パース中のローカル変数保持用の領域をセットする
        Map *local_variables = new_map();
        variables = local_variables;

        // 引数の数を計測するための変数をpush
        Vector *arguments = def_argument(name);

        // 引数部分の定義の後は {}で囲まれたBody部
        if (!consume(TK_LCBACKET)) {
            error_at(get_token(pos)->loc, "関数定義に対応する 開きブランケットがありません");
        }

        // Bodyのパース
        Vector *body = func_body();

        // TODO 関数に分ける
        Function *function = (Function *)malloc(sizeof(Function));
        function->name = name;
        function->code = body;
        function->arguments = arguments;
        function->variables = local_variables;
        function->label = label_++;

        if (!consume(TK_RCBACKET)) {
            error_at(get_token(pos)->loc, "関数定義に対応する 閉じブランケットがありません");
        }
        return function;
    // }

    // error("関数定義が不正です :%s\n", get_token(pos)->input);
    // return NULL;
}

void def_global_variable(int ty) {
    // このデータの型
    Type *type = new_type(ty, NULL);
    // *があれば足していく
    while (consume('*')) {
        Type *ptr_type = new_type(PTR, type);
        type = ptr_type;
    }

    if (get_token(pos)->ty == TK_IDENT) {
        // 名称を取得しておく
        char *name = get_token(pos++)->input;

        if (consume('[')) {
            // 配列定義
            Token *num_token = get_token(pos);
            if (num_token->ty != TK_NUM) {
                error_at(get_token(pos)->loc, "配列の定義が不正です.");
            }
            Type *ptr_type = new_type(ARRAY, type);
            ptr_type->array_size = num_token->val;  // 配列のサイズを設定
            type = ptr_type;
            pos++;
            if (consume(']') == 0) {
                error_at(get_token(pos)->loc, "配列の定義が不正です. 閉じ括弧がありません.");
            }
        }

        Variable *val_info = NULL;
        if (get_token(pos)->ty == TK_ASSIGN) {
            pos++;
            // 初期化のあるグローバル変数
            // 単なるバイト列か関数かグローバル変数へのポインタ
            if (get_token(pos)->ty == TK_NUM) {
                // 数値で初期化, 数値の初期値をval_infoに持たせる
                int *val = (int *)malloc(sizeof(int));
                *val = get_token(pos++)->val;
                val_info = new_global_variable(type, 0, val);
            } else if (get_token(pos)->ty == TK_STRING) {
                // 文字列で初期化
               val_info = new_variable(type, 0);
            } else if (get_token(pos)->ty == '&') {
                // グローバル変数へのポインタ
               val_info = new_variable(type, 0);
            } else if (get_token(pos)->ty == TK_IDENT) {
                // グローバル変数へのポインタ(配列)
                error_at(get_token(pos)->loc, "この定義にはまだ対応していません。");
            }
        }

        // グローバル変数はMapに入れる
        map_put(global_variables_, name, val_info);
    }

    // 最後はTK_STMT(;)のはず
    if (!consume(TK_STMT)) {
        error_at(get_token(pos)->loc, "変数定義の終わりが不正です. \";\"でありません");
    }
}

void parse() {
    while(get_token(pos)->ty != TK_EOF) {
        if (consume(TK_INT) && get_token(pos)->ty == TK_IDENT) {
            if (consume_not_add(pos + 1, '(')) {
                // ( があれば関数定義
                Function *function = def_function();
                if (function) {
                    vec_push(functions_, function);
                }
            } else {
                // グローバル変数
                def_global_variable(INT);
            }
        } else if (consume(TK_CHAR) && get_token(pos)->ty == TK_IDENT) {
            if (consume_not_add(pos + 1, '(')) {
                // ( があれば関数定義
                Function *function = def_function();
                if (function) {
                    vec_push(functions_, function);
                }
            } else {
                // グローバル変数
                def_global_variable(CHAR);
            }
        } else {
            error_at(get_token(pos)->loc, "関数またはグローバル変数の定義が不正です");
        }
    }
}

