
#define MACRO (7)
#define PRINT_MACRO(x, y) printf("x: %s, y:%s", x, y);

int variable_int() {
    int x;
    int y;

    x = 0;
    y = MACRO;
    x = y + 9;

    return x;   // 16
}


// エラー表示を行う
int check_result(char *message, int expect, int result) {
    if (expect == result) {
        printf("[%s] OK\n", message);
    } else {
        printf("[%s] failed. expect: [%d] but result: [%d]\n", message, expect, result);
        exit(-1);
    }
}

int main() {

    int expect;
    int result;
   
    check_result("variable_int", 16, variable_int());

    PRINT_MACRO(1, 2)


    printf("OK\n");
    return 0;
}

