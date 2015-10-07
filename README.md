# 2015.10.17 Sapporo.cpp 第9回勉強会 お題「オセロ」

作：H.Hiro <main@hhiro.net>

# ファイル構成

- `Makefile`
  - Unix系OS向けのMakefileです。デフォルトでは `othello_runner` と `othello_sample` を生成します。
- `othello_lib.hpp`
  - 共通の挙動を定義するヘッダです。
- `othello_runner.cpp`
  - 対戦状況を管理するプログラムです。
- `othello_sample.cpp`
  - 指す手を決めるプログラムです。これに相当するプログラムを皆様に作成していただくことになります。

# 動かしてみる

`make`でビルドができたら、

`./othello_runner ./othello_sample ./othello_sample`  
（Windows環境では`./`は不要）

として実行します。これは、対戦状況を管理するプログラムである`othello_runner`が、黒プレイヤーを`othello_sample`、白プレイヤーを`othello_sample`として対戦させることを意味します。

# 指す手を決める方法

例については `othello_sample.cpp` を見てください。これは、盤面の各場所について、石をルール上置いてよい場所であるか確かめ、それができるならその場所に置く（置いてよいと最初にわかった場所に置く）というものです。

皆様に作成していただくプログラムの挙動は、プログラムを1回実行するごとに「現在の盤面が与えられ、次に指す1手を選んで出力する」ものとなります。

より具体的には、
- `othello_runner`がこのプログラムに対し、コマンドライン引数を通じて盤面の情報を与えます（どんな引数が来るのかは気にしなくてよいです。`othello_lib.hpp`側で処理します）。
- 皆様はそれを受け、どの手を次に指すかを決定し標準出力に出力します（出力方法についてはAPIがあります）。これが`othello_runner`に引き渡されます。
- 上記手順を繰り返し、対戦を進めます。

## API

盤面情報の取得や結果の出力は、`othello_lib.hpp`内の`Othello::Board`クラスに定義されています。APIは以下のものですべてです（具体的な利用例は`othello_sample.cpp`を参照してください）。

- `const char Othello::BLACK;` // 黒石を表す定数
- `const char Othello::WHITE;` // 白石を表す定数
- `const char Othello::EMPTY;` // 石は置かれていない
- `const char Othello::INVALID;` // 無効な座標を指定した場合などの値
- `char Othello::Board::place(int i, int j);`  
  // 座標(i, j)に置かれている石。`Othello::BLACK` などで返る。
- `void Othello::Board::put(int i, int j);`  
- `void Othello::Board::put(void);`  
  // 座標(i, j)に石を置くことを表す出力を行う。  
  // **1回のプログラム実行につき1回だけ呼ぶこと。**  
  // どこにも石が置けない場合、引数なしの put() を呼ぶ。
- `char Othello::Board::my_color(void);`
- `char Othello::Board::opponent_color(void);`  
  // それぞれ、自分の色ならびに相手の色。  
  // `Othello::BLACK` などで返る。

## 補足

- 結果は標準出力を使って受け渡されるので、何かデバッグ出力をしたい場合は標準エラーを使ってください。
