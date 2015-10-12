#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "othello_lib.hpp"

#if defined(_WIN16) || defined(_WIN32) || defined(_WIN64)
#define POPEN _popen
#else
#define POPEN popen
#endif

const size_t BUFSIZE = 64;

// 実行結果を読み込む。
// 返ってくる文字列は、以下のいずれかでなければならない。
// ・「nonce 置く場所(行) 置く場所(列)」（置ける場合）
// ・「nonce」（どこにも置けない場合）
bool parse_sent_string(const char * buf, std::string & nonce, int & r, int & c){
    // まず、スペースで区切るため、スペースの位置を列挙する
    std::vector<char *> spaces;
    char *end_r, *end_c;
    
    char * pos = std::strchr(buf, ' ');
    while(pos != NULL){
        spaces.push_back(pos);
        pos = std::strchr(pos + 1, ' ');
    }
    
    switch(spaces.size()){
    case 0:
        // buf中にスペースがない＝もう置ける場所がない（buf全体がnonce）
        nonce.assign(buf);
        r = -1;
        c = -1;
        return true;
    case 2:
        // 石が置かれる
        nonce.assign(buf, spaces[0] - buf);
        r = static_cast<int>(std::strtol(spaces[0] + 1, &end_r, 10));
        c = static_cast<int>(std::strtol(spaces[1] + 1, &end_c, 10));
        
        // 数値を指定すべき部分で数字以外が見つかった場合、不正なフォーマット
        if(end_r != spaces[1] || end_c != buf + std::strlen(buf)){
            return false;
        }
        
        return true;
    default:
        return false;
    }
}

// (r, c)の場所にplayerが石を置き、裏返す。
// 一つでも裏返したらtrue, そうでなければfalse
bool flip_stones(Othello::Placement & placement, int r, int c, Othello::Piece player_color, Othello::Piece opponent_color){
    // 8方向に石を伸ばしていく
    int t;
    bool flipped = false;
    
    for(int y = -1; y <= 1; ++y){
        for(int x = -1; x <= 1; ++x){
            if(x == 0 && y == 0) continue;
            
            // まず、隣が相手の石でなければ、石は裏返せない
            if(placement.get(r + y, c + x) != opponent_color){
                continue;
            }
            
            // そして、そこから石を伸ばしていって、
            // 対戦相手の石が続いて、そのあと自分の石が現れればよい
            t = 2;
            while(placement.get(r + y*t, c + x*t) == opponent_color){
                ++t;
            }
            
            if(placement.get(r + y*t, c + x*t) == player_color){
                // TODO: ここを保留する
                // まず石を置く
                placement.put(r, c, player_color);
                
                // 石を裏返す
                for(; t >= 1; --t){
                    placement.put(r + y*t, c + x*t, player_color);
                }
                flipped = true;
            }
        }
    }
    
    return flipped;
}

// メイン
int main(int argc, char ** argv){
    if(argc != 3){
        std::cerr << "Usage: " << argv[0] << " PROGRAM1 PROGRAM2" << std::endl;
        return 1;
    }
    
    // 乱数初期化（nonceの生成に利用）
    // 現在時刻の取得部分は http://kudannlab.blogspot.jp/2013/12/c11.html を参考にした
    std::chrono::high_resolution_clock::time_point clock = std::chrono::high_resolution_clock::now();
    long long int count = std::chrono::duration_cast<std::chrono::nanoseconds>(clock.time_since_epoch()).count();
    std::mt19937_64 rand_engine(count);
    std::uniform_int_distribution<char> rand_distribution('A', 'Z');
    
    // 盤面の状態
    Othello::Placement placement(true);
    
    int player; // 1 or 2 (argv[1] / argv[2] と指定するため)
    int opponent = 1;
    Othello::Piece player_color, opponent_color;
    
    std::string command, nonce, received_nonce;
    char buf[BUFSIZE];
    int received_c = -1, received_r = -1;
    bool last_choice_is_pass = false; // 直前がパスだったことを表す。2連続パスなら終了
    
    for(;;){
        player = opponent;
        opponent = 3 - player;
        
        player_color = (player == 1) ? Othello::Piece::BLACK : Othello::Piece::WHITE;
        opponent_color = (opponent == 1) ? Othello::Piece::BLACK : Othello::Piece::WHITE;
        
        // 表示
        std::cout << "----------" << std::endl;
        placement.display(received_r, received_c);
        std::cout << "[Press Enter Key]" << std::endl;
        fgets(buf, BUFSIZE, stdin);
        
        // コマンドラインを生成
        nonce.assign(1, rand_distribution(rand_engine));
        nonce.append(1, rand_distribution(rand_engine));
        
        command.assign(argv[player]);
        command.append(" ");
        command.append(nonce); // nonce
        command.append(" ");
        command.append(1, (player == 1 ? 'B' : 'W')); // どちらの手番か
        command.append(" ");
        for(int i = 0; i < Othello::SIZE; ++i){
            for(int j = 0; j < Othello::SIZE; ++j){
                command.append(1, Othello::get_piece_name(placement.get(i, j)));
            }
        }
        
        // コマンドを実行し、結果を受け取る
        FILE * fp = POPEN(command.c_str(), "r");
        if(fp == NULL){
            std::cerr << "[ERROR] Failed in running: \"" << command << "\"" << std::endl;
            return 1;
        }
        
        fgets(buf, BUFSIZE, fp);
        size_t buflen = std::strlen(buf);
        
        // 末尾の改行を除去
        if(buflen == 0 || buf[buflen - 1] != '\n'){
            std::cerr << "[ERROR] Player " << player << ": Too long result received (received \"" << buf << "\") [" << static_cast<int>(buf[buflen - 1]) << "]" << std::endl;
            return 1;
        }
        buf[buflen - 1] = '\0';
        
        // 受け取った文字列を解析
        if(!parse_sent_string(buf, received_nonce, received_r, received_c)){
            std::cerr << "[ERROR] Player " << player << ": Invalid format received: \"" << buf << "\" (Expected: \"NONCE ROWNUM COLNUM\")" << std::endl;
            return 1;
        }
        
        if(nonce.compare(received_nonce) != 0){
            std::cerr << "[ERROR] Player " << player << ": Invalid nonce received: \"" << received_nonce << "\" (Expected: \"" << nonce << "\")" << std::endl;
            return 1;
        }
        
        // 有効な手が指されているか確認
        if(received_r == -1 || received_c == -1){
            // パスした場合
            std::cout << "Player" << player << " passed his/her turn." << std::endl;
            if(last_choice_is_pass) break; // 二人ともパスした場合は終了
            last_choice_is_pass = true;
            continue;
        }
        
        std::cout << "Player" << player << " put at (" << received_r << ", " << received_c << ")." << std::endl;
        
        if(received_c < 0 || received_c >= Othello::SIZE || received_r < 0 || received_r >= Othello::SIZE){
            // 座標が不正な場合（盤面の範囲外）
            std::cerr << "[ERROR] Player " << player << ": Invalid coordinates received: " << received_r << "," << received_c << std::endl;
            return 1;
        }
        
        if(placement.get(received_r, received_c) != Othello::Piece::EMPTY){
            // 石が置かれていない場所以外に置こうとした場合
            std::cerr << "[ERROR] Player " << player << ": Piece already exists" << std::endl;
            return 1;
        }
        
        last_choice_is_pass = false;
        
        // その場所に石を置いて裏返す
        if(!flip_stones(placement, received_r, received_c, player_color, opponent_color)){
            // 一つも石が裏返らなかった場合は反則
            std::cerr << "Player " << player << ": No stone flipped" << std::endl;
            return 1;
        }
    }
    
    // 終了。結果を調べる
    int b_count = 0, w_count = 0;
    
    for(int i = 0; i < Othello::SIZE; ++i){
        for(int j = 0; j < Othello::SIZE; ++j){
            switch(placement.get(i, j)){
            case Othello::Piece::BLACK:
                ++b_count;
                break;
            case Othello::Piece::WHITE:
                ++w_count;
                break;
            default:
                // 何もしない（空きマスが残っていても構わない）
                break;
            }
        }
    }
    std::cout << "Black: " << b_count << std::endl;
    std::cout << "White: " << w_count << std::endl;
    
    return 0;
}
