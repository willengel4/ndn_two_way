#ifndef NODE_H
#define NODE_H
#include <vector>
#include "Packet.h"
#include "Table.h"
#include "PairNodePacket.h"

using namespace std;

class Table;
class PairNodePacket;

class Node
{
	public:
		vector<Node*> links;
		vector<PairNodePacket*> inPackets;
		vector<PairNodePacket*> outPackets;
		Table* contentStore;
		Table* pit;
		Table* pat;
		Table* fib;
		Table* fab;
		vector<Packet *> * datas;
		int transmissionRate = 50;
		int nodeId;
		int xPos;
		int yPos;
		int nodeType;
		Node();
		virtual void Act();
		void View();
		void Forward(Packet * p, Node * to);
		void PreForward(Packet * p, Node * to);
		void PreForwardMultiple(Packet * p, vector<Node*> nodes, int excluding);
		void PreForwardMultiple(Packet * p, vector<Node*> nodes);
		void InsertPacket(PairNodePacket* pnp);
		void HandleOutPackets();
		void HandleInPackets();
		void HandleInterest(Packet * interest, Node * from);
		void HandleAd(Packet * ad, Node * from);
		void HandleData(Packet * data, Node * from);
};
#endif