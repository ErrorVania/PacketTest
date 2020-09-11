#pragma once

#include <WS2tcpip.h>
#include <iostream>
#include "general.h"



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

void populateHeaderDefault(IP_Header* ip) {
	auto bytes8 = static_cast<uint8_t*>((void*)ip);

	ip->version = 4;
	ip->ihl = 5;
	//size_t used = (ULONG)ip->IHL * 4;
	//bytes8[0] = ip->version << 4 | ip->IHL;



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




	ip->header_chksum = header_checksum(ip, ip->ihl*4);

}



void displayHeader(const IP_Header* object)
{
	auto bytes = static_cast<const unsigned char*>((void*)object);


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
	inet_ntop(AF_INET, &object->src, t, 16);
	std::cout << "Source: " << t << std::endl;
	inet_ntop(AF_INET, &object->dst, t, 16);
	std::cout << "Destination: " << t << std::endl;
	

}