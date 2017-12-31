#ifndef __ROUTE_H__
#define __ROUTE_H__

#include "lib_io.h"
#include "graph.h"
#include "scan.h"
#include "genetic.h"

void deploy(char * topo[MAX_EDGE_NUM], int line_num, char* filename);

void deploy_server(char * topo[MAX_EDGE_NUM], int line_num,char * filename);
	

#endif
