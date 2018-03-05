#include "stdafx.h"
#include "Node.h"
#include <iostream>

using namespace std;

Node::Node()
{
	contentStore = new Table();
	pit = new Table();
	fib = new Table();
	fab = new Table();
	pat = new Table();
}

void Node::Act()
{
	cout << "Id " << nodeId << " acting." << endl;
	HandleOutPackets();
	HandleInPackets();
}

void Node::Forward(Packet * p, Node * to)
{
	to->InsertPacket(new PairNodePacket(this, p));
	cout << "Node " << nodeId << " forwarded Packet(" << p->packetType << "," << p->dataSize << "," << p->name << ") to " << to->nodeId << endl;
}

void Node::PreForward(Packet * p, Node * to)
{
	outPackets.push_back(new PairNodePacket(to, p));
	cout << "Node " << nodeId << " WILL forward Packet(" << p->packetType << "," << p->dataSize << "," << p->name << ") to " << to->nodeId << endl;
}

void Node::PreForwardMultiple(Packet * p, vector<Node*> nodes, int excluding)
{
	for (int i = 0; i < nodes.size(); i++)
		if (nodes[i]->nodeId != excluding)
			PreForward(p, nodes[i]);
}

void Node::PreForwardMultiple(Packet * p, vector<Node*> nodes)
{
	PreForwardMultiple(p, nodes, -1);
}

void Node::InsertPacket(PairNodePacket* pnp)
{
	inPackets.push_back(pnp);
}

void Node::HandleOutPackets()
{
	for (int i = outPackets.size() - 1; i >= 0; i--)
	{
		outPackets[i]->packet->timeToForward -= transmissionRate;

		if (outPackets[i]->packet->timeToForward <= 0)
		{
			Forward(outPackets[i]->packet, outPackets[i]->node);
			outPackets.erase(outPackets.begin() + i);
		}
	}
}

void Node::HandleInPackets()
{
	for (int i = 0; i < inPackets.size(); i++)
	{
		if (inPackets[i]->packet->packetType == 0)
			HandleData(inPackets[i]->packet, inPackets[i]->node);
		else if (inPackets[i]->packet->packetType == 1)
			HandleInterest(inPackets[i]->packet, inPackets[i]->node);
		else
			HandleAd(inPackets[i]->packet, inPackets[i]->node);
	}

	inPackets.clear();
}

void Node::HandleInterest(Packet * interest, Node * from)
{
	/* Content store hit. Forward the content
	* store packet back onto the incoming face */
	if (contentStore->SearchExact(interest->name))
	{
		PreForward(contentStore->SearchExact(interest->name)->packet, from);
		return;
	}

	/* PIT hit. Discard */
	if (pit->SearchExact(interest->name))
		return;

	/* PAT hit. Interest meets Ad. Send Interest
	* towards Producer via breadcrumb */
	if (pat->SearchExact(interest->name))
	{
		PreForward(interest, pat->SearchExact(interest->name)->nodes[0]);
		return;
	}

	/* Add the entry to the pat */
	pit->AddEntry(interest, from);

	/* FIB hit. Forward Interest along matching FIB entry faces */
	if (fib->SearchExact(interest->name))
	{
		vector<Node*> d = fib->SearchExact(interest->name)->nodes;
		PreForwardMultiple(interest, d);
		return;
	}

	/* FAB hit. Forward Interest along matching FAB entry faces */
	if (fab->SearchExact(interest->name))
	{
		PreForwardMultiple(interest, fab->SearchExact(interest->name)->nodes);
		return;
	}

	/* No hits, forward along all nodes except in node */
	PreForwardMultiple(interest, links, from->nodeId);
}

void Node::HandleAd(Packet * ad, Node * from)
{
	/* Content store hit. Forward the content
	* store packet back onto the incoming face */
	if (contentStore->SearchExact(ad->name))
		return;

	/* PIT hit. Ad meets interest. Copy ad, make it an interest. Forward it along pat entry face. */
	if (pit->SearchExact(ad->name))
	{
		Packet* searchResult = pit->SearchExact(ad->name)->packet;
		Packet* newInterest = new Packet(1, searchResult->dataSize, searchResult->name);
		PreForward(newInterest, pit->SearchExact(ad->name)->nodes[0]);
		return;
	}

	/* PAT hit. Redundant ad. */
	if (pat->SearchExact(ad->name))
		return;

	/* Add the entry to the pat table */
	pat->AddEntry(ad, from);

	/* FIB hit. Forward ad along matching FIB entry faces */
	if (fib->SearchExact(ad->name))
	{
		vector<Node*> searchNodes = fib->SearchExact(ad->name)->nodes;
		PreForwardMultiple(ad, searchNodes);
		return;
	}

	/* FAB hit. Forward ad along matching FAB entry faces */
	if (fab->SearchExact(ad->name))
	{
		PreForwardMultiple(ad, fab->SearchExact(ad->name)->nodes);
		return;
	}

	/* No hits, forward along all nodes except in node */
	PreForwardMultiple(ad, links, from->nodeId);
}

void Node::HandleData(Packet * data, Node * from)
{
	/* Content store hit. Duplicate, discard. */
	if (contentStore->SearchExact(data->name))
		return;

	contentStore->AddEntry(data, NULL);

	/* PIT hit. Send data towards consumer */
	if (pit->SearchExact(data->name))
	{
		PreForward(data, pit->SearchExact(data->name)->nodes[0]);
		return;
	}
}