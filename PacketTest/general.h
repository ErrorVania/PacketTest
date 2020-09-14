#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include <iomanip>
#pragma comment(lib,"Ws2_32.lib")

#define IP_FLAG_NONE 0
#define IP_FLAG_DF 2
#define IP_FLAG_MF 1
#define IP_FLAG_DFMF 3

using namespace std;

uint16_t header_checksum(void* vdata, size_t length) {
	// Cast the data pointer to one that can be indexed.
	char* data = (char*)vdata;

	// Initialise the accumulator.
	uint32_t acc = 0xFFFF; //0xFFFF

	// Handle complete 16-bit blocks.
	for (size_t i = 0; i + 1 < length; i += 2) {
		uint16_t word;
		memcpy(&word, data + i, 2);
		acc += ntohs(word);
		if (acc > 0xFFFF) {
			acc -= 0xffff;
		}
	}

	// Handle any partial block at the end of the data.
	if (length & 1) {
		uint16_t word = 0;
		memcpy(&word, data + length - 1, 1);
		acc += ntohs(word);
		if (acc > 0xffff) {
			acc -= 0xffff;
		}
	}

	// Return the checksum in network byte order.
	return htons(~acc);
}




inline bool isPrintable(unsigned char c) {
	if (c == 0x00)
		return false;
	auto b = static_cast<signed char>(c);
	return isprint(c);
}



void display(const void* object,size_t siz)
{
	auto bytes = static_cast<const unsigned char*>(object);
	size_t i;

	const unsigned int offs = 8;
	unsigned char buf[offs];

	bool first = true;
	cout << hex << setfill('0');
	for (i = 0; i < siz; i++) {

		if (i % offs == 0) {
			if (i > 0) {
				cout << "| ";
				memcpy(buf, bytes + i - offs, offs);
				for (int x = 0; x < offs; x++) {
					if (isPrintable(buf[x]) != 0) {
						cout << buf[x];
					}
					else {
						cout << ".";
					}
				}
			}
			cout << endl << "0x" << setw(offs) << i << " | ";
		}
		cout << setw(2) << (unsigned)bytes[i] << " ";
	}
	

	cout << setw(1);
	size_t backup = i;
	while (i % offs != 0) {
		cout << "   ";
		i++;
	}
	cout << "| ";
	memcpy(buf, bytes + siz - offs, offs);

	unsigned t = backup % offs;
	t = t == 0 ? t = 8 : backup % offs;


	for (unsigned x = 0; x < t; x++) {

		if (isPrintable(buf[x])) {
			cout << buf[x];
		}
		else {
			cout << ".";
		}
	}


	cout << endl;
}





uint32_t ip_to_uint32(uint32_t& x, const char* buf) {
	IN_ADDR ia;

	inet_pton(AF_INET, buf, &ia);
	x = ia.S_un.S_addr;
	return x;
}
unsigned long ip_to_ulong(unsigned long& x, const char* buf) {
	IN_ADDR ia;

	inet_pton(AF_INET, buf, &ia);
	x = ia.S_un.S_addr;
	return x;
}