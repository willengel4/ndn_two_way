#include "stdafx.h"
#include "Consumer.h"
#include <iostream>

using namespace std;

void Consumer::Act()
{
	Node::Act();

	timeToSatisfaction += 1;

	if (satisfied)
	{
		cout << "\nConsumer with id:  " << this->nodeId << " has been satisfied in " << timeToSatisfaction << " intervals" << endl << endl;
		satisfied = false;
	}

	if (datas->size() == 0)
		return;

	/* Consumer has a 20% chance of wanting data */
	if (!first)
	{
		first = true;
		string nameRequest = (*(this->datas))[rand() % datas->size()]->name;
		waiting.push_back(nameRequest);
		cout << "Consumer with id " << nodeId << " wants " << nameRequest << endl;
		Packet * p = new Packet(1, 10, nameRequest);
		for (int i = 0; i < links.size(); i++)
			PreForward(p, links[i]);
	}
}
