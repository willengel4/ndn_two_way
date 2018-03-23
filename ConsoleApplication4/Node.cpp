#include "stdafx.h"
#include "Node.h"
#include <iostream>
#include "Consumer.h"

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

void Node::View()
{
	return;
	
	cout << "Displaying node: " << nodeId << endl;
	cout << "Packets in in-queue..." << endl;
	for (int i = 0; i < inPackets.size(); i++)
		cout << inPackets[i]->node->nodeId << "\t" << "Packet(" << inPackets[i]->packet->packetType << "," << inPackets[i]->packet->name << ")" << endl;
	cout << "Packets in out-queue..." << endl;
	for (int i = 0; i < outPackets.size(); i++)
		cout << outPackets[i]->node->nodeId << "\t" << "Packet(" << outPackets[i]->packet->packetType << "," << outPackets[i]->packet->name << ")" << endl;
	cout << "Content store" << endl;
	this->contentStore->View();
	cout << "PIT" << endl;
	this->pit->View();
	cout << "PAT" << endl;
	this->pat->View();
	cout << "FIB" << endl;
	this->fib->View();
	cout << "FAB" << endl;
	this->fab->View();

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
		cout << "Node " << this->nodeId << " has received packet (" << inPackets[i]->packet->packetType << ", " << inPackets[i]->packet->dataSize << ", " << inPackets[i]->packet->name << ")" << endl;
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
		cout << "Content store hit. Forwarding data back down stream" << endl;
		PreForward(contentStore->SearchExact(interest->name)->packet, from);
		return;
	}

	/* PIT hit. Discard */
	if (pit->SearchExact(interest->name))
	{
		cout << "PIT Hit. Duplicate interest. Discarding." << endl;
		return;
	}
	/* Add the entry to the pit */
	pit->AddEntry(interest, from);

	/* PAT hit. Interest meets Ad. Send Interest
	* towards Producer via breadcrumb */
	if (pat->SearchExact(interest->name))
	{
		cout << "PAT hit. Sending interest upstream toward producer." << endl;
		PreForward(interest, pat->SearchExact(interest->name)->nodes[0]);
		return;
	}

	/* FIB hit. Forward Interest along matching FIB entry faces */
	if (fib->SearchExact(interest->name))
	{
		cout << "FIB Hit. Forwarding interest along FIB faces" << endl;
		vector<Node*> d = fib->SearchExact(interest->name)->nodes;
		PreForwardMultiple(interest, d);
		return;
	}

	/* FAB hit. Forward Interest along matching FAB entry faces */
	if (fab->SearchExact(interest->name))
	{
		cout << "FAB Hit. Forwarding interest along FAB faces" << endl;
		PreForwardMultiple(interest, fab->SearchExact(interest->name)->nodes);
		return;
	}

	cout << "No table hits. Forwarding along all faces." << endl;
	/* No hits, forward along all nodes except in node */
	PreForwardMultiple(interest, links, from->nodeId);
}

void Node::HandleAd(Packet * ad, Node * from)
{
	/* Content store hit. data already exists and doesn't need an Ad here. Discard ad. */
	if (contentStore->SearchExact(ad->name))
	{
		cout << "Content store hit. Redundant Ad. Discarding" << endl;
		return;
	}

	/* PIT hit. Ad meets interest. Copy ad, make it an interest. Forward it along pat entry face. */
	if (pit->SearchExact(ad->name))
	{
		cout << "PIT hit. sending interest up towards producer along incoming face." << endl;
		Packet* searchResult = pit->SearchExact(ad->name)->packet;
		Packet* newInterest = new Packet(1, searchResult->dataSize, searchResult->name);
		PreForward(newInterest, from);
		return;
	}

	/* PAT hit. Redundant ad. */
	if (pat->SearchExact(ad->name))
	{
		cout << "PAT hit. Redundant Ad. Discarding" << endl;
		return;
	}
	/* Add the entry to the pat table */
	pat->AddEntry(ad, from);

	/* FIB hit. Forward ad along matching FIB entry faces */
	if (fib->SearchExact(ad->name))
	{
		cout << "FIB Hit. Forwarding ad along fib faces" << endl;
		vector<Node*> searchNodes = fib->SearchExact(ad->name)->nodes;
		PreForwardMultiple(ad, searchNodes);
		return;
	}

	/* FAB hit. Forward ad along matching FAB entry faces */
	if (fab->SearchExact(ad->name))
	{
		cout << "FAB hit. Forwarding ad along fab faces" << endl;
		PreForwardMultiple(ad, fab->SearchExact(ad->name)->nodes);
		return;
	}

	cout << "No table hits. Forwarding ad along all faces." << endl;
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
		if (this->nodeType == 0)
		{
			Consumer * c = (Consumer*)this;
			for (int i = 0; i < c->waiting.size(); i++)
			{
				if (c->waiting[i] == data->name)
				{
					c->satisfied = true;
				}
			}
		}

		Entry * entry = pit->SearchExact(data->name);
		cout << "Sending  data to " << entry->nodes[0]->nodeId << " and removing interest from PIT" << endl;
		pit->RemoveEntry(entry);
		PreForward(data, entry->nodes[0]);
		return;
	}
}