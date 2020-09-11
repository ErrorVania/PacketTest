#pragma once
#define _WINSOCK_DEPRACATED_NO_WARNINGS
#include <WS2tcpip.h>
#include <iostream>
#pragma comment(lib,"Ws2_32.lib")

#define IP_FLAG_NONE 0
#define IP_FLAG_DF 2
#define IP_FLAG_MF 1
#define IP_FLAG_DFMF 3


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
void display(const void* object,size_t siz)
{
	auto bytes = static_cast<const unsigned char*>(object);

	for (size_t x = 0; x < siz; x++) {
		if (x % 8 == 0 && x != 0)
			printf("\n");
		printf("%02x ", (UINT)bytes[x]);

	}
	std::cout << std::endl;



}
void ip_to_uint32(uint32_t& x, const char* buf) {
	IN_ADDR ia;

	inet_pton(AF_INET, buf, &ia);
	x = ia.S_un.S_addr;
}