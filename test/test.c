
#include <stdio.h>
#include <stdlib.h>
#define MACRO (7)

int GROVAL_INT = 17;

int variable_int() {
    int x;
    int y;

    x = 0;
    y = MACRO;
    x = y + 9;

    return x;   // 16
}

/*
Multi line comment
 */


// エラー表示を行う
int check_result(char *message, int expect, int result) {
    if (expect == result) {
        printf("OK [%s]\n", message);
    } else {
        printf("[%s] failed. expect: [%d] but result: [%d]\n", message, expect, result);
        exit(-1);
    }

    return 0;
}

// add argument test int
// add argument test char

int test_int_arguments(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f;}
int test_char_arguments(char a, char b, char c, char d, char e, char f) { return a + b + c + d + e + f;}

int test_add()           { return 1 + 2 + 3 + 4; }
int test_sub()           { return 5 - 3; }
int test_multi()         { return 5 * 3;}
int test_division()      { return 15 / 3;}
int test_calc()          { return 1 * 2 + (3 + 4); }
int test_equal1()        {return 1 == 1;}
int test_equal2()        {return 1 == 3;}
int test_equal3()        {return 1 != 3;}
int test_equal4()        {return 1 != 1;}
int test_1()             {int a; int b; a = 3; b = 5 * 6 - 8; return b;}
int test_2()             {int a; int b; a = 3; b = 5 * 6 - 8; return b / 2;}
int test_3()             {int a; int b; a = 3; b = 5 * 6 - 8; return a;}
int test_4()             {int a; int b; a = 3; b = 5 * 6 - 8; return a + b / 2;}
int test_5()             {int a; int z; a = 5; z = 12; return a + z;}
int test_6()             {int val; int z; val = 5; z = 12; return val + z;}
int test_7()             {int val; int hoge; int foo; val = 4; hoge = 3; foo = val; return hoge + foo;}
int test_8()             {int a; a = 1 == 1 + 2; return a;}
int test_9()             {int a; a = 4 == 1 + 2 * 2 - 1 == 1; return a;}

int test_10()            {int a; a = 3; if(a){a = 2;} return a;}
int test_11()            {int a; a = 0; if(a){a = 2;} return a;}
int test_12()            {int a; a = 0; if(a){a = 2;}else{a = 7;} return a;}
int test_13()            {int a; a = 0; if(a){a = 2;}else a = 7; return a;}
int test_14()            {int a; a = 0; if(a){a = 2;}else if (a == 2){a = 5;} return a;}
int test_15()            {int a; a = 2; if(a){a = 2;}else if (a == 0){a = 5;} return a;}
int test_16()            {int a; a = 2; if(a == 1){a = 2;}else if (a == 0){a = 5;} else {a = 3;}  return a;}
int test_17()            {int a; a = 2; if(a){a = 2;}else if (a == 0){a = 5;} else {a = 3; return 7;}  return a;}
// int test_18()            {int a; a = 0; if(hoge()){a = 2;} return a;}
//int test_19()            {int a; a = 0; if(hoge()){a = 2;} return a;}
int test_20()            {int a; a = 0; if(a == 0){a = 2;} return a;}
int test_21()            {int a; int b; a = 10; b = 0; while (a != 0) {b = b + a; a = a - 1;} return b;}
int test_22()            {int a; int i; a = 0; for(i = 0; i != 11; i = i + 1){a = a + i;} return a;}
int test_23()            {int a; a = 0; int i; i = 10; for(; i != 11; i = i + 1){a = a + i;} return a;}
int test_24()            {int i; for(i = 0; i != 10; i = i + 1){return i;} return 10;}
int test_25()            {int i; for(i = 0; i != 10; i = i + 1){ if (i == 5) return i;} return 10;}
int test_26()            {int a; int *b; a = 10; return a;}
int test_27()            {int x; int *y; x = 1; y = &x; return x;}
int test_28()            {int x; int *y; x = 10; y = &x; return *y;}
int test_29()            {int x; int c; x = 10; int *y; y = &x; return *y;}
int test_30()            {int x; int c; x = 13; int *y; y = &x; return x;}
int test_31()            {int x; int c; x = 13; int *y; y = &x; return *y;}
int test_32()            {int x; int *y; int c; x = 13; y = &x; c = 0; return *y;}
int test_33()            {int x; int c; x = 13; int *y; y = &x; c = 0; return *y;}
int test_34()            {int x; int c; x = 10; c = 7; return c;}
int test_35()            {int x; int c; x = 10; c = 7; return x;}
int test_36()            {int x; int *c; x = 10; int *y; y = &x; return *y;}
int test_37()            {int x; int c; x = 10; c = 7; int *y; return c;}
int test_38()            {int x; int c; x = 10; c = 7; int *y; y = &x; return c;}
int test_39()            {int x; int c; x = 10; int *y; y = &x; *y = 7; return *y;}
int test_40()            {int x; int *y; x = 10; y = &x; return *y;}
int test_41()            {int x; x = 3; int *y; y = &x; x = 13; return *y;}
int test_42()            {int *b; int a; b = &a; *b = 10; return *b;}
// int test_43()            {int a; a = func_TEST01(); return a;}
int test_44()            {int *p; alloc4(&p, 1, 2, 4, 8); return 1;}
int test_45()            {int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 2; return *q;}
int test_46()            {int *p; alloc4(&p, 1, 2, 4, 8); int *q; q = p + 2; *q; q = p + 3; return *q;}
int test_47()            {int *p; alloc4(&p, 1, 2, 4, 8); p = p + 2; return *p;}
int test_48()            {int *p; alloc4(&p, 1, 2, 4, 8); return *(p + 2);}
int test_49()            {int x; x = -6; return (6 + x);}
int test_50()            {int x; x = -5; return (5 + x);}
int test_51()            {int x; x = 3; int y; y = -(x + 5); return (8 + y);}
int test_52()            {int x; x = +3; int y; y = -(x + 5); return (8 + y);}
int test_53()            {int x; x = sizeof(1); return x;}
int test_54()            {return sizeof(sizeof(1));}
int test_55()            {int r; int x; int *y; r = sizeof(x); return r;}
int test_56()            {int r; int x; int *y; r = sizeof(y); return r;}
int test_57()            {int r; int x; int *y; r = sizeof(x + 3); return r;}
int test_58()            {int r; int x; int *y; r = sizeof(y + 3); return r;}
int test_59()            {int r; int x; int *y; r = sizeof(*y); return r;}
int test_60()            {int r; int x; int *y; r = sizeof(*y) + 1; return r;}
int test_61()            {int a[10]; int b; b = 7; return b;}
int test_62()            {int x; int a[10]; int b; x = 7; b = 13; return x;}
int test_63()            {int x; int a[10]; int b; x = 7; b = 13; return b;}
int test_64()            {int a[10]; return sizeof(a);}
int test_65()            {int a[2]; *a = 1; int *p; p = a; return *p;}
int test_66()            {int a[2]; *a = 1; *(a + 1) = 2; int *p; p = a; return *p + *(p + 1);}
int test_67()            {int a[2]; *a = 1; a[1] = 2; return a[1];}
int test_68()            {int a[2]; *a = 1; a[1] = 2; return *a + a[1];}
int test_69()            {int a[5]; a[3] = 7; return 3[a];}
int test_70()            {int a[2]; *a = 1; a[1] = 7; a[0] = 3; return a[0] + a[1];}
int test_71()            {return 2 > 1;}
int test_72()            {return 1 > 2;}
int test_73()            {return 1 < 2;}
int test_74()            {return 2 < 1;}
int test_75()            {return 2 <= 1;}
int test_76()            {return 2 <= 2;}
int test_77()            {return 2 <= 3;}
int test_78()            {return 3 <= 2;}
int test_79()            {return 1 >= 0;}
int test_80()            {return 1 >= 1;}
int test_81()            {return 0 >= 1;}
int test_82()            { int a; {a = 7; int b; b = 2;} return a;}
int test_83()            { int a; {a = 5; int b; b = 1; {a = a + b;}} {a = a + 2;} return a;}
int test_84()            { int a; {a = 5; int b; b = 1; if(0){a = a + b;}} {a = a + 2;} return a;}
int test_85()            {char a; a = 7; return a;}
int test_86()            {char a; a = 2; a = 1; a = 7; return a;}
int test_87()            {char a; char b; a=1; b=2; return b + 3;}
int test_88()            {char a; char b; char c; a=1; b=2; return a;}
int test_89()            {char a; char *b; a = 10; return a;}
int test_90()            {char x; char *y; x = 1; y = &x; return x;}
int test_91()            {char x; char *y; x = 10; y = &x; return *y;}
int test_92()            {char x; char c; x = 10; char *y; y = &x; return *y;}
int test_93()            {char x; char c; x = 13; char *y; y = &x; return x;}
int test_94()            {char x; char c; x = 13; char *y; y = &x; return *y;}
int test_95()            {char x; char *y; char c; x = 13; y = &x; c = 0; return *y;}
int test_96()            {char x; char c; x = 13; char *y; y = &x; c = 0; return *y;}
int test_97()            {char x; char c; x = 10; c = 7; return c;}
int test_98()            {char x; char c; x = 10; c = 7; return x;}
int test_99()            {char x; char *c; x = 10; char *y; y = &x; return *y;}
int test_100()           {char x; char c; x = 10; c = 7; char *y; return c;}
int test_101()           {char x; char c; x = 10; c = 7; char *y; y = &x; return c;}
int test_102()           {char x; char c; x = 10; char *y; y = &x; *y = 7; return *y;}
int test_103()           {char x; char *y; x = 11; y = &x; return *y;}
int test_104()           {char x; x = 3; char *y; y = &x; x = 13; return *y;}
int test_105()           {char *b; char a; b = &a; *b = 10; return *b;}
int test_106()           {int r; char x; int *y; r = sizeof(x); return r;}
int test_107()           {char a[10]; return sizeof(a);}
int test_108()           {char r; char x; char *y; r = sizeof(y); return r;}
int test_109()           {char r; char x; char *y; r = sizeof(x + 3); return r;}
int test_110()           {char r; char x; char *y; r = sizeof(y + 3); return r;}
int test_111()           {char x[3]; char y; char result; x[0] = 7; x[1] = 2; y = 4; result = x[0] + y; return result; }

int main() {

    check_result("test_int_arguments",  21, test_int_arguments(1,2,3,4,5,6));
    check_result("test_char_arguments", 21, test_char_arguments(1,2,3,4,5,6));
    check_result("test_add",          10, test_add());
    check_result("test_sub",           2, test_sub());
    check_result("test_multi",        15, test_multi());
    check_result("test_division",      5, test_division());
    check_result("test_calc",          9, test_calc());
    check_result("test_equal1",        1, test_equal1());
    check_result("test_equal2",        0, test_equal2());
    check_result("test_equal3",        1, test_equal3());
    check_result("test_equal4",        0, test_equal4());
    check_result("test_1",            22, test_1());
    check_result("test_2",            11, test_2());
    check_result("test_3",             3, test_3());
    check_result("test_4",            14, test_4());
    check_result("test_5",            17, test_5());
    check_result("test_6",            17, test_6());
    check_result("test_7",             7, test_7());
    check_result("test_8",             0, test_8());
    check_result("test_9",             1, test_9());
    check_result("test_10",            2, test_10());
    check_result("test_11",            0, test_11());
    check_result("test_12",            7, test_12());
    check_result("test_13",            7, test_13());
    check_result("test_14",            0, test_14());
    check_result("test_15",            2, test_15());
    check_result("test_16",            3, test_16());
    check_result("test_17",            2, test_17());
    // check_result("test_18",            2, test_18());
    // check_result("test_19",            0, test_19());
    check_result("test_20",            2, test_20());
    check_result("test_21",           55, test_21());
    check_result("test_22",           55, test_22());
    check_result("test_23",           10, test_23());
    check_result("test_24",            0, test_24());
    check_result("test_25",            5, test_25());
    check_result("test_26",           10, test_26());
    check_result("test_27",            1, test_27());
    check_result("test_28",           10,test_28());
    check_result("test_29",           10,test_29());
    check_result("test_30",           13,test_30());
    check_result("test_31",           13,test_31());
    check_result("test_32",           13,test_32());
    check_result("test_33",           13,test_33());
    check_result("test_34",            7,test_34());
    check_result("test_35",           10, test_35());
    check_result("test_36",           10, test_36());
    check_result("test_37",            7, test_37());
    check_result("test_38",            7, test_38());
    check_result("test_39",            7, test_39());
    check_result("test_40",           10, test_40());
    check_result("test_41",           13, test_41());
    check_result("test_42",           10, test_42());
    // check_result("test_43",           7, test_43());
    // check_result("test_44",           1, test_44());
    // check_result("test_44",           1, test_44());
    // check_result("test_45",           4, test_45());
    // check_result("test_46",           8, test_46());
    // check_result("test_47",           4, test_47());
    // check_result("test_48",           4, test_48());
    check_result("test_49",           0, test_49());
    check_result("test_50",           0, test_50());
    check_result("test_51",           0, test_51());
    check_result("test_52",           0, test_52());
    check_result("test_53",           4, test_53());
    check_result("test_54",           4, test_54());
    check_result("test_55",           4, test_55());
    check_result("test_56",           8, test_56());
    check_result("test_57",           4, test_57());
    check_result("test_58",           8, test_58());
    check_result("test_59",           4, test_59());
    check_result("test_60",           5, test_60());
    check_result("test_61",           7, test_61());
    check_result("test_62",           7, test_62());
    check_result("test_63",          13, test_63());
    check_result("test_64",          10, test_64());
    check_result("test_65",           1, test_65());
    check_result("test_66",           3, test_66());
    check_result("test_67",           2, test_67());
    check_result("test_68",           3, test_68());
    check_result("test_69",           7, test_69());
    check_result("test_70",          10, test_70());
    check_result("test_71",           1, test_71());
    check_result("test_72",           0, test_72());
    check_result("test_73",           1, test_73());
    check_result("test_74",           0, test_74());
    check_result("test_75",           0, test_75());
    check_result("test_76",           1, test_76());
    check_result("test_77",           1, test_77());
    check_result("test_78",           0, test_78());
    check_result("test_79",           1, test_79());
    check_result("test_80",           1, test_80());
    check_result("test_81",           0, test_81());
    check_result("test_82",           7, test_82());
    check_result("test_83",           8, test_83());
    check_result("test_84",           7, test_84());
    check_result("test_85",           7, test_85());
    check_result("test_86",           7, test_86());
    check_result("test_87",           5, test_87());
    check_result("test_88",           1, test_88());
    check_result("test_89",          10, test_89());
    check_result("test_90",           1, test_90());
    check_result("test_91",          10, test_91());
    check_result("test_92",          10, test_92());
    check_result("test_93",          13, test_93());
    check_result("test_94",          13, test_94());
    check_result("test_95",          13, test_95());
    check_result("test_96",          13, test_96());
    check_result("test_97",           7, test_97());
    check_result("test_98",          10, test_98());
    check_result("test_99",          10, test_99());
    check_result("test_100",          7, test_100());
    check_result("test_101",          7, test_101());
    check_result("test_102",          7, test_102());
    check_result("test_103",         11, test_103());
    check_result("test_104",         13, test_104());
    check_result("test_105",         10, test_105());
    check_result("test_106",          1, test_106());
    check_result("test_107",         10, test_107());
    check_result("test_108",          8, test_108());
    check_result("test_109",          1, test_109());
    check_result("test_110",          8, test_110());
    check_result("test_111",         11, test_111());

    printf("OK\n");
    return 0;
}

