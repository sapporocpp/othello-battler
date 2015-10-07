#include <iostream>
#include <string>
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
const char * NONCE_SOURCE = "qawsedrftgyhujikolp";

// 0�ȏ�n�����̐�����Ԃ�
int rand_int(int n){
	int result;
	do{
		result = std::rand() / (RAND_MAX / n);
	}while(result >= n);
	return result;
}

int main(int argc, char ** argv){
	if(argc != 3){
		std::cerr << "Usage: " << argv[0] << " PROGRAM1 PROGRAM2" << std::endl;
		return 1;
	}
	
	std::srand(std::time(0));
	
	Othello::Placement placement(true);
	
	int player; // 1 or 2 (argv[1] / argv[2] �Ǝw�肷�邽��)
	int opponent = 1;
	char player_color, opponent_color;
	
	std::string command, nonce;
	char buf[BUFSIZE];
	char received_nonce[BUFSIZE];
	int received_c = -1, received_r = -1;
	bool last_choice_is_pass = false; // ���O���p�X���������Ƃ�\���B2�A���p�X�Ȃ�I��
	
	for(;;){
		player = opponent;
		opponent = 3 - player;
		
		player_color = (player == 1) ? Othello::BLACK : Othello::WHITE;
		opponent_color = (opponent == 1) ? Othello::BLACK : Othello::WHITE;
		
		// �\��
		std::cout << "----------" << std::endl;
		placement.display(received_r, received_c);
		std::cout << "[Press Enter Key]" << std::endl;
		fgets(buf, BUFSIZE, stdin);
		
		// �R�}���h���C���𐶐�
		nonce.assign(1, 'a' + rand_int(26));
		nonce.append(1, 'a' + rand_int(26));
		
		command.assign(argv[player]);
		command.append(" ");
		command.append(nonce); // nonce
		command.append(" ");
		command.append(1, (player == 1 ? 'B' : 'W')); // �ǂ���̎�Ԃ�
		command.append(" ");
		for(int i = 0; i < Othello::SIZE; ++i){
			for(int j = 0; j < Othello::SIZE; ++j){
				command.append(1, Othello::NAME[placement.get(i, j)]);
			}
		}
		
		// �R�}���h�����s
		FILE * fp = POPEN(command.c_str(), "rb");
		if(fp == NULL){
			std::cerr << "Failed in running: \"" << command << "\"" << std::endl;
			return 1;
		}
		
		// ���s���ʂ�ǂݍ���
		fgets(buf, BUFSIZE, fp);
		
		received_nonce[0] = '\0';
		received_r = -1;
		received_c = -1;
		sscanf(buf, "%s %d %d", received_nonce, &received_r, &received_c);
		
		if(nonce.compare(received_nonce) != 0){
			std::cerr << "Player " << player << ": Invalid nonce received: \"" << received_nonce << "\" (Expected: \"" << nonce << "\")" << std::endl;
			return 1;
		}
		
		if(received_c == -1 && received_r == -1){
			// �����u����ꏊ���Ȃ�
			std::cout << "Player" << player << " passed his/her turn." << std::endl;
			if(last_choice_is_pass) break;
			last_choice_is_pass = true;
			continue;
		}else{
			std::cout << "Player" << player << " put at (" << received_r << ", " << received_c << ")." << std::endl;
			last_choice_is_pass = false;
		}
		
		if(received_c < 0 || received_c >= Othello::SIZE || received_r < 0 || received_r >= Othello::SIZE){
			std::cerr << "Player " << player << ": Invalid coordinates received: " << received_r << "," << received_c << std::endl;
			return 1;
		}
		
		if(placement.get(received_r, received_c) != Othello::EMPTY){
			std::cerr << "Player " << player << ": Piece already exists" << std::endl;
			return 1;
		}
		
		// ���̏ꏊ�ɐ΂�u���ė��Ԃ�
		// 8�����ɐ΂�L�΂��Ă���
		int t;
		bool flipped = false;
		
		for(int y = -1; y <= 1; ++y){
			for(int x = -1; x <= 1; ++x){
				if(x == 0 && y == 0) continue;
				
				// �܂��A�ׂ�����̐΂łȂ���΁A�΂͗��Ԃ��Ȃ�
				if(placement.get(received_r + y, received_c + x) != opponent_color){
					continue;
				}
				
				// �����āA��������΂�L�΂��Ă����āA
				// �ΐ푊��̐΂������āA���̂��Ǝ����̐΂������΂悢
				t = 2;
				while(placement.get(received_r + y*t, received_c + x*t) == opponent_color){
					++t;
				}
				
				if(placement.get(received_r + y*t, received_c + x*t) == player_color){
					// TODO: ������ۗ�����
					// �܂��΂�u��
					placement.put(received_r, received_c, player_color);
					
					// �΂𗠕Ԃ�
					for(; t >= 1; --t){
						placement.put(received_r + y*t, received_c + x*t, player_color);
					}
					flipped = true;
				}
			}
		}
		
		if(!flipped){
			std::cerr << "Player " << player << ": No stone flipped" << std::endl;
			return 1;
		}
	}
	
	// �I���B���ʂ𒲂ׂ�
	int b_count = 0, w_count = 0;
	
	for(int i = 0; i < Othello::SIZE; ++i){
		for(int j = 0; j < Othello::SIZE; ++j){
			switch(placement.get(i, j)){
			case Othello::BLACK:
				++b_count;
				break;
			case Othello::WHITE:
				++w_count;
				break;
			}
		}
	}
	std::cout << "Black: " << b_count << std::endl;
	std::cout << "White: " << w_count << std::endl;
	
	return 0;
}
