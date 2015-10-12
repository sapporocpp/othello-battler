// ------------------------------------------------------------
// 2015.10.17 Sapporo.cpp 第9回勉強会 お題「オセロ」
// 作：H.Hiro <main@hhiro.net>
// ------------------------------------------------------------

#include <iostream>
#include "othello_lib.hpp"

// 指定した場所に石が置ける（＝少なくとも一つ、相手の石を裏返せる）か判定
bool possible(const Othello::Placement & placement, Othello::Piece my_color, Othello::Piece opponent_color, int i, int j){
    // もしその場所にすでに石がある場合は、置けない
    if(placement.get(i, j) != Othello::Piece::EMPTY) return false;
    
    // 8方向に石を伸ばしていく
    int t;
    for(int y = -1; y <= 1; ++y){
        for(int x = -1; x <= 1; ++x){
            if(x == 0 && y == 0) continue;
            
            // まず、隣が相手の石でなければ、石は裏返せない
            if(placement.get(i + y, j + x) != opponent_color){
                continue;
            }
            
            // そして、そこから石を伸ばしていって、
            // 対戦相手の石が続いて、そのあと自分の石が現れればよい
            t = 2;
            while(placement.get(i + y*t, j + x*t) == opponent_color){
                ++t;
            }
            
            if(placement.get(i + y*t, j + x*t) == my_color){
                return true;
            }
        }
    }
    
    return false;
}

int main(int argc, char ** argv){
    // 盤面（石の配置＋次はどちらのプレイヤーの手番か＋nonce）を取得
    Othello::Board board(argc, argv);
    if(!board) return 1; // 盤面が正常に取得できなかった場合
    
    Othello::Placement placement(board.placement());
    // 必要ならば、placementは複製できる。
    // 相手の手のシミュレーションをする場合などに。
    // Othello::Placement placement2(placement);
    
    // 盤面を見て、置ける場所があるか確認する。
    for(int i = 0; i < Othello::SIZE; ++i){
        for(int j = 0; j < Othello::SIZE; ++j){
            // このサンプルでは、最初に「ここになら置いてよい」と
            // わかった場所に置くことにしている。
            if(possible(placement, board.my_color(), board.opponent_color(), i, j)){
                board.put(i, j);
                return 0;
            }
        }
    }
    
    board.put(); // どこにも置けない
    return 0;
}
