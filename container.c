#include "9cc.h"
/**
 * ベクタ, マップ, およびそのテストコード
 */

// Vectorを作成する
Vector *new_vector() {
    Vector *vec = malloc(sizeof(Vector));
    vec->data = malloc(sizeof(void *) * 16);
    vec->capacity = 16;
    vec->len = 0;
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

// elemをindexの位置にいれる
 void vec_insert(Vector *vec, void *elem, int index) {
     if (vec->len <= index) {
         vec_push(vec, elem);
         return;
     }

     void *tmp = vec->data[index];
     vec->data[index] = elem;
     vec_insert(vec, tmp, ++index);
}

// vecからindexの物を削除する
void vec_remove(Vector *vec, int index) {
    if ((vec->len - 1) < index) {
        fprintf(stderr, "error : index out of bounds %d", index);
        exit(1);
    }

    // indexから後ろのデータを前に詰める
    int len = vec->len;
    for (int i = index; i < len - 1; i++) {
        vec->data[i] = vec->data[i + 1];
    }
    vec->len -= 1;
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
    for (int i =  map->keys->len - 1; i >= 0; i--) {
        if (strcmp(key, map->keys->data[i]) == 0) {
            map->vals->data[i] = val;
            return;
        }
    }

    vec_push(map->keys, key);
    vec_push(map->vals, val);
}

void *map_get(Map *map, char *key) {
    if (key == NULL) return NULL;
    for (int i =  map->keys->len - 1; i >= 0; i--) {
        if (strcmp(map->keys->data[i], key) == 0) {
            return map->vals->data[i];
        }
    }

    return NULL;
}

// Mapのindex番目の要素を返す
void *map_get_at_index(Map *map, int index) {
    if (index < map->vals->len) {
        return map->vals->data[index];
    }

    return NULL;
}

// Mapのindex番目のKey要素を返す
char *map_get_key_at_index(Map *map, int index) {
    if (index < map->vals->len) {
        return map->keys->data[index];
    }

    return NULL;
}

// Mapのサイズを返す
int get_map_size(Map *map) {
    return map->vals->len;
}

Type *new_type(int ty, Type *ptrof) {
    Type *type = malloc(sizeof(Type));
    type->ty = ty;
    type->ptrof = ptrof;
    return type;
}

Variable *new_variable(Type *type, int offset) {
    Variable *value = malloc(sizeof(Variable));
    value->type = type;
    value->stack_offset = offset;
    value->initial_value_ = NULL;
    return value;
}

Variable *new_global_variable(Type *type, int offset, void* initial_value) {
    Variable *value = malloc(sizeof(Variable));
    value->type = type;
    value->stack_offset = offset;
    value->initial_value_ = initial_value;
    return value;
}

// エラー表示用関数
// void error(char *message, char *s) {
//     fprintf(stderr, message, s);
//     exit(1);
// }

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// 引数でプログラムを受け取った時のエラーメッセージを表示する
void error_at_nofile(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int max_pos = strlen(source_);
    int pos = loc - source_;
    if (pos < 0)
        pos = 0;
    if (pos > max_pos)
        pos = max_pos;
    fprintf(stderr, "%s\n", source_);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// ファイルからプログラムを受け取った時のエラーメッセージを表示する
void error_at(char *loc, char *fmt, ...) {
    // ファイル名がなければ引数からプログラムを受け取っている
    if (filename_ == NULL)
        error_at_nofile(loc, fmt);

    va_list ap;
    va_start(ap, fmt);

    // locが含まれている行の開始地点と終了地点を取得
    char *line = loc;
    while (source_ < line && line[-1] != '\n')
        line--;

    char *end = loc;
    while (*end != '\n')
        end++;

    // 見つかった行が全体の何行目なのか調べる
    int line_num = 1;
    for (char *p = source_; p < line; p++)
        if(*p == '\n')
            line_num++;

    // 見つかった行をファイル名と行番号と一緒に表示
    int indent = fprintf(stderr, "%s:%d: ", filename_, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    // エラー箇所を"^"で指し示して、エラーメッセージを出力
    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void expect(int line, int expected, int actual) {
    if (expected == actual) {
        return;
    }

    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
}

void test_vector_insert() {

    Vector *vec = new_vector();
    expect(__LINE__, 0, vec->len);

    for (int i = 0; i < 100; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        vec_push(vec, (void *)val);
    }

    int *tmpval = malloc(sizeof(int));
    *tmpval = 101;
    vec_insert(vec, tmpval, 50);

    expect(__LINE__, 0  , (int)*((int *)vec->data[0 ]));
    expect(__LINE__, 101, (int)*((int *)vec->data[50]));
    expect(__LINE__, 50,  (int)*((int *)vec->data[51]));
    expect(__LINE__, 99 , (int)*((int *)vec->data[100]));
}

void test_vector() {
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

    // 50番目を削除
    vec_remove(vec, 50);
    expect(__LINE__, 99, vec->len);
    expect(__LINE__, 0  , (int)*((int *)vec->data[0 ]));
    expect(__LINE__, 48 , (int)*((int *)vec->data[48]));
    expect(__LINE__, 49 , (int)*((int *)vec->data[49]));
    expect(__LINE__, 51 , (int)*((int *)vec->data[50]));
    expect(__LINE__, 52 , (int)*((int *)vec->data[51]));
    expect(__LINE__, 99 , (int)*((int *)vec->data[98]));

    test_vector_insert();

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

    expect(__LINE__, 6, (intptr_t)map_get_at_index(map, 0));
    expect(__LINE__, 4, (intptr_t)map_get_at_index(map, 1));

    expect(__LINE__, 2, get_map_size(map));
    map_put(map, "hoge", (void *)6);
    expect(__LINE__, 3, get_map_size(map));

    printf("test_map OK\n");
}

void test_type() {
    Type *type = new_type(INT, NULL);
    expect(__LINE__, INT, type->ty);
    expect(__LINE__, 0, (intptr_t)type->ptrof);

    Type *intptr = new_type(PTR, type);
    expect(__LINE__, PTR, intptr->ty);
    expect(__LINE__, (intptr_t)type, (intptr_t)intptr->ptrof);

    printf("test_type OK\n");
}

