#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include "general.h"

using namespace std;

struct IP_Header {
#ifdef LITTLEENDIAN
	uint8_t ihl : 4;
	uint8_t version : 4;

#elif defined(BIGENDIAN)
	uint8_t version : 4;
	uint8_t ihl : 4;
#endif



	uint8_t DSCP : 6;
	uint8_t ECN : 2;

	uint16_t total_len;
	uint16_t ident;


	uint16_t flags : 3;
	uint16_t frag_offset : 13;


	uint8_t ttl;
	uint8_t proto;
	uint16_t header_chksum;
	uint32_t src;
	uint32_t dst;

};
struct ICMP_Header {
	uint8_t Type;
	uint8_t Code;

	uint16_t Checksum;
	uint32_t rest;
};




void populateDefault(IP_Header* ip) {
	auto bytes8 = static_cast<uint8_t*>((void*)ip);

	ip->version = 4;
	ip->ihl = 5;



	ip->DSCP = 0;
	ip->ECN = 0;

	ip->total_len = htons(20); //no data
	ip->ident = htons(1);


	ip->flags = IP_FLAG_NONE;
	ip->frag_offset = 0;
	bytes8[6] = ip->flags << 5 | ip->frag_offset;


	ip->ttl = 64;
	ip->proto = IPPROTO_HOPOPTS;
	ip->header_chksum = 0;

	ip_to_uint32(ip->src, "192.168.178.31");
	ip_to_uint32(ip->dst, "192.168.178.1");




	ip->header_chksum = header_checksum(ip, ip->ihl * 4);

}
void populateDefault(ICMP_Header* icmp) {
	icmp->Type = 8;
	icmp->Code = 0;
	icmp->rest = 0;
	icmp->Checksum = 0;
	icmp->Checksum = header_checksum(icmp, sizeof ICMP_Header);
}


















void displayHeader(IP_Header* ip) {
	auto bytes = static_cast<const unsigned char*>((void*)ip);


	size_t i = 0;



	printf("Version/IHL: 0x%02x\n", bytes[i]);
	i += 1;
	printf("DSCP/ECN: 0x%02x\n", bytes[i]);
	i += 1;
	printf("Length: 0x%02x%02x\n", bytes[i], bytes[i + 1]);
	i += 2;
	printf("Id: 0x%02x%02x\n", bytes[i], bytes[i + 1]);
	i += 2;
	printf("Flags/Frag: 0x%02x%02x\n", bytes[i], bytes[i + 1]);
	i += 2;
	printf("TTL: 0x%02x\n", bytes[i]);
	i += 1;
	printf("Protocol: 0x%02x\n", bytes[i]);
	i += 1;
	printf("Checksum: 0x%02x%02x\n", bytes[i], bytes[i + 1]);
	i += 1;



	char t[16];
	inet_ntop(AF_INET, &ip->src, t, 16);
	cout << "Source: " << t << std::endl;
	inet_ntop(AF_INET, &ip->dst, t, 16);
	cout << "Destination: " << t << std::endl;

}
void displayHeader(ICMP_Header* icmp) {
	auto bytes = static_cast<const unsigned char*>((void*)icmp);

	printf("Type: 0x%02x\n", bytes[0]);
	printf("Code: 0x%02x\n", bytes[1]);
	printf("Checksum: 0x%02x%02x\n", bytes[2], bytes[3]);
	printf("Rest: ");
	for (int i = 4; i < sizeof ICMP_Header; i++) {
		printf("0x%02x ", bytes[i]);
	}
	std::cout << std::endl;

}