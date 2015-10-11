#include <iostream>
#include <vector>
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

// 0�ȏ�n�����̐�����Ԃ�
int rand_int(int n){
	int result;
	do{
		result = std::rand() / (RAND_MAX / n);
	}while(result >= n);
	return result;
}

// ���s���ʂ�ǂݍ��ށB
// �Ԃ��Ă��镶����́A�ȉ��̂����ꂩ�łȂ���΂Ȃ�Ȃ��B
// �E�unonce �u���ꏊ(�s) �u���ꏊ(��)�v�i�u����ꍇ�j
// �E�unonce�v�i�ǂ��ɂ��u���Ȃ��ꍇ�j
bool parse_sent_string(const char * buf, std::string & nonce, int & r, int & c){
	// �܂��A�X�y�[�X�ŋ�؂邽�߁A�X�y�[�X�̈ʒu��񋓂���
	std::vector<char *> spaces;
	char *end_r, *end_c;
	
	char * pos = std::strchr(buf, ' ');
	while(pos != NULL){
		spaces.push_back(pos);
		pos = std::strchr(pos + 1, ' ');
	}
	
	switch(spaces.size()){
	case 0:
		// buf���ɃX�y�[�X���Ȃ��������u����ꏊ���Ȃ��ibuf�S�̂�nonce�j
		nonce.assign(buf);
		r = -1;
		c = -1;
		return true;
	case 2:
		// �΂��u�����
		nonce.assign(buf, spaces[0] - buf);
		r = static_cast<int>(std::strtol(spaces[0] + 1, &end_r, 10));
		c = static_cast<int>(std::strtol(spaces[1] + 1, &end_c, 10));
		
		// ���l���w�肷�ׂ������Ő����ȊO�����������ꍇ�A�s���ȃt�H�[�}�b�g
		if(end_r != spaces[1] || end_c != buf + std::strlen(buf)){
			return false;
		}
		
		return true;
	default:
		return false;
	}
}

// (r, c)�̏ꏊ��player���΂�u���A���Ԃ��B
// ��ł����Ԃ�����true, �����łȂ����false
bool flip_stones(Othello::Placement & placement, int r, int c, char player_color, char opponent_color){
	// 8�����ɐ΂�L�΂��Ă���
	int t;
	bool flipped = false;
	
	for(int y = -1; y <= 1; ++y){
		for(int x = -1; x <= 1; ++x){
			if(x == 0 && y == 0) continue;
			
			// �܂��A�ׂ�����̐΂łȂ���΁A�΂͗��Ԃ��Ȃ�
			if(placement.get(r + y, c + x) != opponent_color){
				continue;
			}
			
			// �����āA��������΂�L�΂��Ă����āA
			// �ΐ푊��̐΂������āA���̂��Ǝ����̐΂������΂悢
			t = 2;
			while(placement.get(r + y*t, c + x*t) == opponent_color){
				++t;
			}
			
			if(placement.get(r + y*t, c + x*t) == player_color){
				// TODO: ������ۗ�����
				// �܂��΂�u��
				placement.put(r, c, player_color);
				
				// �΂𗠕Ԃ�
				for(; t >= 1; --t){
					placement.put(r + y*t, c + x*t, player_color);
				}
				flipped = true;
			}
		}
	}
	
	return flipped;
}

// ���C��
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
	
	std::string command, nonce, received_nonce;
	char buf[BUFSIZE];
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
				command.append(1, Othello::get_piece_name(placement.get(i, j)));
			}
		}
		
		// �R�}���h�����s���A���ʂ��󂯎��
		FILE * fp = POPEN(command.c_str(), "r");
		if(fp == NULL){
			std::cerr << "[ERROR] Failed in running: \"" << command << "\"" << std::endl;
			return 1;
		}
		
		fgets(buf, BUFSIZE, fp);
		size_t buflen = std::strlen(buf);
		
		// �����̉��s������
		if(buflen == 0 || buf[buflen - 1] != '\n'){
			std::cerr << "[ERROR] Player " << player << ": Too long result received (received \"" << buf << "\") [" << static_cast<int>(buf[buflen - 1]) << "]" << std::endl;
			return 1;
		}
		buf[buflen - 1] = '\0';
		
		// �󂯎��������������
		if(!parse_sent_string(buf, received_nonce, received_r, received_c)){
			std::cerr << "[ERROR] Player " << player << ": Invalid format received: \"" << buf << "\" (Expected: \"NONCE ROWNUM COLNUM\")" << std::endl;
			return 1;
		}
		
		if(nonce.compare(received_nonce) != 0){
			std::cerr << "[ERROR] Player " << player << ": Invalid nonce received: \"" << received_nonce << "\" (Expected: \"" << nonce << "\")" << std::endl;
			return 1;
		}
		
		// �L���Ȏ肪�w����Ă��邩�m�F
		if(received_r == -1 || received_c == -1){
			// �p�X�����ꍇ
			std::cout << "Player" << player << " passed his/her turn." << std::endl;
			if(last_choice_is_pass) break; // ��l�Ƃ��p�X�����ꍇ�͏I��
			last_choice_is_pass = true;
			continue;
		}
		
		std::cout << "Player" << player << " put at (" << received_r << ", " << received_c << ")." << std::endl;
		
		if(received_c < 0 || received_c >= Othello::SIZE || received_r < 0 || received_r >= Othello::SIZE){
			// ���W���s���ȏꍇ�i�Ֆʂ͈̔͊O�j
			std::cerr << "[ERROR] Player " << player << ": Invalid coordinates received: " << received_r << "," << received_c << std::endl;
			return 1;
		}
		
		if(placement.get(received_r, received_c) != Othello::EMPTY){
			// �΂��u����Ă��Ȃ��ꏊ�ȊO�ɒu�����Ƃ����ꍇ
			std::cerr << "[ERROR] Player " << player << ": Piece already exists" << std::endl;
			return 1;
		}
		
		last_choice_is_pass = false;
		
		// ���̏ꏊ�ɐ΂�u���ė��Ԃ�
		if(!flip_stones(placement, received_r, received_c, player_color, opponent_color)){
			// ����΂����Ԃ�Ȃ������ꍇ�͔���
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
