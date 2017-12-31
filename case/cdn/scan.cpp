#include "scan.h"


bool Comp(const p& p1, const p& p2) {
	return p1.size > p2.size;
}

void scanNetwork(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& min_flow, int min_sum, int serverCost) {
	int V = size.NN + size.NC + size.NS;
	int N = size.NN;
	int tempCost = INF, tempFlow = -INF;
	int find_min_cost, find_max_flow;
	int* unoccupied = new int[N];
	char* used = new char[G.size()];
	int* dist = new int[G.size()];
	int* prevv = new int[G.size()];
	int* prevv_inv = new int[G.size()];
	int* preve = new int[G.size()];
	bool flag = false;
	vector<int> serverId;
	vector<int> t_state;
	for (int i = 0; i < N; ++i)
		unoccupied[i] = i;
	while (N) {
		if (!flag) {
			//if (serverId.size() > 5)	break;
			for (int i = 0; i < size.NN; ++i) {
				if (unoccupied[i] == -1)	continue;
				t_state = serverId;
				t_state.push_back(i);
				adjustGraph(G, t_state, table, min_flow, size);
				backup(G, size);
				int flow = getMaxFlow(G, min_flow, size, used);
				if (flow < min_sum) {
					//cout << "Cannot satisfy!" << endl;
					if (flow > tempFlow) {
						tempFlow = flow;
						find_max_flow = i;
						if (tempFlow != -INF)	cout << "Max flow updated! Max flow: " << flow << endl;
					}
					continue;
				}
				flag = true;
				update(G, size, true);
				recover(G, min_flow, size.NN, size.NC);
				int fee = min_fee_bf(G, size, dist, prevv, preve, flow);
				cout << "Min fee found! Min fee: " << fee << endl;
				if (fee < tempCost) {
					tempCost = fee;
					find_min_cost = i;
					cout << "Min fee updated!" << endl;
				}
			}
			if (flag) {
				serverId.push_back(find_min_cost);
				unoccupied[find_min_cost] = -1;
			}
			else
			{
				cout << "Min fee Not found!" << endl;
				serverId.push_back(find_max_flow);
				unoccupied[find_max_flow] = -1;
			}
		}
		else {
			int server_min_cost = tempCost;
			for (int i = 0; i < size.NN; ++i) {
				if (unoccupied[i] == -1)	continue;
				t_state = serverId;
				t_state.push_back(i);
				adjustGraph(G, t_state, table, min_flow, size);
				backup(G, size);
				int fee = min_fee_bf(G, size, dist, prevv, preve, min_sum);
				if (fee < tempCost) {
					tempCost = fee;
					find_min_cost = i;
				}
			}
			if (tempCost < server_min_cost) {
				cout << "Best topology found!" << endl;
				cout << endl << "Server numbers: " << serverId.size() << endl;
				break;
			}
			else {
				serverId.push_back(find_min_cost);
			}
		}
		cout << "Server number: " << serverId.size() << endl;
		update(G, size, true);
		--N;
	}
	cout << "Min fee: " << tempCost << endl;
	delete unoccupied, used, dist, prevv, preve, prevv_inv;
}

void shift(vector<vector<edge> >& G, GSize& size, int pos, int& max_pos, int& second_pos) {
	int max= G[pos].size(), second=max,s=G[pos].size();
	if (G[pos][s - 1].to == size.NN + size.NC + 1) {
		max = s - 2; second = max;
	}
	else if (G[pos][s - 2].to == size.NN + size.NC + 1) {
		max = s - 3; second = max;
	}
	max_pos = pos;
	second_pos = pos;
	for (int i = 0; i < G[pos].size(); ++i) {
		edge& e = G[pos][i];
		if (e.to >= size.NN)	continue;
		int to_size = G[e.to].size();	
		if (G[e.to][to_size - 1].to == size.NN + size.NC + 1)
			to_size -= 1;
		else if(G[e.to][to_size - 2].to == size.NN + size.NC + 1)
			to_size -= 2;
		if (to_size>max || to_size>second ) {
			if (to_size > max) {
				max = to_size;
				max_pos = e.to;
			}
			else if (max >= second) {
				second = to_size;
				second_pos = e.to;
			}
		}
	}
}

//calculate the value of node at pos
int calc_value(vector<vector<edge> >& G, GSize& size, int pos){
	int s=G[pos].size();
	int value=0, cost=0, cnt=0;
	char near_consumer=0;
	if (G[pos][s - 1].to == size.NN + size.NC + 1){
		s -= 2;	near_consumer=1;
	}
	else if (G[pos][s - 2].to == size.NN + size.NC + 1){
		s -= 3; near_consumer=1;
	}
	for(int i=0;i<G[pos].size();++i){
		edge& e=G[pos][i];
		if(e.to>=size.NN+size.NC)	continue;
		if(e.cost<=0 || e.cost==INF)	continue;
		value+=e.cap;
		cost+=e.cost;
		++cnt;	//density
	}
	cost/=cnt;
	cnt-=cost;
	return cnt;
}

void sort_density(vector<vector<edge> >& G, vector<p>& mpair, GSize& size, int pos) {
	int max=G[pos].size(), s = max;
	if (G[pos][s - 1].to == size.NN + size.NC + 1) max = s - 2;
	else if (G[pos][s - 2].to == size.NN + size.NC + 1) max = s - 3;
	for (int i = 0; i < G[pos].size(); ++i) {
		edge& e = G[pos][i];
		if (e.to >= size.NN || e.cost<0)	continue;
		int to_size = G[e.to].size();
		if (G[e.to][to_size - 1].to == size.NN + size.NC + 1)
			to_size -= 2;
		else if (G[e.to][to_size - 2].to == size.NN + size.NC + 1)
			to_size -= 3;
		mpair.push_back(p{ e.to, to_size });
	}
	mpair.push_back(p{ pos, max });
	sort(mpair.begin(), mpair.end(), Comp);
}

void sort_value(vector<vector<edge> >& G, vector<p>& mpair, GSize& size, int pos) {
	int s=G[pos].size();
	for(int i=0;i<G[pos].size();++i){
		edge& e=G[pos][i];
		if(e.to >= size.NN || e.cost<0) continue;
		mpair.push_back(p{e.to, calc_value(G, size, e.to)});
	}
	mpair.push_back(p{pos, calc_value(G, size, pos)});
	sort(mpair.begin(), mpair.end(), Comp);
}


void execute(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& Gtable, vector<int>& min_flow, vector<int>& min_index, int min_sum, int serverCost, int& minfee){
	char* used = new char[G.size()];
	char* flow_used=new char[G.size()];
	int* dist = new int[G.size()];
	int* h = new int[G.size()];
	int* prevv = new int[G.size()];
	int* prevv_inv = new int[G.size()];
	int* preve = new int[G.size()];

	vector<int> serverId(size.NC);
	vector<int> backupId(size.NC);

	int init_fee=0;
	int init_last_fee=INF;
	int init_flow;
	int iter_outer = 60;
	int iter_inner = 60;
	int thres=20;
	int low_bound=SHIFT_LOW_BOUND;  //8
	if (size.NN >= 70 && size.NN < 100) {
		iter_outer = 30;
		iter_inner = 200;
		thres= 20;
		low_bound*=2;
	}
	else if(size.NN>=100 && size.NN<250){
		iter_outer = 24;
		iter_inner = 50;
		thres= 0;
		low_bound*=2;
	}
	else if (size.NN >= 200 && size.NN<400) {
		iter_outer = 5;
		iter_inner = 60;
		thres= 0;
		low_bound*=3;
	}
	else if(size.NN>=400){
		iter_outer=2;
		iter_inner=45;
		thres= 0;
		low_bound*=5;
	}
	for (int i = 0; i < size.NC; ++i) {
		serverId[i] = Gtable[G[i + size.NN][0].to];
		backupId[i] = serverId[i];
	}
	adjustGraph(G, serverId, table, min_flow, size);
	backup(G, size);
	init_flow = getMaxFlow(G, min_flow, size, flow_used);
	update(G, size, true);
	if (init_flow ==min_sum) {
		recover(G, min_flow, size.NN, size.NC);
		init_fee = min_fee(G, size, h, dist, prevv, preve, init_flow) + serverId.size()*serverCost;
		init_last_fee = init_fee;
		update(G, size, true);
	}
	int pos=0;
	int fee=0, last_fee = INF;
	fee = init_fee; last_fee = init_last_fee;
	for (int k = 0; k < iter_outer; ++k) {
		int iter = iter_inner;
		int cnt = 0;
		int flow = 0;
		bool satisfied = true;
		double scale= SHIFTP/ (double)iter;
		double disc_scale = DISCARD / (double)iter;
		double shift_p = START_SHIFT, discard_p=START_DISCARD;
		vector<vector<int> > history;
		for (int i = 0; i < size.NC; ++i) {
			serverId[i] = Gtable[G[i + size.NN][0].to];
			backupId[i] = serverId[i];
		}
		printf("iter %d\n", k);
		fee = init_fee; last_fee = init_last_fee;
		int pos=0;
		char up=1;
		history.push_back(serverId);
		while (iter)  {
			vector<int> id;
			vector<p> around;
			memset(used, 0, G.size()*sizeof(char));

			for (int i = 0; i < size.NC; ++i) {
				backupId[i] = serverId[i];
				
				double p = rand()*1. / RAND_MAX;
				if (p < discard_p)	serverId[i] = -1;//discard some nodes first
				if (serverId[i] == -1)	continue;
				p = rand()*1. / RAND_MAX;
				if (p > shift_p) {
					if (!used[table[serverId[i]]]) {
						id.push_back(serverId[i]);
						used[table[serverId[i]]] = 1;
					}
					else {
						serverId[i] = -1;
					}
					continue;
				}
				int r = serverId[i];
				sort_density(G, around, size, table[r]);
				
				for (int j = 0; j < around.size(); ++j) {
					int to = around[j].id;
					if (!used[to] && Gtable[to] != serverId[i]) {
						used[to] = 1;
						serverId[i] = Gtable[to];
						id.push_back(serverId[i]);
						break;
					}
					else if (j == around.size() - 1) {	// || j >= low_bound
						double disp=rand()*1./RAND_MAX;
						if(disp<discard_p)	serverId[i] = -1;
						break;
					}
				}
		
			}
			for(int i = 0; i<2;++i){
				int random_add=rand()%(size.NN);
				if(!used[random_add]){
					id.push_back(Gtable[random_add]);
					used[random_add]=1;
				}
			}
			adjustGraph(G, id, table, min_flow, size);
			backup(G, size);
			flow = getMaxFlow(G, min_flow, size, flow_used);
			update(G, size, true);
			if (flow < min_sum) {
				satisfied = false;
			}
			else {
				recover(G, min_flow, size.NN, size.NC);
				fee = min_fee(G, size, h, dist, prevv, preve, flow) + id.size()*serverCost;
				update(G, size, true);
				if(pos >= thres && pos<iter_inner-thres){
					if (fee <= last_fee) {
						last_fee = fee;
						satisfied = true;
						if (fee < minfee) {
							minfee = fee;
							min_index.erase(min_index.begin(), min_index.end());
							for (int i = 0; i < id.size(); ++i) {
								min_index.push_back(id[i]);
							}
						}
					}
					else satisfied = false;
				}
				else{
					satisfied = true;
					if (fee < minfee) {
						minfee = fee;
						min_index.erase(min_index.begin(), min_index.end());
						for (int i = 0; i < id.size(); ++i) {
							min_index.push_back(id[i]);
						}
					}
				}
			}
			if(satisfied)	history.push_back(id);
			else {
				int row=rand()%history.size();//rand()%(history.size());
				for (int i = 0; i < size.NC; ++i) {
					serverId[i] = -1;
				}	
				for (int i = 0; i < mmin(size.NC, history[row].size()); ++i) {
					serverId[i] = history[row][i];
				}
				satisfied = true;
			}
			--iter;
			if(shift_p<SHIFTP)
				shift_p += scale;
			if(discard_p<DISCARD)	
				discard_p+=disc_scale;
			++pos;	
			
		}
	}
	
}

void execute2(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& Gtable, vector<int>& min_flow, vector<int>& min_index, int min_sum, int serverCost, int& minfee){
	clock_t start=clock();

	char* used = new char[G.size()];
	char* flow_used=new char[G.size()];
	int* dist = new int[G.size()];
	int* h = new int[G.size()];
	int* prevv = new int[G.size()];
	int* prevv_inv = new int[G.size()];
	int* preve = new int[G.size()];

	vector<int> serverId(size.NC);
	vector<int> backupId(size.NC);

	int init_fee=0;
	int init_last_fee=INF;
	int init_flow;
	int thres=20;
	int low_bound=SHIFT_LOW_BOUND;  //8
	int k=0;
	if (size.NN >= 70 && size.NN < 100) {
		thres=20;
		low_bound*=2;
	}
	else if(size.NN>=100 && size.NN<250){
		thres=10;
		low_bound*=2;
	}
	else if (size.NN >= 200 && size.NN<400) {
		thres=10;
		low_bound*=3;
	}
	else if(size.NN>=400){
		thres=5;
		low_bound*=5;
	}
	for (int i = 0; i < size.NC; ++i) {
		serverId[i] = Gtable[G[i + size.NN][0].to];
		backupId[i] = serverId[i];
	}
	adjustGraph(G, serverId, table, min_flow, size);
	backup(G, size);
	init_flow = getMaxFlow(G, min_flow, size, flow_used);
	update(G, size, true);
	if (init_flow ==min_sum) {
		recover(G, min_flow, size.NN, size.NC);
		init_fee = min_fee(G, size, h, dist, prevv, preve, init_flow) + serverId.size()*serverCost;
		init_last_fee = init_fee;
		update(G, size, true);
	}
	int pos=0;
	int fee = init_fee, last_fee = init_last_fee;
	bool satisfied = true;
	double scale= SHIFTP/ (double)thres/2;
	double disc_scale = DISCARD / (double)thres/2;
	double shift_p = START_SHIFT, discard_p=START_DISCARD;
	vector<vector<int> > history;
	vector<int> lastId;
	int cnt=0;
	while(clock()-start<60*CLOCKS_PER_SEC){
		vector<int> id;

		vector<p> around;
		memset(used, 0, G.size()*sizeof(char));
		history.push_back(serverId);
		for (int i = 0; i < size.NC; ++i) {
			backupId[i] = serverId[i];
			
			double p = rand()*1. / RAND_MAX;
			if (p < discard_p)	serverId[i] = -1;//discard some nodes first
			if (serverId[i] == -1)	continue;
			p = rand()*1. / RAND_MAX;
			if (p > shift_p) {
				if (!used[table[serverId[i]]]) {
					id.push_back(serverId[i]);
					used[table[serverId[i]]] = 1;
				}
				else {
					serverId[i] = -1;
				}
				continue;
			}
			int r = serverId[i];
			sort_density(G, around, size, table[r]);

			for (int j = 0; j < around.size(); ++j) {
				int to = around[j].id;
				if (!used[to] && Gtable[to] != serverId[i]) {
					used[to] = 1;
					serverId[i] = Gtable[to];
					id.push_back(serverId[i]);
					break;
				}
				else if (j == around.size() - 1) {	// || j >= low_bound
					double disp=rand()*1./RAND_MAX;
					if(disp<discard_p)	serverId[i] = -1;
					break;
				}
			}
			/*
			vector<double> sections(around.size());
			int sum=0;
			for(int j=0;j<around.size();++j){
				sum+=around[j].size;
			}
			sections[0]=around[0].size*1.0/(double)sum;
			for(int j=1;j<sections.size();++j){
				sections[j] = around[j].size*1.0/(double)sum;
				sections[j]+=sections[j-1];
			}
			for (int j = 0; j < sections.size(); ++j) {
				double choose_p=rand()*1./RAND_MAX;
				int to=0;
				if(choose_p<sections[0])	to=0;
				else
					for(int k=1;k<sections.size();++k)
						if(choose_p>sections[k-1] && choose_p<sections[k]){
							to=k;	break;
						}
				if (!used[to] && Gtable[to] != serverId[i]) {
					used[to] = 1;
					serverId[i] = Gtable[to];
					id.push_back(serverId[i]);
					break;
				}
				else if (j == sections.size() - 1) {	// || j >= low_bound
					double disp=rand()*1./RAND_MAX;
					if(disp<discard_p)	serverId[i] = -1;
					break;
				}
			}*/
		}
		for(int i = 0; i<2;++i){
			int random_add=rand()%(size.NN);
			if(!used[random_add]){
				id.push_back(Gtable[random_add]);
				used[random_add]=1;
			}
		}
		adjustGraph(G, id, table, min_flow, size);
		backup(G, size);
		int flow = getMaxFlow(G, min_flow, size, flow_used);
		update(G, size, true);
		if (flow < min_sum) {
			satisfied = false;
		}
		else {
			recover(G, min_flow, size.NN, size.NC);
			fee = min_fee(G, size, h, dist, prevv, preve, flow) + id.size()*serverCost;
			update(G, size, true);
			cnt = 0;
			if(pos >= thres){
				if (fee <= last_fee) {
					last_fee = fee;
					satisfied = true;
					if (fee < minfee) {
						minfee = fee;
						min_index.erase(min_index.begin(), min_index.end());
						for (int i = 0; i < id.size(); ++i) {
							min_index.push_back(id[i]);
						}
					}
				}
				else satisfied = false;
			}
			else{
				satisfied = true;
				if (fee < minfee) {
					minfee = fee;
					min_index.erase(min_index.begin(), min_index.end());
					for (int i = 0; i < id.size(); ++i) {
						min_index.push_back(id[i]);
					}
				}
			}
		}
		if(shift_p<SHIFTP)
			shift_p += scale;
		if(discard_p<DISCARD)	
			discard_p+=disc_scale;
		
		if(!satisfied) {	
			int row=history.size()-1;
			for (int i = 0; i < size.NC; ++i) {
				serverId[i] = -1;
			}	
			for (int i = 0; i < mmin(size.NC, history[row].size()); ++i) {
				serverId[i] = history[row][i];
			}	
			//for(int i=0;i<size.NC;++i)
			//	serverId[i]=backupId[i];
			//satisfied = true;
		}
		char same=1;
		if(id.size()!=lastId.size())	same=0;
		else{
			for(int i=0;i<id.size();++i){
				if(id[i]!=lastId[i]){
					same=0;break;
				}	
			}
		}
		if(same)	++cnt;
		else 	cnt=0;
		if(cnt==3){
			cout<<"Local minimum"<<endl;
			int row=rand()%(history.size());
			for (int i = 0; i < size.NC; ++i) {
				serverId[i] = -1;
			}	
			for (int i = 0; i < size.NC; ++i) {
				serverId[i] = history[row][i];
			}
			cnt=0;
		}
		lastId=id;
		++k;
	}
	cout<<"Iteration finished. Iter number: "<<k<<endl;
}

void execute3(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& Gtable, vector<int>& min_flow, vector<int>& min_index, int min_sum, int serverCost, int& minfee){
	char* used = new char[G.size()];
	char* flow_used=new char[G.size()];
	int* dist = new int[G.size()];
	int* h = new int[G.size()];
	int* prevv = new int[G.size()];
	int* prevv_inv = new int[G.size()];
	int* preve = new int[G.size()];

	vector<int> serverId(size.NC);
	vector<int> backupId(size.NC);

	int init_fee=0;
	int init_last_fee=INF;
	int init_flow;
	int iter_outer = 60;
	int iter_inner = 60;
	int thres=20;
	int low_bound=SHIFT_LOW_BOUND;  //8
	if (size.NN >= 70 && size.NN < 100) {
		iter_outer = 30;
		iter_inner = 200;
		thres= 20;
		low_bound*=2;
	}
	else if(size.NN>=100 && size.NN<250){
		iter_outer = 20;
		iter_inner = 70;
		thres= 0;
		low_bound*=2;
	}
	else if (size.NN >= 200 && size.NN<400) {
		iter_outer = 5;
		iter_inner = 60;
		thres= 0;
		low_bound*=3;
	}
	else if(size.NN>=400){
		iter_outer=2;
		iter_inner=45;
		thres= 0;
		low_bound*=5;
	}
	for (int i = 0; i < size.NC; ++i) {
		serverId[i] = Gtable[G[i + size.NN][0].to];
		backupId[i] = serverId[i];
	}
	adjustGraph(G, serverId, table, min_flow, size);
	backup(G, size);
	init_flow = getMaxFlow(G, min_flow, size, flow_used);
	update(G, size, true);
	if (init_flow ==min_sum) {
		recover(G, min_flow, size.NN, size.NC);
		init_fee = min_fee(G, size, h, dist, prevv, preve, init_flow) + serverId.size()*serverCost;
		init_last_fee = init_fee;
		update(G, size, true);
	}
	int pos=0;
	int fee=0, last_fee = INF;
	fee = init_fee; last_fee = init_last_fee;
	for (int k = 0; k < iter_outer; ++k) {
		int iter = iter_inner;
		int cnt = 0;
		int flow = 0;
		bool satisfied = true;
		double scale= SHIFTP/ (double)iter;
		double disc_scale = DISCARD / (double)iter;
		double shift_p = SHIFTP, discard_p = DISCARD;
		vector<vector<int> > history;
		memset(used, 0, G.size()*sizeof(char));
		for (int i = 0; i < size.NC; ++i) {
			serverId[i] = Gtable[G[i + size.NN][0].to];
			backupId[i] = serverId[i];
			used[G[i + size.NN][0].to]=1;
		}
		printf("iter %d\n", k);
		fee = init_fee; last_fee = init_last_fee;
		int pos=0;
		char up=1;
		history.push_back(serverId);
		
		while (iter)  {
			vector<int> id;
			vector<p> around;
			for (int i = 0; i < size.NC; ++i) {
				backupId[i] = serverId[i];
				
				double p = rand()*1. / RAND_MAX;
				//if (p < discard_p){
				//	serverId[i] = -1;//discard some nodes first
				//	used[table[serverId[i]]]=0;
				//}
				if (serverId[i] == -1)	continue;
				p = rand()*1. / RAND_MAX;
				if (p > shift_p) {
					id.push_back(serverId[i]);
					continue;
				}
				int r = serverId[i];
				sort_density(G, around, size, table[r]);
				
				for (int j = 0; j < around.size(); ++j) {
					int to = around[j].id;
					if (!used[to]) {  // && Gtable[to] != serverId[i]
						used[to] = 1;
						serverId[i] = Gtable[to];
						id.push_back(serverId[i]);
						break;
					}
					else if (j == around.size() - 1 || j >= low_bound) {	// 
						double disp=rand()*1./RAND_MAX;
						if(disp<discard_p){
							serverId[i] = -1;
							used[table[serverId[i]]]=0;
						}
						break;
					}
				}
			}
			for(int i = 0; i<2;++i){
				int random_add=rand()%(size.NN);
				if(!used[random_add]){
					id.push_back(Gtable[random_add]);
					used[random_add]=1;
				}
			}
			adjustGraph(G, id, table, min_flow, size);
			backup(G, size);
			flow = getMaxFlow(G, min_flow, size, flow_used);
			update(G, size, true);
			if (flow < min_sum) {
				satisfied = false;
			}
			else {
				recover(G, min_flow, size.NN, size.NC);
				fee = min_fee(G, size, h, dist, prevv, preve, flow) + id.size()*serverCost;
				update(G, size, true);
				if(pos >= thres && pos<iter_inner-thres){  //
					if (fee <= last_fee) {
						last_fee = fee;
						satisfied = true;
						if (fee < minfee) {
							minfee = fee;
							min_index.erase(min_index.begin(), min_index.end());
							for (int i = 0; i < id.size(); ++i) {
								min_index.push_back(id[i]);
							}
						}
					}
					else satisfied = false;
				}
				else{
					satisfied = true;
					if (fee < minfee) {
						minfee = fee;
						min_index.erase(min_index.begin(), min_index.end());
						for (int i = 0; i < id.size(); ++i) {
							min_index.push_back(id[i]);
						}
					}
				}
			}
			if(satisfied)	history.push_back(id);
			else {
				int s=history.size();
				int n=3;
				int row=s-1;
				if(s>=n)  row=s-n+rand()%n;//rand()%(history.size());
				for (int i = 0; i < size.NC; ++i) {
					serverId[i] = -1;
				}	
				for (int i = 0; i < mmin(size.NC, history[row].size()); ++i) {
					serverId[i] = history[row][i];
				}
				satisfied = true;
			}
			--iter;
			if(shift_p>0)	shift_p -= scale;
			if(discard_p>0)	discard_p -= disc_scale;
			++pos;	
			
		}
	}
	

}
