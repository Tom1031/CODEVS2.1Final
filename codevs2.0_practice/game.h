#pragma once

#include"state.h"
#include<iostream>
#include<string>
#include<utility>
#include<vector>

/*
ゲーム全体の情報を持っておく
パックの情報，今の状態
連鎖などの計算をここで行う
*/

class Game{
	int ****packs;//パックの中身　もらったら　事前に全部回転させたのも計算しちゃう
	int ****packs_garbage;
//	int packs_garbage[1000][4][MAX_T][MAX_T];
	int turn;
	State now_state;//現在のState
	long long int total_point;

	void input_pack();
	bool check_inside(const State &state,const int &x,const int &r,bool is_garbage)const;
	bool check_over(const State &state,const int h)const;
	void push_pack(State &state,const int &x,const int &r,bool is_enemy,bool is_garbage);
	void drop_block(State &state,int change_info[][2][MAX_W+MAX_H+MAX_T],bool is_drop[MAX_W])const;//ブロックを全て下まで落とす
	int erase(State &state,int change_info[][2][MAX_W+MAX_H+MAX_T],bool is_drop[MAX_W])const;
	int erase_vertically(State &state,bool is_erased[MAX_H][MAX_W],int change_info[2][MAX_W+MAX_H+MAX_T],int change_pos[2][MAX_H])const;
	int erase_horizontally(State &state,bool is_erased[MAX_H][MAX_W],int change_info[2][MAX_W+MAX_H+MAX_T],int change_pos[2][MAX_H])const;
	int erase_right_down(State &state,bool is_erased[MAX_H][MAX_W],int change_info[2][MAX_W+MAX_H+MAX_T],int change_pos[2][MAX_H])const;
	int erase_left_down(State &state,bool is_erased[MAX_H][MAX_W],int change_info[2][MAX_W+MAX_H+MAX_T],int change_pos[2][MAX_H])const;
	long long int calc_point(const int &C,const int &E,const int &F)const;
	long long calc_garbage_block_point(const int &C,const int &E,const int &Fc);
	bool is_attacked();//攻撃されたらtrueが返る
	pair<int,long long int> put_imaginally_block(State state,const int &num,const int &margine,const int &x);

	int ****packs_enemy;
	State state_enemy;
	int my_stock_garbage,enemy_stock_garbage;


public:

	void input_order(int x,int r){
		cout<<x<<" "<<r<<endl;
		turn++;
	}

	bool input_pack_changed();
	long long update_enemy_state();
	void input_stock_garbage();

	long long int evaluate_state(State state);
	int calc_gurantee_point(const State &state)const;
	int H,W,N,T,S,P,Th;
	void input_order(const int &num,const pair<int,int> order[1000]);
	int calc_gap(const State &state)const;
	State get_state()const{return now_state;}
	Game(int W,int H,int T,int S,int N,int P,int Th):N(N),H(H),W(W),T(T),S(S),P(P),turn(0),Th(Th),my_stock_garbage(0),enemy_stock_garbage(0){
		packs = new int***[1000];
		packs_garbage= new int***[1000];
		packs_enemy = new int***[1000];
		for(int i=0;i<1000;i++){
			packs[i]= new int**[4];
			packs_garbage[i]=new int**[4];
			packs_enemy[i]=new int**[4];
			for(int j=0;j<4;j++){
				packs[i][j]=new int*[MAX_T];
				packs_garbage[i][j]=new int*[MAX_T];
				packs_enemy[i][j]= new int*[MAX_T];
				for(int z=0;z<MAX_T;z++){
					packs[i][j][z]=new int[MAX_T];
					packs_garbage[i][j][z]=new int[MAX_T];
					packs_enemy[i][j][z]=new int[MAX_T];
					for(int k=0;k<MAX_T;k++){
						packs[i][j][z][k]=0;
						packs_garbage[i][j][z][k]=0;
						packs_enemy[i][j][z][k]=0;
					}
				}
			}
		}
		string name="game_log.txt";
		input_pack();
		total_point=0;
	}

	void get_pack(const int &idx,const int &rotation,int res[][MAX_T])const{
		memcpy(res,packs[idx][rotation],sizeof(packs[idx][rotation]));
	}
	
	Info update(State &state,const int x,const int r,const int h,bool is_enemy,bool is_garbage);
	int get_turn()const{return turn;}

	void random_order(){
		cout<<0<<" "<<0<<endl;
		update(now_state,0,0,H,true,false);
		//		print(now_state);
		turn++;
	}

/*	void print(State &state){
		game_log<<"turn :"<<state.get_step_idx()<<endl;
		for(int y=MAX_H+MAX_T;y>=0;y--){
			for(int x=0;x<MAX_W;x++){
				if(x>=W || y>=(int)H+(int)MAX_T) continue;
				else if(state.field[y][x]==0) game_log<<"..";
				else{
					if(state.field[y][x]>S) game_log<<"##";
					else{
						game_log<<state.field[y][x];
						if(state.field[y][x]<10) game_log<<" ";
					}
				}
				game_log<<" ";
			}
			if(y>=(int)H+(int)MAX_T) continue;
			game_log<<endl;
		}
		game_log<<endl;
	}*/

	bool output(State &state);
	State get_enemy_state()const{return state_enemy;}
};