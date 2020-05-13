
#define MACRO (1)

int variable_int() {
    int x;
    int y;

    x = 1;
    y = 7;
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
    check_result("variable_int", 15, variable_int());


    printf("OK\n");
    return 0;
}

