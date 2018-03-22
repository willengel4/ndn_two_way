#include "stdafx.h"
#include "Table.h"
#include <iostream>
using namespace std;

Entry* Table::SearchExact(string name)
{
	for (int i = 0; i < entries.size(); i++)
		if (entries[i]->packet->name == name)
			return entries[i];
	return NULL;
}

int Table::min(int a, int b)
{
	return  a < b ? a : b;
}

void Table::RemoveEntry(Entry * entry)
{
	for (int i = 0; i < entries.size(); i++)
	{
		if (entries[i] == entry)
		{
			entries.erase(entries.begin() + i);
			return;
		}
	}
}

Entry* Table::SearchLongestPrefix(string name)
{
	int longest = 0;
	Entry * ent = NULL;

	for (int i = 0; i < entries.size(); i++)
	{
		string n = entries[i]->packet->name;

		int countSlash = 0;
		for (int f = 0; f < min(n.length(), name.length()) && n[i] == name[i]; f++)
			if (n[i] == '/')
				countSlash++;

		if (countSlash > longest)
		{
			longest = countSlash;
			ent = entries[i];
		}
	}

	return ent;
}

void Table::View()
{
	for (int i = 0; i < entries.size(); i++)
	{
		for (int f = 0; f < entries[i]->nodes.size(); f++)
			if(entries[i]->nodes[f] != NULL)
				cout << entries[i]->nodes[f]->nodeId << " ";
		cout << "\tPacket(" << entries[i]->packet->packetType << "," << entries[i]->packet->name << ")" << endl;
	}
}

void Table::AddEntry(Packet * p, Node * node)
{
	Entry * e = new Entry();
	e->packet = p;
	e->nodes.push_back(node);
	entries.push_back(e);
}