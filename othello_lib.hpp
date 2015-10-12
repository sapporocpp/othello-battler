#include <iostream>
#include <cstdlib>
#include <cstring>

namespace Othello{
    enum class Piece : char {
        BLACK = 1, // 黒が置かれている
        WHITE = 2, // 白が置かれている
        EMPTY = 0, // 石は置かれていない
        INVALID = 4 // 無効な座標を指定した場合など
    };
    
    constexpr int SIZE = 8; // 盤面の大きさ
    
    class InvalidArgumentException{
    public:
        InvalidArgumentException(){}
    };
    
    // 石の番号（BLACK, WHITEなど）を石の文字（'B', 'W'など）に変換
    // 引数が不正でも特段のチェックはしていない
    char get_piece_name(Piece color){
        return ".BW"[static_cast<size_t>(color)];
    }
    
    // 石の配置
    class Placement{
    private:
        Piece places_[SIZE][SIZE];
    public:
        // 空の盤面を生成
        Placement(){}
        
        // 初期盤面を生成
        Placement(bool initialized){
            if(initialized){
                for(int i = 0; i < SIZE; ++i){
                    for(int j = 0; j < SIZE; ++j){
                        if((i == SIZE/2-1 || i == SIZE/2) && (j == SIZE/2-1 || j == SIZE/2)){
                            places_[i][j] = ((i+j)%2 ? Piece::BLACK : Piece::WHITE);
                        }else{
                            places_[i][j] = Piece::EMPTY;
                        }
                    }
                }
            }
        }
        
        // 位置を指定して石を設置
        // ※「設置」とは、単にその場所にその石がある状態にするだけで
        //   ゲームのルールとして置く（裏返したりもする）わけではない
        inline bool put(int i, int j, Piece piece){
            if(i < 0 || i >= SIZE || j < 0 || j >= SIZE) return false;
            places_[i][j] = piece;
            return true;
        }
        
        // 位置を指定して石を取得
        inline Piece get(int i, int j) const{
            if(i < 0 || i >= SIZE || j < 0 || j >= SIZE) return Piece::INVALID;
            return places_[i][j];
        }
        
        // 表示する
        // current_r, current_cは「石がどこに置かれたか」（省略可能）
        void display(int current_r, int current_c) const{
            for(int i = 0; i < SIZE; ++i){
                for(int j = 0; j <= SIZE; ++j){
                    if(i == current_r && j == current_c){
                        std::cout << '[';
                    }else if(i == current_r && j == current_c+1){
                        std::cout << ']';
                    }else{
                        std::cout << ' ';
                    }
                    if(j == SIZE) break;
                    std::cout << get_piece_name(places_[i][j]);
                }
                std::cout << std::endl;
            }
        }
        
        void display(void) const{
            display(-1, -1);
        }
    };
    
    // 盤面（次がどちらの番か、などの情報含む）
    class Board{
    public:
        static void error_and_throw(const char * progname, size_t error_line){
            std::cerr << "Argument Error at line " << error_line << "." << std::endl;
            std::cerr << "Usage: " << progname << " NonceString Turn BoardString" << std::endl;
            std::cerr << "Turn must be 'B' OR 'W'." << std::endl;
            std::cerr << "BoardString must be /^[.BW]{" << (SIZE * SIZE) << "}$/." << std::endl;
            throw InvalidArgumentException();
        }
        
    private:
        Placement pl_;
        std::string nonce_;
        Piece turn_;
        
    public:
        // コマンドライン引数から生成
        Board(int argc, char ** argv){
            if(argc != 4) error_and_throw(argv[0], __LINE__);
            
            // ランダムな文字列（本来答えを返すべきクライアントが答えを返すように）
            nonce_.assign(argv[1]);
            
            // どちらの手番か
            switch(argv[2][0]){
            case 'B':
                turn_ = Piece::BLACK;
                break;
            case 'W':
                turn_ = Piece::WHITE;
                break;
            default:
                error_and_throw(argv[0], __LINE__);
                break;
            }
            
            // 現在の石の配置
            if(std::strlen(argv[3]) != SIZE * SIZE) error_and_throw(argv[0], __LINE__);
            
            for(int i = 0; i < SIZE * SIZE; ++i){
                switch(argv[3][i]){
                case 'B':
                    pl_.put(i/SIZE, i%SIZE, Piece::BLACK);
                    break;
                case 'W':
                    pl_.put(i/SIZE, i%SIZE, Piece::WHITE);
                    break;
                case '.':
                    pl_.put(i/SIZE, i%SIZE, Piece::EMPTY);
                    break;
                default:
                    error_and_throw(argv[0], __LINE__);
                    break;
                }
            }
        }
        
        // その場所に置かれている石
        inline Piece place(int i, int j) const{
            return pl_.get(i, j);
        }
        
        // 石を置くことに対応する出力を行う
        // どこにも置けない場合、引数なしで呼び出す
        void put(int i, int j) const{
            std::cout << nonce_ << " " << i << " " << j << std::endl;
        }
        
        void put(void) const{
            std::cout << nonce_ << std::endl;
        }
        
        // 自分の石の色
        inline Piece my_color(void) const{
            return turn_;
        }
        
        // 対戦相手の石の色
        inline Piece opponent_color(void) const{
            return(turn_ == Piece::BLACK ? Piece::WHITE : Piece::BLACK);
        }
    };
}
