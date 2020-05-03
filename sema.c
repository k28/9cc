#include "9cc.h"

int    current_pointer_offset_ = 1;

int offset_of_variable(Variable *val_info) {
    if (val_info->type->ty == INT) {
        // INTの変数
        return 1;
    }
    if (val_info->type->ty == CHAR) {
        // CHARの変数
        return 1;
    }
    if (val_info->type->ty == PTR && val_info->type->ptrof->ty == INT) {
        // INTへのポインター
        return SIZE_OF_INT;
    }
    if (val_info->type->ty == PTR && val_info->type->ptrof->ty == PTR) {
        // ポインターへのポインター
        return SIZE_OF_ADDRESS;
    }
    if (val_info->type->ty == ARRAY && val_info->type->ptrof->ty == INT) {
        // INT 配列へのポインター
        return SIZE_OF_INT;
    }

    // ここに来たら対応不足
    error("不明な変数型です.", "");
    return 1;
}

void walk(Node *node) {
    if (node == NULL) return;

    switch (node->ty) {
        case ND_NUM:
            node->val = node->val * current_pointer_offset_;
            return;

        case '+':
        case '-':
            // 左のNodeの値が変数の場合、+,-の計算の際にオフセットを設定する
            // その後数値が出てきた時には、値にオフセットを掛ける
            if (node->lhs->ty == ND_IDENT) {
                int before_offset = current_pointer_offset_;
                Variable *val_info = map_get(variables, node->lhs->name);
                current_pointer_offset_ = offset_of_variable(val_info);
                walk(node->rhs);
                current_pointer_offset_ = before_offset;
                return;
            } else if (node->rhs->ty == ND_IDENT) {
                // 右のNodeの値が変数の場合は上記のパターンの逆
                // 3[a] -> *(3 + a) みたいなパターンに対応するため
                int before_offset = current_pointer_offset_;
                Variable *val_info = map_get(variables, node->rhs->name);
                current_pointer_offset_ = offset_of_variable(val_info);
                walk(node->lhs);
                current_pointer_offset_ = before_offset;
                return;
            }
    }

    walk(node->lhs);
    walk(node->rhs);
}

void sema() {
    for (int i = 0; i < functions->len; i++) {
        Function *function = (Function *)functions->data[i];
        // ローカル変数のMapをグローバル領域にコピー (値の定義はcodegen.cにある)
        variables = function->variables;

        for (int i = 0; i < function->code->len; i++) {
            // オフセットを初期化
            current_pointer_offset_ = 1;
            Node *node = function->code->data[i];
            walk(node);
        }
    }
}


