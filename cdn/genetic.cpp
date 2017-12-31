#include "genetic.h"

chromosome::chromosome(){
	gene=NULL;
}
chromosome::~chromosome(){
	destroy();
}
chromosome::chromosome(const chromosome& c){
	gene = NULL;
	length = c.length;
	rfit = c.rfit;
	cfit = c.cfit;
	cost = c.cost;
	if (!gene)	
		gene = new char[length];
	for (int i = 0; i < length; ++i)
		gene[i] = c.gene[i];
}
void chromosome::destroy(){
	if (gene) {
		delete gene;
		gene = NULL;
	}
}

chromosome& chromosome::operator=(const chromosome& c){
	this->length = c.length;
	this->rfit = c.rfit;
	this->cfit = c.cfit;
	this->cost = c.cost;
	if(!this->gene)	this->gene = new char[length];
	for (int i = 0; i < length; ++i)
		this->gene[i] = c.gene[i];
	return *this;
}

double value(int fee) {
	return (double)K / (double)fee;
}

void gene2Id(char* gene, vector<int>& id, int len) {
	for (int i = 0; i < len; ++i) {
		if (gene[i]) {
			id.push_back(i);
		}
	}
}

bool cmp(const chromosome& c1, const chromosome& c2) {
	return c1.cost < c2.cost;
}

void calc_fit(vector<vector<edge> >& G, chromosome& c, GSize& size, \
	vector<int>& table, vector<int>& min_flow, char* used, int* h, int* _stack, int* dist, int* prevv, int* preve, int min_sum, int serverCost) {

	vector<int> serverId;
	gene2Id(c.gene, serverId, c.length);
	if (serverId.size() == 0) {
		c.cost = INF;
		c.rfit = 0;
		return;
	}
	adjustGraph(G, serverId, table, min_flow, size);
	backup(G, size);
	//int flow = max_flow_stack(G, min_flow, size, used, _stack, prevv, preve);
	int flow = getMaxFlow(G, min_flow, size, used);
	update(G, size, true);
	if (flow < min_sum) {
		//cout << "Cannot satisfy" << endl;
		c.cost = INF;
		c.rfit = 0;
	}
	else {
		//cout << "Satisfy" << endl;
		recover(G, min_flow, size.NN, size.NC);
		int fee = min_fee(G, size, h, dist, prevv, preve, flow) + serverId.size()*serverCost;
		c.cost = fee;
		c.rfit = value(fee);
		update(G, size, true);
	}
}


bool updateFit(vector<vector<edge> >& G, vector<chromosome>&  population, GSize& size, \
	vector<int>& table, vector<int>& min_flow,char* used, int* h, int* _stack,  int* dist, int* prevv, int* preve, int min_sum, int serverCost) {
	double sum = 0.0;
	for (int i = 0; i < population.size(); ++i) {
		calc_fit(G, population[i], size, table, min_flow,used,h,_stack ,dist, prevv, preve, min_sum, serverCost);
		sum += population[i].rfit;
	}
	for (int i = 0; i < population.size(); ++i) {
		population[i].rfit /= sum;
		population[i].cfit = 0;
	}
	if (sum < 1e-5)	return false;
	return true;
}

void init(vector<chromosome>& population, GSize& size) {
	double random;
	double sum = 0.0;
	chromosome c;
	c.length = size.NN;
	c.gene = new char[c.length];
	c.cfit = 0; c.rfit = 0; c.cost = 0;
	double p = (double)size.NC/(double)size.NN;
	int cnt = 0;
	for (int i = 0; i < POPN; ++i) {
		cnt = 0;
		for (int j = 0; j < size.NN; ++j)	
			c.gene[j] = 0;
		for (int j = 0; j <size. NN; ++j) {
			random = rand()*1./RAND_MAX;
			if (random < p) {
				c.gene[j] = 1;
				cnt++;
				if (cnt == size.NC)	break;
			}
		}
		population.push_back(c);
	}
}

void reInit(vector<chromosome>& population, GSize& size) {
	double random;
	double sum = 0.0;
	chromosome c;
	c.length = size.NN;
	c.gene = new char[c.length];
	c.cfit = 0; c.rfit = 0; c.cost = 0;
	double p = (double)size.NC/(double)size.NN;
	int cnt = 0;
	for (int i = 0; i < POPN; ++i) {
		cnt = 0;
		for (int j = 0; j <size.NN; ++j)
			c.gene[j] = 0;
		for (int j = 0; j <size.NN; ++j) {
			random = rand()*1. / RAND_MAX;
			if (random < p) {
				c.gene[j] = 1;	
				cnt++;
				if (cnt == size.NC)	break;
			}
		}
		population[i] = c;
	}
}

void pick_chrom_prob(vector<chromosome>& population) {
	population[0].cfit = population[0].rfit;
	for (int i = 1; i < population.size(); ++i) {
		population[i].cfit = population[i - 1].cfit + population[i].rfit;
	}
	vector<chromosome> temp(population.size());
	for (int i = 0; i < temp.size(); ++i) {
		double r = rand()*1./RAND_MAX;
		if (r < population[0].cfit)	temp[i] = population[0];
		else {
			for (int j = 1; j < population.size(); ++j) {
				if (r > population[j - 1].cfit && r < population[j].cfit) {
					temp[i] = population[j];
				}
			}
		}
	}
	for (int i = 0; i < temp.size(); ++i)
		population[i] = temp[i];
}

void pick_chrom_sort(vector<chromosome>& population) {
	sort(population.begin(), population.end(), cmp);
	int size=population.size(), start = population.size() / 2;
	for (int i = start,k=1; i < population.size(); ++i,++k) {
		population[size-k] = population[i - start];
	}
}

void crossover(vector<chromosome>& population) {
	int size = population.size();
	int len = population[0].length;
	for (int i = 0; i < size; ++i) {
		int spouse=rand()%size;
		int pos=rand()%len;
		char tmp;
		if(pos<=2)	pos+=2;
		if(pos>=len-3)	pos-=2;
		for (int k = pos; k < len; ++k) {
			tmp=population[i].gene[k];
			population[i].gene[k] = population[spouse].gene[k];
			population[spouse].gene[k]=tmp;
		}
		
/*
		for (int j = 0; j < size; ++j) {
			int pos = rand()% (len);
			for (int k = pos; k < len; ++k) {
				population[i].gene[k] = population[j].gene[k];
			}
		}
*/
	}
}

void mutation(vector<chromosome>& population) {
	int len=population[0].length;
	for (int i = 0; i < population.size(); ++i) {
		for(int j=0;j<len;++j){
			double p = rand()*1. / RAND_MAX;
			if(p<MUTATION)
				population[i].gene[j] = 1 - population[i].gene[j];
		}
	}
}

void evolution(vector<vector<edge> >& G, GSize& size, \
	vector<int>& table, vector<int>& min_flow, int min_sum, int serverCost, int& minfee) {

	
	int iter = ITER;
	double sum = 0.0;
	vector<chromosome> population;
	vector<chromosome> temp;
	char* used = new char[G.size()];
	int* dist = new int[G.size()];
	int* h = new int[G.size()];
	int* _stack = new int[G.size()];
	int* prevv = new int[G.size()];
	int* prevv_inv = new int[G.size()];
	int* preve = new int[G.size()];

	init(population, size);
	bool ok=updateFit(G, population, size, table, min_flow, used, h, _stack, dist, prevv, preve, min_sum, serverCost);
	while (!ok) {
		reInit(population, size);
		ok = updateFit(G, population, size, table, min_flow, used, h, _stack, dist, prevv, preve, min_sum, serverCost);
	}
	while (iter) {
		cout << "iteration: " << ITER - iter << endl;
		pick_chrom_sort(population);
		crossover(population);
		mutation(population);
		ok=updateFit(G, population, size, table, min_flow, used, h, _stack, dist, prevv, preve, min_sum, serverCost);
		//cout << "Fee: "<<population[1].cost << endl;
		while (!ok) {
			reInit(population, size);
			ok = updateFit(G, population, size, table, min_flow, used, h, _stack, dist, prevv, preve, min_sum, serverCost);
		}
		--iter;
	}
	cout << "Finished" << endl;
	double min = 10000;
	sum = 0.0;
	int min_id = 0;
	for (int i = 0; i < population.size(); ++i) {
		sum += population[i].rfit;
		if (population[i].cost < min && population[i].cost>0) {
			min_id = i;
			min = population[i].cost;
		}
	}
	int cnt = 0;
	for (int i = 0; i < population[min_id].length; ++i) {
		if (population[min_id].gene[i] == 1)
			++cnt;
	}
	cout << "Min fee: " << min <<", length: "<<cnt<< endl<<endl;
	minfee=min;
	delete used, h, _stack, dist, prevv, preve, prevv_inv;
}
