#pragma once
#include"game.h"
#include"util.h"
#include<list>
#include<fstream>
#include<iostream>
#include<string>
#include<time.h>
using namespace std;

class Player{
	Game game;//�Q�[���G���W���͏�Ɏg���̂Ŏ����Ă���
	State best_state;//��Ԃ悢���ʂ̏�Ԃ�ۑ����Ă���
	long long best_score;//��Ԃ悢�X�R�A��ۑ����Ă���
	int limit_sum_chain_num;
	int num_good_state;
	int limit_depth;//�{���̈Ӗ��ł̖؂̐[�������@����Ȃ��Ɓ@�ꐶ�v�Z���Ă��܂�
	void insert_state(list<State> &list_state,State state,int max_num);
	long long int search_tree(State state,list<State> &list_good_state,const int depth,time_t start_time);
	const time_t start_time;
	int limit_height;
	int gap_cap;
	int pre_turn;
	int turn_Fc_update;
	bool is_equal(const State &a,const State &b)const;
	bool input();
	time_t time_passed;

	int my_stock_garbage,enemy_stock_garbage;
	int turn_attacked;
	State best_state_Fc;
	long long best_score_Fc;

	int turn_honki;


	/*��ԊǗ��t���O��ǉ��@���̃t���O�����ĕ]���֐���؂�ւ���*/
	bool is_attacked;//���ݍU������Ă���
	bool is_fatal_attacked;//�v���I�ȍU�����󂯂����ǂ���
	bool is_enemy_prepared;//���肪����Ȉꌂ��ł��Ƃ��ł���
	bool is_enemy_weak; //���肪����Ă���@�S�͂ōU�����@���I
//	bool is_enemy_filled;//����̃t�B�[���h�����܂肷���Ă���
	int enemy_attack_power;//����̌��݂̍U����
	int attack_power_now;//���݂̍U����

	void observe_enemy();//����̏�Ԃ��ώ@����@�t���O���X�V����
	void calc_my_attack_power();

public:
	Player(int W,int H,int T,int S,int N,int P,int Th):num_good_state(1),limit_sum_chain_num(2),best_state(State()),best_score(0),game(Game(W,H,T,S,N,P,Th)),limit_depth(2),start_time(clock()),pre_turn(600){
		ifstream ifs("config.txt");//3����s����Ă��܂��̂ŁC���O�ɔԍ��t���邽�߂ɐݒ�t�@�C�����O�ŗp�ӂ���
		string name="log",idx;
		ifs>>idx;
		ifs.close();
		name+=idx;
		name+=".txt";
		ofstream ofs("config.txt");
		ofs<<(idx[0]-'0')+1<<endl;;//���s�ԍ�����������
//		halt_message.open("halt.txt");
		name="player_log.txt";
//		log.open(name);
		turn_attacked=0;
		best_state_Fc=State();
		best_score_Fc=0;
	};
	void calc();

/*	void print(const State state){
		log<<"turn :"<<state.get_step_idx()<<endl;
		for(int y=MAX_H+MAX_T;y>=0;y--){
			for(int x=0;x<MAX_W;x++){
				if(x>=game.W || y>=(int)game.H+(int)MAX_T) continue;
				else if(state.field[y][x]==0) log<<"..";
				else{
					if(state.field[y][x]>game.S) log<<"##";
					else{
						log<<state.field[y][x];
						if(state.field[y][x]<10) log<<" ";
					}
				}
				log<<" ";
			}
			if(y>=(int)game.H+(int)MAX_T) continue;
			log<<endl;
		}
		log<<endl;
	}*/
};