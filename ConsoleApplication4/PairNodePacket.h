#ifndef PAIRNODE_H
#define PAIRNODE_H
#include "Packet.h"
#include "Node.h"

using namespace std;

class Node;
class Packet;

class PairNodePacket
{
public:
	Node * node;
	Packet * packet;
	PairNodePacket(Node * n, Packet * p);
};
#endif