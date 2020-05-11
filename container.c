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

// 処理中のファイル名
char *filename;

void error_at(char *loc, char *fmt, ...) {
    // locが含まれている行の開始地点と終了地点を取得
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

void expect(int line, int expected, int actual) {
    if (expected == actual) {
        return;
    }

    fprintf(stderr, "%d: %d expected, but got %d\n", line, expected, actual);
    exit(1);
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

