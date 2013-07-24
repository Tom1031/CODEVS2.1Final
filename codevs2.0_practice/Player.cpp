#include"Player.h"
#include<string>
#include<time.h>
#include<omp.h>
#include<vector>
#include<cassert>

void Player::calc_my_attack_power(){
	attack_power_now=0;
	for(int x1=-game.T;x1<game.W+game.T;x1++){//2�^�[����ǂ݂��ā@�����̍U���͂��ǂ�Ȃ��̂����Ă݂�
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
	is_fatal_attacked=false;//�v���I�ȍU�����󂯂����ǂ���
	is_enemy_prepared=false;//���肪����Ȉꌂ��ł��Ƃ��ł���
	is_enemy_weak=false; //���肪����Ă���@�S�͂ōU�����@���I
//	is_enemy_filled=false;//����̃t�B�[���h�����܂肷���Ă���
	enemy_attack_power=0;//����̌��݂̍U����

	if(my_stock_garbage>=150){//��΂��@�{�C�̍U����
		is_fatal_attacked=true;
//		log<<"#########FATAL ATTACKED!!#########"<<endl;
	}
//	log<<"ENEMY STATE"<<endl;
//	print(game.get_enemy_state());

	for(int x1=-game.T;x1<game.W+game.T;x1++){//2�^�[����ǂ݂��ā@����̍U���͂��ǂ�Ȃ��̂����Ă݂�
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
	if(enemy_attack_power>150) is_enemy_prepared=true;//���肪�U���̐��ɓ����Ă���
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


void Player::calc(){//�v�l���[�`���{��
	turn_honki=555;
	time_passed=0;
	const int num_stock_turn=10;//�ǂꂭ�炢��܂œǂނ�
	int beam_length=30;//�r�[�����̂���
	limit_depth=2;//�[������
	bool is_attack=false;
	State save_point=State();
	is_attacked=false;
	bool is_success_updated=false;//�o�O�΍�@�ꉞ
	while(true){
/*		if(game.get_turn()==0){//1�^�[���ڂɃ����_���ɍs�����郁�\�b�h
			State temp=game.get_state();
			game.update(temp,rand()%20,rand()%4,36,false,false);
			is_success_updated=game.output(temp);
			continue;
		}*/
		bool bef_is_attacked=is_attacked;
		bool is_changed_attack_state=false;//�U���󂯂Ă��Ȃ��ˎ󂯂��I�@�܂��͂��̋t�Ƃ��́@�T����蒼���Ȃ���
		if(game.get_turn()>0 && is_success_updated){//���͂����ł���̂Ŏ󂯎��
			is_attacked=input();
			is_success_updated=false;
		}
		//����̐�ǂ݁{�����̍U���͂̋ߎ�
		observe_enemy();
		calc_my_attack_power();

		if(bef_is_attacked!=is_attacked){
			is_changed_attack_state=true;
		}
		time_t bef=clock();//�n�܂�������

		if(is_changed_attack_state){//�U�����ꂽ�̂Ń��Z�b�g
//			log<<"STATE CHANGED search again"<<endl;
			best_state=State();
			save_point=State();
			is_attack=false;
//			best_score=0;
		}

		if(is_changed_attack_state || (!is_attack && save_point.get_step_idx()<=game.get_turn()) || game.get_turn()==0){//�Z�[�u����Ă��Ȃ�������@�r�[���T�[�`�I
			long long before_best_score=best_score;
			best_score=0;
			best_state=State();
//			log<<"search!"<<endl;
//			if(game.get_turn()>=turn_honki){
//				log<<"HONKI MODE"<<endl;
//			}
			list<State> list_good_state;
			list_good_state.push_back(game.get_state());
			while(list_good_state.front().get_step_idx()<game.get_turn()+num_stock_turn){//���ӁI�I�I�@�܂��I�����Ȃ��\���@�o�O
				vector<list<State> > next_states(list_good_state.size());
				int temp=0;
				num_good_state=5;
				for(list<State>::iterator state=list_good_state.begin(); state!=list_good_state.end();++state){//�r�[���T�[�`�I�r�[���I�I
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
				list_good_state=next_state;//���s����I�I�@���I�I
				if(best_score>=100000*10000-10000*1000 && best_state.get_step_idx()<turn_honki){
//					log<<"FcUP!!"<<endl;
					break;
				}
				if(list_good_state.empty()){//���Ȃ��Ƃ��N�����Ă��邩��
//					log<<"#######EMPTY!!########"<<endl;
					cout<<0<<" "<<0<<endl;
					break;
//					cout<<"Error"<<endl;
				}
//				log<<"size "<<list_good_state.size()<<endl;
//				log<<"search end once "<<endl;
			}
			bool is_stop_search=false;
			if(best_score>=100000*10000-10000*1000 || is_fatal_attacked){//�X�R�A��100000*10000�𒴂��Ă���ƍU�����Ă��ǂ����}�@�܂��̓J�E���^�[���Ȃ��Ǝ���
				is_stop_search=true;
			}
			if(is_stop_search){//����������܂œǂ񂾂��ǃX�R�A���オ��Ȃ�����
				is_attack=true;
//				log<<"ATTACK ON"<<endl;
//				print(best_state);
			}
			else{//�܂��܂��X�R�A�͏オ��C������
				save_point=list_good_state.front();//�w�肵���^�[����܂œǂ񂾂̂ŃZ�[�u�����!!
//				log<<"NEXT"<<endl;
			}
		}
		if(is_attack){
//			log<<"attack"<<endl;
			is_success_updated=game.output(best_state);
//			game.output(best_state);

			//			print(game.get_state());
			if(game.get_turn()>=best_state.get_step_idx()){
				is_attack=false;//�A�������I
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
			is_success_updated=game.output(save_point);//�Z�[�u�|�C���g��output�I
//			game.output(save_point);//�Z�[�u�|�C���g��output�I
		}
		if(!is_success_updated){//�Ȃ񂩃G���[�N����񂾂��� ��蒼����
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


//��Ԃ��^������̂ŁC�悳�����Ȃ炱������X�g�ɓ����C�����łȂ��Ȃ牽�����Ȃ�
void Player::insert_state(list<State> &list_state,State state,int max_num){
	//	if(state.get_value()<=0) return;
	if(list_state.empty()){
		list_state.push_back(state);
		return;
	}

	if(state.get_value()<list_state.back().get_value()){//���X�g�̈�Ԍ������������E�E�E
		if(list_state.size()<max_num){//�܂��������������@���ɂ����
			list_state.push_back(state);
			return;
		}
		else{//�����ȏ�@�܂肢��Ȃ��q�ł��������Ƃ�����
			return;//�����������悤�Ȃ�
		}
	}

	//�K�v�Ȏq�ł��邱�Ƃ͂킩�����̂ŁC��납��ǂ��ɓ����ׂ����𒲂ׁC�}���i�}���\�[�g�j

	for (list<State>::iterator it=list_state.begin(); it!=list_state.end();it++){
		if(it->get_value()==state.get_value()){//�_���������������瓯�����ǂ����̔�����s��
			if(is_equal(*it,state)){
				return;
			}
			//			return;
		}
		if(it->get_value()<state.get_value()){//���l�����������̂��̂𔭌�������}��
			list_state.insert(it,state);//�����̒��O�ɑ}������ƃ\�[�g���ꂽ�܂�
			break;
		}
	}

	if(list_state.size()>max_num){//�����ȏ�̌������X�g�ɓ����Ă���
		list_state.pop_back();//��Ԍ��͂������悤�Ȃ�
	}
	return;
}

/*
�Q�[���؂�������
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
			const Info got=game.update(copy,x,r,game.H,false,is_garbage);//���s
			if(got.garbage_num<0) continue;//����
			long long value;
			value=got.garbage_num;
			if(game.get_turn()>=turn_honki){//���낻��U�����n�߂Ă��ǂ���
				if((state.get_num_normal_block()+state.get_num_garbage())>=450){//�����t�B�[���h�������ς��ł��@�U�����悤�H
					value=100000*10000+got.garbage_num;
				}
				else value=got.garbage_num;//�܂��܂��܂����߂��[�I�I
			}
			else if(is_fatal_attacked){//��΂��@�������v���I�ȍU����������Ă�I
				value=10000*1000-abs(((my_stock_garbage-20*(copy.get_step_idx()-game.get_turn()))-got.garbage_num));//����ꂽ���ז��Ɠ������炢�̂��Ԃ�
				value+=copy.get_num_normal_block()-max(0,copy.get_num_garbage()-50);
				if(got.point_Fc>=10000) value+=100;
			}
			else if(got.point_Fc<10000){//���b�ɂȂ�Ȃ����炢�|�C���g���Ⴂ
				value=got.point_Fc;
			}
			else if(!is_enemy_prepared){//���肪�܂��U�������ł��Ă��Ȃ��@Fc�グ��
				is_incremented=true;
				value=100000*10000-copy.get_step_idx()*10000-max(0,copy.get_num_garbage()-50)+copy.get_num_normal_block();
			}
			else{//���肪�U�����Ă���������E�E�E
				if(copy.get_num_normal_block()>=game.get_enemy_state().get_num_normal_block()/2+game.get_enemy_state().get_num_garbage()/2){//�J�E���^�[�ł������Ńu���b�N�����������Ȃ��Ƃ�
					is_incremented=true;
					value=100000*10000-copy.get_step_idx()*10000-max(0,copy.get_num_garbage()-50)+copy.get_num_normal_block();//�ǂ����U������Ă��J�E���^�[����΂悢�@Fc������
				}
				else{
					value=10000*got.garbage_num;//���łƂ�΂����ʁI�@�J�E���^�[���\���グ��
				}
			}

			if(copy.get_step_idx()>=1000) value=10000*10000;
			max_value=max(max_value,value);
			if(value>best_score){//�x�X�g�X�R�A���X�V������I
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
		insert_state(list_good_state,state,num_good_state);//���Đ[���T������ɒl����state�Ȃ́H
	}
	return 0;
}
