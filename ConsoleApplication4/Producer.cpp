#include "stdafx.h"
#include "Producer.h"
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include <iostream>

Producer::Producer(string pre)
{
	this->prefix = pre;
}

void Producer::Act()
{
	Node::Act();

	/* 20% chance of generating data */
	if (!first)
	{
		first = true;
		string newDataName = prefix + GenerateName(10);
		int newDataSize = rand() % 100 + 10;
		Packet * p = new Packet(0, newDataSize, newDataName);
		contentStore->AddEntry(p, NULL);
		cout << "Producer with id " << nodeId << " generated Packet(" << p->packetType << "," << p->dataSize << "," << newDataName << ")" << endl;
		Packet * ad = new Packet(2, 10, newDataName);
		for (int i = 0; i < links.size(); i++)
			PreForward(ad, links[i]);
		datas->push_back(p);
	}
}

string Producer::GenerateName(int len)
{
	string s = "";

	srand(time(0));

	for (int i = 0; i < len; i++)
		s += (char)(rand() % 25 + 65);

	return s;
}
