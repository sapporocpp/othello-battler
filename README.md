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

なお、C++11で新規に規格化された機能も利用しているため、対応したコンパイラの用意をお願いします。

# 動かしてみる

`make`でビルドができたら、

`./othello_runner ./othello_sample ./othello_sample`  
（Windows環境では`./`は不要）

として実行します。これは、対戦状況を管理するプログラムである`othello_runner`が、黒プレイヤーを`othello_sample`、白プレイヤーを`othello_sample`として対戦させることを意味します。

# 指す手を決める方法

## 概説

プログラムの例については `othello_sample.cpp` を見てください。これは、盤面の各場所について、石をルール上置いてよい場所であるか確かめ、それができるならその場所に置く（置いてよいと最初にわかった場所に置く）というものです。

皆様に作成していただくプログラムの挙動は、プログラムを1回実行するごとに「現在の盤面が与えられ、次に指す1手を選んで出力する」ものとなります。

より具体的には、

- `othello_runner`がこのプログラムに対し、コマンドライン引数を通じて盤面の情報を与えます（どんな引数が来るのかは気にしなくてよいです。`othello_lib.hpp`側で処理します）。
- 皆様はそれを受け、どの手を次に指すかを決定し標準出力に出力します（出力方法についてはAPIがあります）。これが`othello_runner`に引き渡されます。
- 上記手順を繰り返し、対戦を進めます。

## 改良の方針

最初、何から手を付けてよいか分からないという場合は、例えば`othello_sample.cpp`を以下のように改良してみるとよいでしょう：`othello_sample.cpp`では「ルール上置ける場所が1か所見つかればその場所に置く」としていたところを、「置ける場所をすべて調べ、その中で一番多く石を裏返せる場所に置く」とする。

もちろんこれだけでは、相手から反撃を受けてしまうこともあるでしょう。その先は皆様のアイデア次第です。

# API

盤面情報の取得や結果の出力は、`othello_lib.hpp`内の`namespace Othello{ ... }`内に定義されています。具体的な利用例は`othello_sample.cpp`を参照してください。

## enum class Othello::Piece;

石を表す列挙型です。

- `Piece BLACK;` // 黒石を表す定数
- `Piece WHITE;` // 白石を表す定数
- `Piece EMPTY;` // 石は置かれていない
- `Piece INVALID;` // 無効な座標を指定した場合などの値

## class Othello::Placement;

石の配置を表すクラスです。

- `bool Placement::put(int i, int j, Piece piece);`  
  // 座標(i, j)に石pieceがある状態にする（裏返す操作まではしない）。
- `Piece Placement::get(int i, int j) const;`  
  // 座標(i, j)の石を取得する。
- `void Placement::display(void) const;`  
  // 盤面を表示する。
- `void Placement::display(int i, int j) const;`  
  // 上記と似るが、座標(i, j)を強調表示する。  
  //`othello_runner.cpp`では最後に指された手を表示するのに利用している。

## class Othello::Board;

盤面情報（石の配置＋次はどちらのプレイヤーの手番か＋nonce）を表すクラスです。

コマンドライン引数をこのクラスに変換したのち、指す手を決める処理を行います。

- `Placement Board::placement(void);`  
  // 石の配置を取得する。  
  // 上記の`Placement`クラスとして得られる。
- `void Board::put(int i, int j);`  
- `void Board::put(void);`  
  // 座標(i, j)に石を置くことを表す出力を行う。  
  // **1回のプログラム実行につき1回だけ呼ぶこと。**  
  // どこにも石が置けない場合、引数なしの put() を呼ぶ。
- `Piece Board::my_color(void);`
- `Piece Board::opponent_color(void);`  
  // それぞれ、自分の色ならびに相手の色。  

# 補足

- 結果は標準出力を使って受け渡されるので、何かデバッグ出力をしたい場合は標準エラーを使ってください。
