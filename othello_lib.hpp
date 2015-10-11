#include <iostream>
#include <cstdlib>
#include <cstring>

namespace Othello{
	const char BLACK = 1; // �����u����Ă���
	const char WHITE = 2; // �����u����Ă���
	const char EMPTY = 0; // �΂͒u����Ă��Ȃ�
	const char INVALID = 4; // �����ȍ��W���w�肵���ꍇ�Ȃ�
	const int SIZE = 8; // �Ֆʂ̑傫��
	
	class InvalidArgumentException{
	public:
		InvalidArgumentException(){}
	};
	
	// �΂̔ԍ��iBLACK, WHITE�Ȃǂ̒萔�j����A�΂̖��O��\�������i'B', 'W'�Ȃǁj�𓾂�B
	// �G���[�`�F�b�N�͍s���Ă��Ȃ��B
	char get_piece_name(char color){
		return ".BW"[static_cast<size_t>(color)];
	}
	
	// �΂̔z�u
	class Placement{
	private:
		char places_[SIZE][SIZE];
	public:
		// �����Ȃ��Ֆʂ𐶐�
		Placement(){}
		
		// �����Ֆʂ𐶐�
		Placement(bool initialized){
			if(initialized){
				for(int i = 0; i < SIZE; ++i){
					for(int j = 0; j < SIZE; ++j){
						if((i == SIZE/2-1 || i == SIZE/2) && (j == SIZE/2-1 || j == SIZE/2)){
							places_[i][j] = ((i+j)%2 ? BLACK : WHITE);
						}else{
							places_[i][j] = EMPTY;
						}
					}
				}
			}
		}
		
		// �ʒu���w�肵�Đ΂�ݒu
		// ���u�ݒu�v�Ƃ́A�P�ɂ��̏ꏊ�ɂ��̐΂������Ԃɂ��邾����
		//   �Q�[���̃��[���Ƃ��Ēu���i���Ԃ����������j�킯�ł͂Ȃ�
		inline bool put(int i, int j, char piece){
			if(i < 0 || i >= SIZE || j < 0 || j >= SIZE) return false;
			places_[i][j] = piece;
			return true;
		}
		
		// �ʒu���w�肵�Đ΂��擾
		inline char get(int i, int j) const{
			if(i < 0 || i >= SIZE || j < 0 || j >= SIZE) return INVALID;
			return places_[i][j];
		}
		
		// �\������
		// current_r, current_c�́u�΂��ǂ��ɒu���ꂽ���v�i�ȗ��\�j
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
	
	// �Ֆʁi�����ǂ���̔Ԃ��A�Ȃǂ̏��܂ށj
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
		char turn_;
		
	public:
		// �R�}���h���C���������琶��
		Board(int argc, char ** argv){
			if(argc != 4) error_and_throw(argv[0], __LINE__);
			
			// �����_���ȕ�����i�{��������Ԃ��ׂ��N���C�A���g��������Ԃ��悤�Ɂj
			nonce_.assign(argv[1]);
			
			// �ǂ���̎�Ԃ�
			switch(argv[2][0]){
			case 'B':
				turn_ = BLACK;
				break;
			case 'W':
				turn_ = WHITE;
				break;
			default:
				error_and_throw(argv[0], __LINE__);
				break;
			}
			
			// ���݂̐΂̔z�u
			if(std::strlen(argv[3]) != SIZE * SIZE) error_and_throw(argv[0], __LINE__);
			
			for(int i = 0; i < SIZE * SIZE; ++i){
				switch(argv[3][i]){
				case 'B':
					pl_.put(i/SIZE, i%SIZE, BLACK);
					break;
				case 'W':
					pl_.put(i/SIZE, i%SIZE, WHITE);
					break;
				case '.':
					pl_.put(i/SIZE, i%SIZE, EMPTY);
					break;
				default:
					error_and_throw(argv[0], __LINE__);
					break;
				}
			}
		}
		
		// ���̏ꏊ�ɒu����Ă����
		inline char place(int i, int j) const{
			return pl_.get(i, j);
		}
		
		// �΂�u�����ƂɑΉ�����o�͂��s��
		// �ǂ��ɂ��u���Ȃ��ꍇ�A�����Ȃ��ŌĂяo��
		void put(int i, int j) const{
			std::cout << nonce_ << " " << i << " " << j << std::endl;
		}
		
		void put(void) const{
			std::cout << nonce_ << std::endl;
		}
		
		// �����̐΂̐F
		inline char my_color(void) const{
			return turn_;
		}
		
		// �ΐ푊��̐΂̐F
		inline char opponent_color(void) const{
			return(turn_ == BLACK ? WHITE : BLACK);
		}
	};
}
