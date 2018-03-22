#ifndef CONSUMER_H
#define CONSUMER_H
#include "Node.h"

class Consumer : public Node
{
public:
	virtual void Act();
	bool first;
	bool satisfied;
	int timeToSatisfaction;
	vector<string> waiting;
};
#endif