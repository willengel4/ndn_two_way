#include "stdafx.h"
#include <iostream>
#include "Consumer.h"
#include "Producer.h"

using namespace std;

int main()
{
	vector<Packet *> allDataObjects;
	Consumer * consumer = new Consumer();
	consumer->nodeId = 1;
	Producer * producer = new Producer("test/com/");
	producer->nodeId = 2;
	Node * router = new Node();
	router->nodeId = 3;


	consumer->links.push_back(router);
	router->links.push_back(consumer);

	router->links.push_back(producer);
	producer->links.push_back(router);

	consumer->datas = producer->datas = router->datas = &allDataObjects;

	for (int i = 0; i < 10; i++)
	{
		producer->Act();
		consumer->Act();
		router->Act();
	}

	system("pause");

	return 0;
}