#ifndef PACKET_H
#define PACKET_H
#include <string>

using namespace std;

class Packet
{
public:
	int packetType;
	int dataSize;
	int timeToForward;
	string name;
	Packet(int pt, int ds, string n);
	void resetTimeToForward();
};
#endif