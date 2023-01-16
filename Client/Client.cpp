#include "Packet.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

//Serializes the Packet object into a char* to prepare for sending via sockets.
void SerializeFile(vector<char*> &pktHold, vector<int> &sizePkt, string filename) {
	int numWholePackets = 0, numPartPackets = 0;
	Packet pkt;

	ifstream file;
	file.open(filename, ios::binary);

	if (file.is_open()) {
		file.seekg(0, ios::end);
		size_t fileSize = (size_t)file.tellg();
		file.seekg(0);

		numWholePackets = fileSize / BODY_LENGTH_MAX;
		numPartPackets = fileSize % BODY_LENGTH_MAX;
		bool last = false;
		int packetLength = BODY_LENGTH_MAX;

		for (int i = 0; i <= numWholePackets; i++) {

			if (i == numWholePackets) {
				packetLength = numPartPackets;
				last = true;
			}
			char* packetBody = new char[packetLength * sizeof(char)];
			file.read(packetBody, packetLength);

			pkt.setHeader(SYNC, i + 1, last, packetLength);
			pkt.setData(packetBody, packetLength);

			int size = 0;
			pktHold.push_back(pkt.SerializePacket(&size));
			sizePkt.push_back(size);
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

	vector<char*> pktHold;
	vector<int> sizePkt;
	SerializeFile(pktHold, sizePkt, "lowpoly.jpg");

	for(int i = 0; i < pktHold.size(); i++){
		send(ClientSocket, pktHold.at(i), sizePkt.at(i), 0);
		cout << "sent!" << endl;
		Sleep(10);
	}
	closesocket(ClientSocket);
	WSACleanup();
}

