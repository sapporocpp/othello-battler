// ------------------------------------------------------------
// 2015.10.17 Sapporo.cpp ��9��׋��� ����u�I�Z���v
// ��FH.Hiro <main@hhiro.net>
// ------------------------------------------------------------

#include <iostream>
#include "othello_lib.hpp"

// �w�肵���ꏊ�ɐ΂��u����i�����Ȃ��Ƃ���A����̐΂𗠕Ԃ���j������
bool possible(const Othello::Board & board, int i, int j){
	// �������̏ꏊ�ɂ��łɐ΂�����ꍇ�́A�u���Ȃ�
	if(board.place(i, j) != Othello::EMPTY) return false;
	
	// 8�����ɐ΂�L�΂��Ă���
	int t;
	for(int y = -1; y <= 1; ++y){
		for(int x = -1; x <= 1; ++x){
			if(x == 0 && y == 0) continue;
			
			// �܂��A�ׂ�����̐΂łȂ���΁A�΂͗��Ԃ��Ȃ�
			if(board.place(i + y, j + x) != board.opponent_color()){
				continue;
			}
			
			// �����āA��������΂�L�΂��Ă����āA
			// �ΐ푊��̐΂������āA���̂��Ǝ����̐΂������΂悢
			t = 2;
			while(board.place(i + y*t, j + x*t) == board.opponent_color()){
				++t;
			}
			
			if(board.place(i + y*t, j + x*t) == board.my_color()){
				return true;
			}
		}
	}
	
	return false;
}

int main(int argc, char ** argv){
	// �Ֆʂ��擾
	Othello::Board board(argc, argv);
	
	// �Ֆʂ����āA�u����ꏊ�����邩�m�F����B
	for(int i = 0; i < Othello::SIZE; ++i){
		for(int j = 0; j < Othello::SIZE; ++j){
			// ���̃T���v���ł́A�ŏ��Ɂu�����ɂȂ�u���Ă悢�v��
			// �킩�����ꏊ�ɒu�����Ƃɂ��Ă���B
			if(possible(board, i, j)){
				board.put(i, j);
				return 0;
			}
		}
	}
	
	board.put(); // �ǂ��ɂ��u���Ȃ�
	return 0;
}
