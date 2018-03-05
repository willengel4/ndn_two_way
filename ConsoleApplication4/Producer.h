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
	string GenerateName(int len);

	bool b;
};
#endif