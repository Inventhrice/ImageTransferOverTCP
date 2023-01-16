#include "../Client/Packet.h"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <windows.networking.sockets.h>
#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main()
{
	//starts Winsock DLLs		
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return 0;

	//create server socket
	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		WSACleanup();
		return 0;
	}

	//binds socket to address
	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27000);
	if (bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	//listen on a socket
	if (listen(ServerSocket, 1) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	cout << "waiting for connection" << endl;

	//accepts a connection from a client
	SOCKET ConnectionSocket;
	ConnectionSocket = SOCKET_ERROR;
	if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return 0;
	}

	cout << "connected!" << endl;

	char rxBuf[524];
	bool last = false;
	ofstream file;
	file.open("rxlowpoly.jpg", ios::binary);
	Packet pkt;
	int counter = 0;
	if (file.is_open()) {
		do {
			strcpy_s(rxBuf, sizeof(rxBuf), "");
			recv(ConnectionSocket, rxBuf, sizeof(rxBuf), 0);
			cout << "recieved!" << endl;
			Packet pkt = Packet(rxBuf);
			file.write(pkt.getData(), pkt.getLength());
			counter++;
		} while (counter < 125);
		file.close();
	}
	
	
	closesocket(ConnectionSocket);
	closesocket(ServerSocket);	    //closes server socket	
	WSACleanup();					//frees Winsock resources
}

