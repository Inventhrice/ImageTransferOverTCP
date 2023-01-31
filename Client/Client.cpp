/*
This is the sending side of this ImageTransfer. A lot of this is draft code, as it was made to be a
proof of concept for another project being worked on.

Thank you to Benjamin Smith for helping me with understanding the code.
*/

#include "Packet.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

//Serializes the Packet object into a char* to prepare for sending via sockets.
void SerializeFile(vector<char*> &pktHold, int sizePkt[], string filename) {
	int numWholePackets = 0, sizeOfPartPkt = 0;
	Packet pkt;

	ifstream file;
	file.open(filename, ios::binary);

	if (file.is_open()) {
		file.seekg(0, ios::end);
		size_t fileSize = (size_t)file.tellg(); //gets the file size
		file.seekg(0);

		sizePkt[0] = fileSize / BODY_LENGTH_MAX; //determines the amount of max capacity packets to be sent
		sizePkt[1] = fileSize % BODY_LENGTH_MAX; //determines the length of the remainder of the data left
		bool last = false;
		int packetLength = BODY_LENGTH_MAX;

		for (int i = 0; i <= sizePkt[0]; i++) {

			if (i == sizePkt[0]) {
				packetLength = sizeOfPartPkt;
				last = true;
			}
			char* packetBody = new char[packetLength * sizeof(char)];
			file.read(packetBody, packetLength);

			pkt.setHeader(SYNC, i + 1, last, packetLength); //hardcoded a SYNC state, once again, this is test code.
			pkt.setData(packetBody, packetLength);

			int size = 0;
			pktHold.push_back(pkt.SerializePacket(&size));
		}
	}
}

int main()
{
	//starts Winsock DLLs
	WSADATA wsaData;
	if ((WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0) {
		return 0;
	}

	//initializes socket. SOCK_STREAM: TCP
	SOCKET ClientSocket;
	ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET) {
		WSACleanup();
		return 0;
	}

	//Connect socket to specified server
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;						//Address family type itnernet
	SvrAddr.sin_port = htons(27000);					//port (host to network conversion)
	SvrAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	//IP address
	if ((connect(ClientSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr))) == SOCKET_ERROR) {
		closesocket(ClientSocket);
		WSACleanup();
		return 0;
	}

	vector<char*> pktHold; //vector to hold all the seralized data pointers
	
	int sizePkt[2]; //This int array holds two values. The first value is the number of whole packets, the second is the size of the remainder data
	//vector<int> sizePkt; 

	SerializeFile(pktHold, sizePkt, "lowpoly.jpg");
	int sizeTx = PACKET_SIZE_MAX; //variable to hold the size of transmission

	for(int i = 0; i < pktHold.size(); i++){

		//This checks if i is above the number of whole packets, in which case we need to modify the size of the transmission
		if (i > sizePkt[0]) sizeTx = sizePkt[1]; 

		send(ClientSocket, pktHold.at(i), sizeTx, 0);
		cout << "sent!" << endl;
		Sleep(10);
	}
	closesocket(ClientSocket);
	WSACleanup();
}

