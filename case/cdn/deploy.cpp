#include "deploy.h"

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename)
{
	vector<vector<edge> > G;
	vector<int> table;
	vector<int> Gtable;
	vector<int> min_flow;
	vector<int> min_index;
	vector<string> path;

	GSize size;
	int serverCost;
	int minfee = INF;
	int NN, NC, NS = 0;
	int min_sum = buildGraph(G, min_flow, table, Gtable, serverCost, NN, NC, topo);
	size.NN = NN;
	size.NC = NC;
	size.NS = NS;
    char* used = new char[G.size()];
	int* dist = new int[G.size()];
	int* h = new int[G.size()];
	int* prevv = new int[G.size()];
	int* prevv_inv = new int[G.size()];
	int* preve = new int[G.size()];

	min_index.resize(NC);

	execute3(G, size, table, Gtable, min_flow, min_index, min_sum, serverCost, minfee);

	adjustGraph(G, min_index, table, min_flow, size);
	recover(G, min_flow, NN, NC);
	min_fee(G, size, h, dist, prevv, preve, min_sum);
	for (int i = 0; i < min_index.size(); ++i) {
		for (int j = 0; j < NC; ++j) {
			findPath(G, Gtable, size, path, used, prevv, preve, prevv_inv, table[min_index[i]], j + NN);
		}
	}
	char lines[20];
	sprintf(lines,"%d",path.size());
	string res=string(lines);
	res+="\n\n";
	for(int i=0;i<path.size();++i){
		res+=path[i];
		res+="\n";
	}
	//cout<<ss.str();
	printf("\nFinal min fee: %d, server numbers: %d, path numbers: %d\n", minfee, min_index.size(), path.size());
	delete used;
	delete dist;
	delete h;
	delete prevv;
	delete preve;
	delete prevv_inv;
	char * topo_file = (char *)res.c_str();
    write_result(topo_file, filename);
}
