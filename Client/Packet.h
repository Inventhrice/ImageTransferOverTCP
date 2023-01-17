#pragma once
#include <memory>

enum STATE { UPLOAD, REQUEST, SYNC, AUTH };

const int BODY_LENGTH_MAX = 512;
const int PACKET_SIZE_MAX = sizeof(STATE) + sizeof(unsigned int) + sizeof(bool) + sizeof(unsigned short) + sizeof(char) + 512;
class Packet {
private:
	//PacketHeader object, containing protocol information. More information can be found in the PacketHeader's header file.
	struct Header
	{
		STATE packet_state;				//what the action the packet is performing	4 bytes
		unsigned int packet_number;		//sequence # of packet						4 bytes
		bool packet_last;				//true if last packet in the run			1 byte
		unsigned short body_length;		//											2 bytes
	} Head;
	//Pointer to a character array containing the packet's body, no larger than the value of ##INSERT VALUE##
	char* packet_body;

public:
	Packet() {
		setHeader(UPLOAD, 0, false, 0);
		this->packet_body = nullptr;
	}
	Packet(char* src) {
		size_t offset = 0;

		memcpy(&this->Head, src, sizeof(Header));
		offset += sizeof(Header);

		int size = sizeof(char) * this->Head.body_length;
		this->packet_body = new char[size];
		memcpy(this->packet_body, src + offset, size);
	}
	
	void setHeader(STATE state, unsigned int number, bool last, unsigned short length) {
		this->Head.packet_state = state;
		this->Head.packet_number = number;
		this->Head.packet_last = last;
		this->Head.body_length = length;
	}

	char* getData() {
		return this->packet_body;
	}

	unsigned short getLength() {
		return this->Head.body_length;
	}

	bool getLast() {
		return this->Head.packet_last;
	}

	void setData(char* srcData, int size) {
		this->packet_body = new char[(size * sizeof(char))];
		memcpy(this->packet_body, srcData, size);
	}

	char* SerializePacket(int* totalSize) {
		*totalSize = 0;
		char* txBuf = new char[sizeof(Header) + (sizeof(char) * this->Head.body_length)];
		
		memcpy(txBuf, &this->Head, sizeof(Header));
		*totalSize += sizeof(Header);

		memcpy(txBuf + *totalSize, this->packet_body, (sizeof(char) * this->Head.body_length));
		*totalSize += (sizeof(char) * this->Head.body_length);

		return txBuf;
	}
};
