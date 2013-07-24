#include"Player.h"
#include<string>
#include<time.h>
#include<omp.h>
#include<vector>
#include<cassert>

void Player::calc_my_attack_power(){
	attack_power_now=0;
	for(int x1=-game.T;x1<game.W+game.T;x1++){//2ターン先読みして　自分の攻撃力がどんなものか見てみる
	for(int r1=0;r1<4;r1++){
		for(int x2=-game.T;x2<game.W+game.T;x2++){
		for(int r2=0;r2<4;r2++){
			State copy=game.get_state();
			if(copy.get_step_idx()<game.N){
				bool is_garbage=false;
				if(my_stock_garbage>=20) is_garbage=true;
				Info got=game.update(copy,x1,r1,game.H,false,is_garbage);
				attack_power_now=max(attack_power_now,(int)got.garbage_num);
				if(copy.get_step_idx()<game.N){
					is_garbage=false;
					if(my_stock_garbage>=40) is_garbage=true;
					got=game.update(copy,x2,r2,game.H,false,is_garbage);
					attack_power_now=max(attack_power_now,(int)got.garbage_num);
				}
			}
		}
		}
	}
	}
}

void Player::observe_enemy(){
	is_fatal_attacked=false;//致命的な攻撃を受けたかどうか
	is_enemy_prepared=false;//相手が強烈な一撃を打つことができる
	is_enemy_weak=false; //相手が弱っている　全力で攻撃だ　やれ！
//	is_enemy_filled=false;//相手のフィールドが埋まりすぎている
	enemy_attack_power=0;//相手の現在の攻撃力

	if(my_stock_garbage>=150){//やばい　本気の攻撃だ
		is_fatal_attacked=true;
//		log<<"#########FATAL ATTACKED!!#########"<<endl;
	}
//	log<<"ENEMY STATE"<<endl;
//	print(game.get_enemy_state());

	for(int x1=-game.T;x1<game.W+game.T;x1++){//2ターン先読みして　相手の攻撃力がどんなものか見てみる
	for(int r1=0;r1<4;r1++){
		for(int x2=-game.T;x2<game.W+game.T;x2++){
		for(int r2=0;r2<4;r2++){
			State copy=game.get_enemy_state();
			if(copy.get_step_idx()<game.N){
				bool is_garbage=false;
				if(enemy_stock_garbage>=20) is_garbage=true;
				Info got=game.update(copy,x1,r1,game.H,true,is_garbage);
				enemy_attack_power=max(enemy_attack_power,(int)got.garbage_num);
				if(copy.get_step_idx()<game.N){
					is_garbage=false;
					if(enemy_stock_garbage>=40) is_garbage=true;
					got=game.update(copy,x2,r2,game.H,true,is_garbage);
					enemy_attack_power=max(enemy_attack_power,(int)got.garbage_num);
				}
			}
		}
		}
	}
	}
	State enemy_state=game.get_enemy_state();
	enemy_attack_power=max(enemy_attack_power,(enemy_state.get_num_normal_block()+enemy_state.get_num_garbage())*2);
//	print(game.get_enemy_state());
	if(enemy_attack_power>150) is_enemy_prepared=true;//相手が攻撃体制に入っている
}

bool Player::input(){
	bool is_attacked2=false;
	is_attacked2=game.input_pack_changed();
	game.update_enemy_state();
	cin>>my_stock_garbage>>enemy_stock_garbage;
	return is_attacked2;
}

bool Player::is_equal(const State &a, const State& b)const{
	if(a.get_value()!=b.get_value()) return false;

	for(int x=0;x<game.W;x++){
		for(int y=0;y<game.H;y++){
			int n1=a.get_num(x,y);
			int n2=b.get_num(x,y);
			if(n1!=n2) return false;
			if(n1<0 && n2<0){
				break;
			}
		}
	}
	return true;
}


void Player::calc(){//思考ルーチン本体
	turn_honki=555;
	time_passed=0;
	const int num_stock_turn=10;//どれくらい先まで読むか
	int beam_length=30;//ビーム幅のこと
	limit_depth=2;//深さ制限
	bool is_attack=false;
	State save_point=State();
	is_attacked=false;
	bool is_success_updated=false;//バグ対策　一応
	while(true){
/*		if(game.get_turn()==0){//1ターン目にランダムに行動するメソッド
			State temp=game.get_state();
			game.update(temp,rand()%20,rand()%4,36,false,false);
			is_success_updated=game.output(temp);
			continue;
		}*/
		bool bef_is_attacked=is_attacked;
		bool is_changed_attack_state=false;//攻撃受けていない⇒受けた！　またはその逆とかは　探索やり直さないと
		if(game.get_turn()>0 && is_success_updated){//入力が飛んでくるので受け取る
			is_attacked=input();
			is_success_updated=false;
		}
		//相手の先読み＋自分の攻撃力の近似
		observe_enemy();
		calc_my_attack_power();

		if(bef_is_attacked!=is_attacked){
			is_changed_attack_state=true;
		}
		time_t bef=clock();//始まった時間

		if(is_changed_attack_state){//攻撃されたのでリセット
//			log<<"STATE CHANGED search again"<<endl;
			best_state=State();
			save_point=State();
			is_attack=false;
//			best_score=0;
		}

		if(is_changed_attack_state || (!is_attack && save_point.get_step_idx()<=game.get_turn()) || game.get_turn()==0){//セーブされていなかったら　ビームサーチ！
			long long before_best_score=best_score;
			best_score=0;
			best_state=State();
//			log<<"search!"<<endl;
//			if(game.get_turn()>=turn_honki){
//				log<<"HONKI MODE"<<endl;
//			}
			list<State> list_good_state;
			list_good_state.push_back(game.get_state());
			while(list_good_state.front().get_step_idx()<game.get_turn()+num_stock_turn){//注意！！！　まだ終了しない可能性　バグ
				vector<list<State> > next_states(list_good_state.size());
				int temp=0;
				num_good_state=5;
				for(list<State>::iterator state=list_good_state.begin(); state!=list_good_state.end();++state){//ビームサーチ！ビーム！！
					search_tree(*state,next_states[temp],0,clock());
					temp++;
				}
				list<State> next_state;
				num_good_state=beam_length;
				for(int i=0;i<next_states.size();i++){
					for(list<State>::iterator it = next_states[i].begin();it!=next_states[i].end();it++){
						insert_state(next_state,*it,num_good_state);
					}
				}
				list_good_state=next_state;//次行くよ！！　次！！
				if(best_score>=100000*10000-10000*1000 && best_state.get_step_idx()<turn_honki){
//					log<<"FcUP!!"<<endl;
					break;
				}
				if(list_good_state.empty()){//嫌なことが起こっているかも
//					log<<"#######EMPTY!!########"<<endl;
					cout<<0<<" "<<0<<endl;
					break;
//					cout<<"Error"<<endl;
				}
//				log<<"size "<<list_good_state.size()<<endl;
//				log<<"search end once "<<endl;
			}
			bool is_stop_search=false;
			if(best_score>=100000*10000-10000*1000 || is_fatal_attacked){//スコアが100000*10000を超えていると攻撃しても良い合図　またはカウンターしないと死ぬ
				is_stop_search=true;
			}
			if(is_stop_search){//けっこう先まで読んだけどスコアが上がらなかった
				is_attack=true;
//				log<<"ATTACK ON"<<endl;
//				print(best_state);
			}
			else{//まだまだスコアは上がる気がする
				save_point=list_good_state.front();//指定したターン先まで読んだのでセーブするね!!
//				log<<"NEXT"<<endl;
			}
		}
		if(is_attack){
//			log<<"attack"<<endl;
			is_success_updated=game.output(best_state);
//			game.output(best_state);

			//			print(game.get_state());
			if(game.get_turn()>=best_state.get_step_idx()){
				is_attack=false;//連鎖完了！
				turn_attacked=game.get_turn();
//				log<<"END attack"<<endl;
//				cout<<"stop "<<endl;
				best_state=State();
				save_point=State();
				best_score=0;
			}
		}
		else{
//			log<<"move save point"<<endl;
			is_success_updated=game.output(save_point);//セーブポイントをoutput！
//			game.output(save_point);//セーブポイントをoutput！
		}
		if(!is_success_updated){//なんかエラー起きるんだけど やり直すし
			best_score=0;
			best_state=State();
			save_point=State();
//			log<<"@@@@@@@@@@@@@Error@@@@@@@@@@@"<<endl;
		}
		time_passed+=clock()-bef;
//		log<<"time: "<<time_passed/CLOCKS_PER_SEC<<endl;
	}
	return;
}


//状態が与えられるので，よさそうならこれをリストに入れる，そうでないなら何もしない
void Player::insert_state(list<State> &list_state,State state,int max_num){
	//	if(state.get_value()<=0) return;
	if(list_state.empty()){
		list_state.push_back(state);
		return;
	}

	if(state.get_value()<list_state.back().get_value()){//リストの一番後ろよりも小さい・・・
		if(list_state.size()<max_num){//まだ個数が制限未満　後ろにいれる
			list_state.push_back(state);
			return;
		}
		else{//制限以上　つまりいらない子であったことが判明
			return;//何もせずさようなら
		}
	}

	//必要な子であることはわかったので，後ろからどこに入れるべきかを調べ，挿入（挿入ソート）

	for (list<State>::iterator it=list_state.begin(); it!=list_state.end();it++){
		if(it->get_value()==state.get_value()){//点数が同じだったら同じかどうかの判定を行う
			if(is_equal(*it,state)){
				return;
			}
			//			return;
		}
		if(it->get_value()<state.get_value()){//価値が自分未満のものを発見したら挿入
			list_state.insert(it,state);//そいつの直前に挿入するとソートされたまま
			break;
		}
	}

	if(list_state.size()>max_num){//制限以上の個数がリストに入っている
		list_state.pop_back();//一番後ろはもうさようなら
	}
	return;
}

/*
ゲーム木をもぐる
*/
long long int Player::search_tree(State state,list<State> &list_good_state,int depth,const time_t time){
	if(depth>=limit_depth){
		return 0;
	}
	if(state.get_step_idx()>=1000) return 0;

	list<State> temp_list;
	long long max_value=0;
	bool is_incremented=false;
	bool is_garbage=false;
	if(my_stock_garbage>0){
		is_garbage=true;
	}
	if(is_fatal_attacked){
//		log<<"debug is fatal"<<endl;
		is_garbage=true;
	}
	for(int x=-game.T+1;x<game.W+game.T;x++){
		for(int r=0;r<4;r++){
			State copy=state;
			State copy2=state;
			const Info got=game.update(copy,x,r,game.H,false,is_garbage);//実行
			if(got.garbage_num<0) continue;//死んだ
			long long value;
			value=got.garbage_num;
			if(game.get_turn()>=turn_honki){//そろそろ攻撃し始めても良いか
				if((state.get_num_normal_block()+state.get_num_garbage())>=450){//もうフィールドがいっぱいです　攻撃しよう？
					value=100000*10000+got.garbage_num;
				}
				else value=got.garbage_num;//まだまだまだためるよー！！
			}
			else if(is_fatal_attacked){//やばい　すごい致命的な攻撃をくらってる！
				value=10000*1000-abs(((my_stock_garbage-20*(copy.get_step_idx()-game.get_turn()))-got.garbage_num));//送られたお邪魔と同じくらいのやつを返す
				value+=copy.get_num_normal_block()-max(0,copy.get_num_garbage()-50);
				if(got.point_Fc>=10000) value+=100;
			}
			else if(got.point_Fc<10000){//お話にならないくらいポイントが低い
				value=got.point_Fc;
			}
			else if(!is_enemy_prepared){//相手がまだ攻撃準備できていない　Fc上げる
				is_incremented=true;
				value=100000*10000-copy.get_step_idx()*10000-max(0,copy.get_num_garbage()-50)+copy.get_num_normal_block();
			}
			else{//相手が攻撃してきそうだよ・・・
				if(copy.get_num_normal_block()>=game.get_enemy_state().get_num_normal_block()/2+game.get_enemy_state().get_num_garbage()/2){//カウンターできそうでブロックを消しすぎないとき
					is_incremented=true;
					value=100000*10000-copy.get_step_idx()*10000-max(0,copy.get_num_garbage()-50)+copy.get_num_normal_block();//どうせ攻撃されてもカウンターすればよい　Fcあげる
				}
				else{
					value=10000*got.garbage_num;//今打つとやばい死ぬ！　カウンター性能を上げる
				}
			}

			if(copy.get_step_idx()>=1000) value=10000*10000;
			max_value=max(max_value,value);
			if(value>best_score){//ベストスコアを更新したよ！
				best_state=copy;
				best_score=value;
//				log<<"attacked:"<<is_attacked<<" fatal_attacked:"<<is_fatal_attacked<<" enemy_prepared:"<<is_enemy_prepared<<endl;
//				log<<"my_attack_power:"<<attack_power_now<<" enemy_attack_power:"<<enemy_attack_power<<endl;
//				log<<value<<" normal:"<<copy.get_num_normal_block()<<" garbage"<<copy.get_num_garbage()<<endl;
//				if(is_fatal_attacked){
//					log<<"stock:"<<my_stock_garbage-20*(copy.get_step_idx()-game.get_turn())<<" counter:"<<got.garbage_num<<endl;
//				}
//				if(game.get_turn()>=turn_honki){
//					log<<"bef_normal:"<<state.get_num_normal_block()<<" bef_garbage:"<<state.get_num_garbage()<<endl;
//				}
//				log<<"turn :"<<copy.get_step_idx()<<" Fc:"<<copy.get_Fc()<<" garbage:"<<got.garbage_num<<" score:"<<got.point_Fc<<" chain:"<<got.chain_max<<" num:"<<got.erased_max<<endl;
				//				print_info(got);
//				log<<endl;
			}
			if(!is_incremented){
				search_tree(copy,list_good_state,depth+1,time);
			}
		}
	}
	state.update_value(max_value);
//	for(list<State>::iterator a = temp_list.begin();a!=temp_list.end();a++){
//		insert_state(list_good_state,*a,num_good_state);
//	}

	if(depth>=1){
		insert_state(list_good_state,state,num_good_state);//さて深く探索するに値するstateなの？
	}
	return 0;
}
