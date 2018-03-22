#include "stdafx.h"
#include "Packet.h"
#include <string>

using namespace std;

Packet::Packet(int pt, int ds, string n)
{
	packetType = pt;
	dataSize = ds;
	name = n;
	timeToForward = dataSize;
}

void Packet::resetTimeToForward()
{
	timeToForward = dataSize;
}