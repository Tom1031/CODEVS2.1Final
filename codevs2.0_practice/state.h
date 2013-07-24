#include<utility>
#include<iostream>
#include"util.h"
using namespace std;
#pragma once

class Info{
public:
	Info():chain_total(0),garbage_num(0),chain_max(0),erased_num(0),erased_max(0),point_Fc(0),turn(0){}
	long long chain_total;
	long long garbage_num;
	long long chain_max;
	long long erased_num;
	long long erased_max;
	long long point_Fc;
	long long turn;
};


class State{
	friend class Game;
	long long int value;//今まで得られることが確定した点数
	int depth;//次に探索する深さ　これはチェインの総数
	pair<int,int> order[1000];//今までおこなってきた命令列 (x,rotation)
	int step_idx;//今何個目のパックを落とすのか　0origin
	int Fc;
	long long int value_Fc;
	int num_garbage;
	int num_normal_block;
public:
	int field[MAX_H][MAX_W];//フィールドを表す　下が0となるように変更

	State():step_idx(0),value(0),depth(2),Fc(0),num_garbage(0),num_normal_block(0){//ホントに最初の状態を作るとき
		memset(field,0,sizeof(field));
	}

	long long int get_value()const{return value;}
	void update_value(const long long int v){
		value=v;
	}
	void update_value_Fc(const long long int v){
		value_Fc=v;
	}
	int get_step_idx()const{return step_idx;}
	int get_depth()const{return depth;}
	void increment_depth(){depth++;}
	void reset_depth(){depth=3;}
	int get_num(const int &x,const int &y)const{return field[y][x];}
	const pair<int,int>* get_order()const{return order;}
	int get_height(){
		int res=0;
		for(int x=0;x<20;x++){
			for(int y=res;y<36+5;y++){
				if(field[y][x]==0){
					res=y;
					break;
				}
			}
		}
		return res;
	}
	void increment_Fc(){Fc++;}
	int get_Fc()const{return Fc;}
	long long get_value_Fc()const{return value_Fc;}
	int get_num_garbage()const{return num_garbage;}
	int get_num_normal_block()const{return num_normal_block;}
	int get_ratio_normal_block()const{
		if(num_normal_block+num_garbage==0) return 0;
//		return 10000-num_garbage;
//		return (min((int)(num_garbage*0.6),num_normal_block)*10000)/(min((int)(num_garbage*0.6),num_normal_block)+num_garbage);
//		return (num_normal_block*10000)/(num_normal_block+num_garbage);
//		return (num_normal_block*1000000)/(num_normal_block+num_garbage*100);
//		return 10000-(num_garbage-num_normal_block*5);
		return 100000000*pow((min(80,num_normal_block)),0.5)/(num_garbage);
//		return 100000000*pow((num_normal_block),0.3)/(num_garbage);
//		return 10000-(num_garbage-num_normal_block*4);
//		return 100000-num_garbage*4+num_normal_block;
	}
};
