#ifndef PRODUCER_H
#define PRODUCER_H

#include <string>
#include "Node.h"

using namespace std;

class Producer : public Node
{
public:
	string prefix;
	Producer(string pre);
	virtual void Act();
	bool first;
	string GenerateName(int len);
};
#endif