#ifndef TABLE_H
#define TABLE_H
#include <vector>
#include "Entry.h"
#include "Node.h"
#include "Packet.h"

using namespace std;

class Table
{
public:
	vector<Entry *> entries;
	Entry* SearchExact(string name);
	Entry* SearchLongestPrefix(string name);
	void AddEntry(Packet * p, Node * node);
	void View();
	void RemoveEntry(Entry * e);
	int min(int a, int b);
};
#endif