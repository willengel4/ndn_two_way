#include "stdafx.h"
#include "Consumer.h"
#include <iostream>

using namespace std;

void Consumer::Act()
{
	Node::Act();

	if (datas->size() == 0 || b)
		return;

	b = true;
	string nameRequest = (*(this->datas))[rand() % datas->size()]->name; // datas

	cout << "Consumer with id " << nodeId << " wants " << nameRequest << endl;

	Packet * p = new Packet(1, 10, nameRequest);
	for (int i = 0; i < links.size(); i++)
		PreForward(p, links[i]);
}
