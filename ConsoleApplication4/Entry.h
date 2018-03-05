#ifndef ENTRY_H
#define ENTRY_H

#include "Packet.h"
#include "Node.h"
#include <vector>

using namespace std;

class Node;
class Packet;

class Entry
{
	public:
		Packet * packet;
		vector<Node*> nodes;
};
#endif