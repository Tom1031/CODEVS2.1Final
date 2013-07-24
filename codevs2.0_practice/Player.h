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
	Game game;//ゲームエンジンは常に使うので持っておく
	State best_state;//一番よい結果の状態を保存しておく
	long long best_score;//一番よいスコアを保存しておく
	int limit_sum_chain_num;
	int num_good_state;
	int limit_depth;//本当の意味での木の深さ制限　これないと　一生計算してしまう
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


	/*状態管理フラグを追加　このフラグを見て評価関数を切り替える*/
	bool is_attacked;//現在攻撃されている
	bool is_fatal_attacked;//致命的な攻撃を受けたかどうか
	bool is_enemy_prepared;//相手が強烈な一撃を打つことができる
	bool is_enemy_weak; //相手が弱っている　全力で攻撃だ　やれ！
//	bool is_enemy_filled;//相手のフィールドが埋まりすぎている
	int enemy_attack_power;//相手の現在の攻撃力
	int attack_power_now;//現在の攻撃力

	void observe_enemy();//相手の状態を観察する　フラグを更新する
	void calc_my_attack_power();

public:
	Player(int W,int H,int T,int S,int N,int P,int Th):num_good_state(1),limit_sum_chain_num(2),best_state(State()),best_score(0),game(Game(W,H,T,S,N,P,Th)),limit_depth(2),start_time(clock()),pre_turn(600){
		ifstream ifs("config.txt");//3回実行されてしまうので，ログに番号付けるために設定ファイルを外で用意する
		string name="log",idx;
		ifs>>idx;
		ifs.close();
		name+=idx;
		name+=".txt";
		ofstream ofs("config.txt");
		ofs<<(idx[0]-'0')+1<<endl;;//実行番号を書き込み
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