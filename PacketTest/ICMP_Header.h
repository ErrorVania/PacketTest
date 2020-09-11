#pragma once
#include <WS2tcpip.h>
#include "general.h"
#include <iostream>


struct ICMP_Header {
	uint8_t Type;
	uint8_t Code;

	uint16_t Checksum;
	uint32_t rest;
};






void populateICMPDefault(ICMP_Header* icmp) {
	icmp->Type = 8;
	icmp->Code = 0;
	icmp->rest = 0;
	icmp->Checksum = 0;
	icmp->Checksum = header_checksum(icmp, sizeof ICMP_Header);
}


void displayICMP(ICMP_Header* icmp) {
	auto bytes = static_cast<const unsigned char*>((void*)icmp);

	printf("Type: 0x%02x\n", bytes[0]);
	printf("Code: 0x%02x\n", bytes[1]);
	printf("Checksum: 0x%02x%02x\n", bytes[2], bytes[3]);
	printf("Rest: ");
	for (int i = 4; i < sizeof ICMP_Header; i++) {
		printf("0x%02x ",bytes[i]);
	}
	std::cout << std::endl;

}