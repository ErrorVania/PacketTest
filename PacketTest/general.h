#pragma once

#include <WS2tcpip.h>
#include <iostream>
#include <iomanip>
#include "defs.h"

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


//right display is broken, displays wrong bytes
void display(const void* object, size_t siz, const unsigned offs = 8)
{
	auto bytes = static_cast<const uint8_t*>(object);
	size_t i;

	uint8_t* buf = (uint8_t*)malloc(offs);

	std::cout << std::hex << std::setfill('0');
	for (i = 0; i < siz; i++) {

		if (i % offs == 0) {
			if (i > 0) {
				std::cout << "| ";
				/*memcpy(buf, bytes + i - offs, offs);
				for (int x = 0; x < offs; x++) {
					if (isPrintable(buf[x]) != 0) {
						cout << buf[x];
					}
					else {
						cout << ".";
					}
				}*/
			}
			std::cout << std::endl << "0x" << std::setw(offs) << i << " | ";
		}
		std::cout << std::setw(2) << (unsigned)bytes[i] << " ";
	}


	std::cout << std::setw(1);
	size_t backup = i;
	while (i % offs != 0) {
		std::cout << "   ";
		i++;
	}
	std::cout << "| ";
	//memcpy(buf, bytes + siz - offs, offs);

	unsigned t = backup % offs;
	t = t == 0 ? t = 8 : backup % offs;


	/*for (unsigned x = 0; x < t; x++) {

		if (isPrintable(buf[x])) {
			cout << buf[x];
		}
		else {
			cout << ".";
		}
	}*/

	free(buf);
	std::cout << std::endl;
}



uint32_t toip(const char* buf) {
	in_addr ia;
	inet_pton(AF_INET, buf, &ia);
	return ia.S_un.S_addr;
}
std::string toaddr(uint32_t addr) {
	char f[16];
	inet_ntop(AF_INET, &addr, f, 16);
	return std::string(f);

}







std::tuple<uint32_t, uint32_t> getIPInfo() {
	SOCKET s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	icmp_hdr i;
	







	return std::make_tuple(0, 0);
}