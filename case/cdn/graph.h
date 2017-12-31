#pragma once
#ifndef _GRAPH_H_
#define _GRAPH_H_
#define INF		0x7fffffff 
#define UNIX
#define DAG		false
#include "lib_io.h"
#include <fstream>
#include <vector>
#include <queue>
#include <cstring>
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

#define CHECK(G,s,v,up)	if (v<s.NN)	up = G[v].size();\
	else if (v >= s.NN && v < s.NN + s.NC)	up = 3;\
	else if (v == s.NN + s.NC)	up = s.NC + s.NS;\
	else if (v == s.NN + s.NC + 1)	up = 2 * s.NC;\
	else up = 2;

using namespace std;

struct edge {
	int to;
	int cap;
	int cost;
	int flow;
	int rev;
	int cap_backup;
};

struct GSize {
	int NN;
	int NC;
	int NS;
};

typedef pair<int, int> P;

int mmin(int x, int y);

int buildGraph(vector<vector<edge> >& G, vector<int>& low, vector<int>& table, vector<int>& Gtable, int& serverCost, int& NN, int& NC, char* topo[MAX_EDGE_NUM]);

void adjustGraph(vector<vector<edge> >& G, vector<int>& serverId, vector<int>& table, vector<int>& low, GSize& size);

void printGraph(vector<vector<edge> >& G, vector<int>& Gtable, int NS, int NN, int NC);

void recover(vector<vector<edge> >& G, vector<int>& low, int NN, int NC) ;

void backup(vector<vector<edge> >& G, GSize& size);

void update(vector<vector<edge> >& G, GSize& size, bool bFlow);

int min_fee(vector<vector<edge> >& G, GSize& size,int* h, int* dist, int* prevv, int* preve, int F);

int min_fee_bf(vector<vector<edge> >& G, GSize& size, int* dist, int* prevv, int* preve, int F);

void getPath(vector<vector<edge> >& G, GSize& size, char* used, int* prevv, int* preve, int* prevv_inv, int s, int t, int& flow);

void findPath(vector<vector<edge> >& G, vector<int>& Gtable, GSize& size, vector<string>& pth, char* used, int* prevv, int* preve, int* prevv_inv, int s, int t);

int max_flow(vector<vector<edge> >& G, char* used, GSize& size, int s, int t, int f) ;

int getMaxFlow(vector<vector<edge> >& G, vector<int>& low, GSize& size, char* used) ;

int max_flow_stack(vector<vector<edge> >& G, vector<int>& low, GSize& size, char* used, int* _stack, int* prevv, int* preve);

void savePath(const char* fileName, vector<string>& path);

#endif
