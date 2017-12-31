#pragma once
#include "graph.h"
#define POPN	128
#define K		1000
#define MUTATION	0.09
#define ITER    1000

class chromosome {
public:
	int length;
	int cost;
	double rfit;
	double cfit;
	char* gene;
public:
	chromosome();
	~chromosome();
	chromosome(const chromosome& c);
	void destroy();
	chromosome& operator=(const chromosome& c);
};

double value(int fee);

void gene2Id(char* gene, vector<int>& id, int len) ;

bool cmp(const chromosome& c1, const chromosome& c2);

void calc_fit(vector<vector<edge> >& G, chromosome& c, GSize& size, vector<int>& table, vector<int>& min_flow, char* used, int* h, int* _stack, int* dist, int* prevv, int* preve, int min_sum, int serverCost);

bool updateFit(vector<vector<edge> >& G, vector<chromosome>&  population, GSize& size, vector<int>& table, vector<int>& min_flow,char* used, int* h, int* _stack,  int* dist, int* prevv, int* preve, int min_sum, int serverCost);

void init(vector<chromosome>& population, GSize& size);

void reInit(vector<chromosome>& population, GSize& size);

void pick_chrom_prob(vector<chromosome>& population);

void pick_chrom_sort(vector<chromosome>& population);

void crossover(vector<chromosome>& population) ;

void mutation(vector<chromosome>& population);

void evolution(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& min_flow, int min_sum, int serverCost, int& minfee);
