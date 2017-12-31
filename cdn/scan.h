#pragma once
#ifndef _SCAN_H_
#define _SCAN_H_
#define	SHIFTP			0.7
#define DISCARD			0.7
#define START_SHIFT		0
#define START_DISCARD	0
#define STOP_SHIFT      0.08
#define STOP_DISCARD    0.05

#define CONST_SHIFT		0.5
#define CONST_DISCARD	0.3
#define SHIFT_LOW_BOUND	10

#include "graph.h"
#include <ctime>

using namespace std;

struct p { int id; int size; };

bool Comp(const p& p1, const p& p2);

void scanNetwork(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& min_flow, int min_sum, int serverCost);

void shift(vector<vector<edge> >& G, GSize& size, int pos, int& max_pos, int& second_pos);

void sort_density(vector<vector<edge> >& G, vector<p>& pair, GSize& size, int pos) ;

void sort_value(vector<vector<edge> >& G, vector<p>& pair, GSize& size, int pos) ;

int calc_value(vector<vector<edge> >& G, GSize& size, int pos);

void execute(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& Gtable, vector<int>& min_flow, vector<int>& min_index, int min_sum, int serverCost, int& minfee);

void execute2(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& Gtable, vector<int>& min_flow, vector<int>& min_index, int min_sum, int serverCost, int& minfee);

void execute3(vector<vector<edge> >& G, GSize& size, vector<int>& table, vector<int>& Gtable, vector<int>& min_flow, vector<int>& min_index, int min_sum, int serverCost, int& minfee);

#endif
