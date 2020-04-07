# 9cc
9cc trace

# Reference URL
* [低レイヤを知りたい人のためのCコンパイラ作成入門](https://www.sigbus.info/compilerbook)

# Development Environment

* Ubuntu 18.04 LTS

# Source list

| ソース      | 説明                                                   |
|-------------|--------------------------------------------------------|
| 9cc.h       | ヘッダーファイル                                       |
| main.c      | メイン関数, -test引数を与えるとテストコードを実行する  |
| tokenize.c  | 入力文字列をトークナイズする                           |
| parse.c     | 構文解析                                               |
| sema.c      | 値の調整を行う(コード生成の前処理)                     |
| codegen.c   | コードの生成                                           |
| container.c | ベクタ, マップ, そのテストコード                       |

