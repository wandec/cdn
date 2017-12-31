#include "graph.h"

int mmin(int x, int y) {
	if (x < y)	return x;
	return y;
}

int buildGraph(vector<vector<edge> >& G, vector<int>& low, vector<int>& table, vector<int>& Gtable, int& serverCost, int& NN, int& NC, char* topo[MAX_EDGE_NUM]) {
	int num_Netnode, num_edges, num_Cusnode, index = 0;
	int k=-1,start=0;
	stringstream ss;
	ss<<topo[++k];
	ss >> num_Netnode >> num_edges >> num_Cusnode;
	++k;
	while(topo[k][0]=='\n' || topo[k][0]=='\r'){
		++k;
	}
	ss.str(""); ss<<topo[k];	
 	ss>> serverCost;
	if (num_Netnode <= 0 || num_Cusnode <= 0)
		return -1;
	NN = num_Netnode;
	NC = num_Cusnode;
	
	cout<<"NN: "<<NN<<", NC: "<<NC<<", edge: "<<num_edges<<", cost: "<<serverCost<<endl;

	if (G.size() == 0) {
		G.resize(num_Netnode * 2 + num_Cusnode + 2);
		table.resize(NN + NC + 2 + NN);
		Gtable.resize(NN + NC + 2 + NN);
		for (int i = 0; i < NN + NC; ++i)	table[i] = -1;
		low.resize(num_Cusnode);
		for (int j = NN; j<NN + NC; ++j) {	//Customers
			G[j].resize(3);
		}
		G[NN + NC].resize(NN + NC);
		G[NN + NC + 1].resize(2 * NC);	//hyper terminal
		for (int j = NN + NC + 2; j<NN + NC + 2 + NN; ++j) {	//Server
			G[j].resize(2);
		}
	}
	char ch = 0;
	++k;
	while(topo[k][0]=='\n' || topo[k][0]=='\r'){
		++k;
	}
	start=k;
	for(int i=0;i<num_edges;++i) {
		int from, to, cap, cost;
		ss.str(""); ss<<topo[i+start];
		ss >> from >> to >> cap >> cost;
		if (table[from] < 0) {
			table[from] = index++;
			Gtable[index - 1] = from;
		}
		if (table[to] < 0) {
			table[to] = index++;
			Gtable[index - 1] = to;
		}
	}
	for (int i = NN + NC; i < NN + NC + 2 + NN; ++i) {
		Gtable[i] = i;
		table[i] = i;
	}
	k+=num_edges;
	while(topo[k][0]=='\n' || topo[k][0]=='\r'){
		++k;
	}
	for (int i = 0; i<NC; ++i) {		//network customer nodes connection
		int _id, netid, flow;
		ss.str(""); ss<<topo[i+k];
		ss >> _id >> netid >> flow;
		_id += NN;
		if (table[_id]<0) table[_id] = index++;
		Gtable[index - 1] = _id;
	}
	//start building
	k=start;
	for(int i=0;i<num_edges;++i){
		int from, to, cap, cost;
		ss.str(""); ss<<topo[i+start];
		ss >> from >> to >> cap >> cost;
		from = table[from]; to = table[to];
		if (DAG) {
			G[from].push_back(edge{ to, cap, cost, 0, (int)G[to].size(),0 });		//undirected graph
			G[to].push_back(edge{ from, 0, -cost, 0, (int)(G[from].size() - 1),0 });
			//G[to].push_back(edge{ from, 0, cost, 0, (int)(G[from].size() - 1),0 });
		}
		else {
			G[from].push_back(edge{ to, cap, cost, 0, (int)G[to].size(),0 });
			G[to].push_back(edge{ from, 0, -cost, 0, (int)(G[from].size() - 1),0 });
			G[to].push_back(edge{ from, cap, cost, 0, (int)G[from].size(),0 });
			G[from].push_back(edge{ to, 0, -cost, 0, (int)(G[to].size() - 1),0 });
		}
		//else fs.putback(ch);
	}
	k+=num_edges;
	while(topo[k][0]=='\n' || topo[k][0]=='\r'){	
		++k;
	}
	for (int i = 0; i<NC; ++i) {		//network customer nodes connection
		int _id, netid, flow;
		ss.str(""); ss<<topo[i+k];
		ss >> _id >> netid >> flow;
		_id += NN;
		_id = table[_id]; netid = table[netid];
		int hyper_T = NN + NC + 1;
		G[netid].push_back(edge{ _id, 0, 0, 0,0 });
		G[_id][0] = edge{ netid, 0, 0, 0, (int)(G[netid].size() - 1),0 };
		G[_id][1] = edge{ hyper_T, INF, 0,0, i,0 };
		G[hyper_T][i] = edge{ _id, 0, 0, 0,1,0 };
		
		G[netid].push_back(edge{ hyper_T, flow, INF, 0,i + NC,0 });
		G[hyper_T][i + NC] = edge{ netid,0,-INF,0,(int)(G[netid].size() - 1),0 };
		low[i] = flow;
	}
	int sum = 0;
	for (int i = 0; i < low.size(); ++i)	sum += low[i];
	return sum;
}

void adjustGraph(vector<vector<edge> >& G, vector<int>& serverId, vector<int>& table, vector<int>& low, GSize& size) {
	int NN = size.NN, NC = size.NC;
	int base = NN + NC + 2;
	if (size.NS > 0) {
		for (int i = 0; i < NN; ++i) {
			edge& e = G[i][G[i].size() - 1];
			if (e.to >= NN + NC + 2)
				G[i].erase(G[i].begin() + G[i].size() - 1, G[i].end());
		}
	}
	for (int i = 0; i<serverId.size(); ++i) {		//network server connection
		int to = table[serverId[i]];		//serverId: id of netnode where server connects during each iteration 
		G[base + i][0] = edge{ to, INF, 0, 0, (int)G[to].size(),0 };
		G[to].push_back(edge{ base + i, 0, 0, 0, 0,0 });
	}
	size.NS = serverId.size();
	int hyper_S = NN + NC;
	for (int i = 0; i<NC; ++i) {
		G[i + NN][2] = edge{ hyper_S, 0, -INF, 0, i,0 };
		G[hyper_S][i] = edge{ NN + i, low[i], INF, 0, 2,0 };
	}
	for (int i = 0; i<serverId.size(); ++i) {
		G[base + i][1] = edge{ hyper_S, 0, 0, 0, i + NC,0 };
		G[hyper_S][i + NC] = edge{ base + i, INF, 0, 0, 1,0 };
	}
}

void printGraph(vector<vector<edge> >& G, vector<int>& Gtable, int NS, int NN, int NC) {
	cout << "Network node:" << endl;
	for (int v = 0; v<NN; ++v) {
		cout << "Node " << Gtable[v] << ":{";
		for (int j = 0; j<G[v].size(); ++j) {
			edge& e = G[v][j];
			cout << '{' << Gtable[e.to] << ',' << e.cap << ',' << e.cost << ',' << e.flow << "}";
			if (j<G[v].size() - 1)	cout << ',';
		}
		cout << "}" << endl;
	}
	cout << "============" << endl;
	cout << "Customer node:" << endl;
	for (int v = 0; v<NC; ++v) {
		cout << "Node " << Gtable[v + NN] << ":{";
		for (int j = 0; j<3; ++j) {
			edge& e = G[v + NN][j];
			cout << '{' << Gtable[e.to] << ',' << e.cap << ',' << e.cost << ',' << e.flow << "}";
			if (j<2)	cout << ',';
		}
		cout << "}" << endl;
	}
	cout << "============" << endl;
	cout << "Server node:" << endl;
	int base = NN + NC + 2;
	for (int v = 0; v<NS; ++v) {
		cout << "Node " << Gtable[v + base] << ":{";
		for (int j = 0; j<2; ++j) {
			edge& e = G[v + base][j];
			cout << '{' << Gtable[e.to] << ',' << e.cap << ',' << e.cost << ',' << e.flow << "}";
			if (j<1)	cout << ',';
		}
		cout << "}" << endl;
	}
	cout << "============" << endl;
	cout << "Hyper server node:" << endl;
	cout << "Node " << NN + NC << ":{";
	int hyper = NN + NC;
	for (int j = 0; j<NS + NC; ++j) {
		edge& e = G[hyper][j];
		cout << '{' << Gtable[e.to] << ',' << e.cap << ',' << e.cost << ',' << e.flow << "}";
		if (j<NS - 1)	cout << ',';
	}
	cout << '}' << endl;
	cout << "============" << endl;
	cout << "Hyper terminal node:" << endl;
	cout << "Node " << NN + NC + 1 << ":{";
	hyper = NN + NC + 1;
	for (int j = 0; j<2 * NC; ++j) {
		edge& e = G[hyper][j];
		cout << '{' << Gtable[e.to] << ',' << e.cap << ',' << e.cost << ',' << e.flow << "}";
		if (j<NC - 1)	cout << ',';
	}
	cout << '}' << endl;
	cout << "============" << endl;
}

void recover(vector<vector<edge> >& G, vector<int>& low, int NN, int NC) {
	for (int v = 0; v < NC; ++v) {
		edge& e = G[NN + v][0];
		edge& from_e = G[NN + v][2];
		G[e.to][e.rev].cap = low[v];
		//G[e.to][e.rev].cap_backup = low[v];
		G[e.to][e.rev].flow = G[from_e.to][from_e.rev].flow;
		e.flow = -G[e.to][e.rev].flow;
	}
}

void backup(vector<vector<edge> >& G, GSize& size) {
	//recover(G, low, NN, NC);
	int V = size.NN + size.NC + size.NS + 2;
	for (int v = 0; v < V; ++v) {
		int up = 0;
		CHECK(G, size, v, up);
		for (int i = 0; i < up; ++i) {
			edge& e = G[v][i];
			e.cap_backup = e.cap;
		}
	}
}

void update(vector<vector<edge> >& G, GSize& size, bool bFlow) {
	int V = size.NN + size.NC + size.NS + 2;
	for (int v = 0; v < V; ++v) {
		int up = 0;
		CHECK(G, size, v, up);
		for (int i = 0; i < up; ++i) {
			edge& e = G[v][i];
			e.cap = e.cap_backup;
			if (e.cap < 0)	cout << '-';
			if (bFlow)	e.flow = 0;
		}
	}
}

int min_fee(vector<vector<edge> >& G, GSize& size,int* h, int* dist, int* prevv, int* preve, int F) {
	int V = G.size();
	int res = 0, up = 0;
	int s = size.NN + size.NC, t = s + 1;
	for (int i = 0; i < V; ++i)	h[i] = 0;
	while (F > 0) {
		priority_queue<P, vector<P>, greater<P> > que;
		for (int i = 0; i < V; ++i)	dist[i] = INF;
		dist[s] = 0;
		que.push(P(0, s));
		while (!que.empty()) {
			P p = que.top(); que.pop();
			int v = p.second;
			if (dist[v] < p.first)	continue;
			CHECK(G, size, v, up);
			for (int i = 0; i < up; ++i) {
				edge& e = G[v][i];
				if (e.cap > 0 && dist[e.to]>dist[v] + e.cost + h[v] - h[e.to]) {
					if (e.cost == INF)	continue;
					dist[e.to] = dist[v] + e.cost + h[v] - h[e.to];
					prevv[e.to] = v;
					preve[e.to] = i;
					que.push(P(dist[e.to], e.to));
				}
			}
		}
		if (dist[t] == INF) {
			return -1;
		}
		for (int v = 0; v < V; ++v)	h[v] += dist[v];
		int d = F;
		for (int v = t; v != s; v = prevv[v]) {
			d = mmin(d, G[prevv[v]][preve[v]].cap);
		}
		F -= d;
		res += d*h[t];
		for (int v = t; v != s; v = prevv[v]) {
			edge& e = G[prevv[v]][preve[v]];
			if(e.cap!=INF)	e.cap -= d;
			e.flow += d;
			if(G[e.to][e.rev].cap!=INF)	G[e.to][e.rev].cap += d;
			G[e.to][e.rev].flow -= d;
		}
	}
	return res;
}

int min_fee_bf(vector<vector<edge> >& G, GSize& size, \
	int* dist, int* prevv, int* preve, int F) {

	int V = size.NN + size.NC + size.NS + 2;
	int res = 0;
	int up = 0;
	int s = size.NN + size.NC, t = s + 1;
	while (F>0) {
		for (int i = 0; i < V; ++i)	dist[i] = INF;
		dist[s] = 0;
		bool update = true;
		while (update) {
			update = false;
			for (int v = 0; v < V; ++v) {
				if (dist[v] == INF)	continue;	//source	
				CHECK(G, size, v, up);
				for (int i = 0; i < up; ++i) {
					edge& e = G[v][i];
					if (e.cost == INF)	continue;
					if (e.cap > 0 && dist[e.to]>dist[v] + e.cost) {
						dist[e.to] = dist[v] + e.cost;
						prevv[e.to] = v;
						preve[e.to] = i;
						update = true;
					}
				}
			}
		}
		if (dist[t] == INF) {
			return -1;
		}
		int d = F;
		for (int v = t; v != s; v = prevv[v]) {
			d = mmin(d, G[prevv[v]][preve[v]].cap);
		}
		F -= d;
		res += d*dist[t];
		for (int v = t; v != s; v = prevv[v]) {
			edge& e = G[prevv[v]][preve[v]];
			if(e.cap!=INF)	e.cap -= d;
			e.flow += d;
			if(G[e.to][e.rev].cap!=INF)	G[e.to][e.rev].cap += d;
			G[e.to][e.rev].flow -= d;
		}
	}
	return res;
}

void getPath(vector<vector<edge> >& G, GSize& size, char* used, int* prevv, int* preve, int* prevv_inv, int s, int t, int& flow) {
	queue<int> q;
	q.push(s);
	memset(used, 0, G.size()*sizeof(char));
	int up = 0;
	bool found = false;
	while (!q.empty() && !found) {// 
								  //update = false;
		int v = q.front(); q.pop();
		used[v] = 1;
		CHECK(G, size, v, up);
		for (int i = 0; i < up; ++i) {
			edge& e = G[v][i];
			if (e.flow > 0 && !used[e.to]) {	//G[e.to][e.rev].flow
				q.push(e.to);
				prevv[e.to] = v;
				preve[e.to] = i;
				if (e.to == t) {
					found = true; break;
				}
			}
		}
	}
	if (q.empty()) {
		flow = -1; return;
	}
	for (int v = t; v != s; v = prevv[v]) {
		prevv_inv[prevv[v]] = v;
		if (G[prevv[v]][preve[v]].flow < flow)
			flow = G[prevv[v]][preve[v]].flow;
	}
}

void findPath(vector<vector<edge> >& G, vector<int>& Gtable, GSize& size, vector<string>& pth, \
	char* used, int* prevv, int* preve, int* prevv_inv, int s, int t) {

	int up = 0;
	bool flag = true;
	while (flag) {
		stringstream ss;
		flag = false;
		bool update = true;
		bool found = false;
		queue<int> q;
		q.push(s);
		memset(used, 0, G.size()*sizeof(char));
		while (!q.empty() && !found) {  //
			int v = q.front(); q.pop();
			used[v] = 1;
			CHECK(G, size, v, up);
			for (int i = 0; i < up; ++i) {
				edge& e = G[v][i];
				if (e.flow > 0 && !used[e.to]) {	//G[e.to][e.rev].flow
					q.push(e.to);
					prevv[e.to] = v;
					preve[e.to] = i;
					if (e.to == t) {
						found = true; break;
					}
				}
			}
		}
		if (q.empty()) {
			break;
		}
		int d = INF;
		for (int v = t; v != s; v = prevv[v]) {
			d = mmin(d, G[prevv[v]][preve[v]].flow);
			prevv_inv[prevv[v]] = v;
		}
		for (int v = t; v != s; v = prevv[v]) {
			edge& e = G[prevv[v]][preve[v]];
			e.flow -= d;
			G[e.to][e.rev].flow += d;
		}
		for (int v = s; v != t; v = prevv_inv[v]) {
			ss << Gtable[v] << ' ';
		}
		ss << Gtable[t] - size.NN << " " << d;
		pth.push_back(ss.str());

		flag = true;
	}
}

int max_flow(vector<vector<edge> >& G, char* used, GSize& size, int s, int t, int f) {
	if (s == t)	return f;
	used[s] = 1;
	int up = 0;
	CHECK(G, size, s, up);
	for (int i = 0; i<up; ++i) {
		edge& e = G[s][i];
		if (e.cap>0 && !used[e.to]) {
			int d = max_flow(G, used, size, e.to, t, mmin(f, e.cap));
			if (d > 0) {
				if (e.cap != INF)
					e.cap -= d;
				e.flow += d;
				if(G[e.to][e.rev].cap!=INF)	G[e.to][e.rev].cap += d;
				G[e.to][e.rev].flow -= d;	
				return d;
			}
		}
	}
	return 0;
}

int getMaxFlow(vector<vector<edge> >& G, vector<int>& low, GSize& size, char* used) {
	int iter = 1000000;
	int flow = 0;
	while (iter) {
		memset(used, 0, sizeof(char)*G.size());
		int f = max_flow(G, used, size, size.NN + size.NC, size.NN + size.NC + 1, INF);
		if (f == 0)	break;
		flow += f;
		--iter;
	}
	if (iter == 0) {
		printf("Error...\n");
		return -1;
	}
	for (int i = 0; i < low.size(); ++i)	flow -= low[i];
	return flow;
}

int max_flow_stack(vector<vector<edge> >& G, vector<int>& low, GSize& size, char* used, int* _stack, int* prevv, int* preve) {
	int flow = 0;
	memset(used, 0, sizeof(used));
	int s = size.NN + size.NC, t = s + 1;
	while (1) {
		int top = -1;
		_stack[++top] = s;
		int min_cap = INF;
		bool pushed = false;
		bool found = false;
		memset(used, 0, G.size()*sizeof(char));
		int up = 0;
		while (top >= 0) {
			int v = _stack[top];
			used[v] = 1;
			pushed = false;
			found = false;
			CHECK(G, size, v, up);
			for (int i = 0; i<up; ++i) {
				edge& e = G[v][i];
				if (e.cap>0 && !used[e.to]) {
					pushed = true;
					_stack[++top] = e.to;
					prevv[e.to] = v;
					preve[e.to] = i;
					if (e.cap<min_cap)	min_cap = e.cap;
					if (e.to == t)	found = true;
					break;
				}
			}
			if (!pushed) {
				--top;
			}
			if (found) {
				for (int v = t; v != s; v = prevv[v]) {
					edge& e = G[prevv[v]][preve[v]];
					if (e.cap != INF)	e.cap -= min_cap;	
					if (e.rev >= G[e.to].size()) {
						cout << "E rev corrupted"<<endl;
						cout <<prevv[v]<< "," << e.to << "," << G[e.to].size() <<","<<e.rev<<endl;
					}
					if(G[e.to][e.rev].cap!=INF)	G[e.to][e.rev].cap += min_cap;
				}
				flow += min_cap;
				break;
			}
		}
		if (!found)	
			break;
	}
	for (int i = 0; i < low.size(); ++i) {
		flow -= low[i];
	}
	return flow;
}

void savePath(const char* fileName, vector<string>& path) {
	ofstream fs;
	fs.open(fileName, ios::out);
	fs << path.size() << "\n";
	fs <<"\n";
	for (int i = 0; i < path.size(); ++i) {
		fs << path[i] << "\n";
	}
	fs.close();
}
