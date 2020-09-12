#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include "general.h"


struct IP_Header {
#ifdef LITTLEENDIAN
	uint8_t ihl : 4,
	        version : 4;

#elif defined(BIGENDIAN)
	uint8_t version : 4;
	uint8_t ihl : 4;
#endif

	uint8_t DSCP : 6,
	        ECN : 2;

	uint16_t total_len, ident;

	uint16_t flags : 3,
	         frag_offset : 13;


	uint8_t ttl, proto;

	uint16_t header_chksum;
	uint32_t src, dst;

	friend ostream& operator<<(ostream& os, const IP_Header& ip) {
		auto bytes = static_cast<const uint8_t*>((void*)&ip);

		os  << "+--------------------IP Header--------------------+" << endl
			<< "Version/IHL: 0x" << setfill('0') << setw(2) << hex << (unsigned)bytes[0] << endl
			<< "DSCP/ECN:    0x" << setfill('0') << setw(2) << hex << (unsigned)bytes[1] << endl
			<< "Length:      0x" << setfill('0') << setw(2) << hex << (unsigned)bytes[2] << setw(2) << (unsigned)bytes[3] << endl
			<< "Id:          0x" << setfill('0') << setw(2) << hex << (unsigned)bytes[4] << setw(2) << (unsigned)bytes[5] << endl
			<< "Flags/Frag:  0x" << setfill('0') << setw(2) << hex << (unsigned)bytes[6] << setw(2) << (unsigned)bytes[7] << endl
			<< "TTL:         0x" << setfill('0') << setw(2) << hex << (unsigned)bytes[8] << endl
			<< "Protocol:    0x" << setfill('0') << setw(2) << hex << (unsigned)bytes[9] << endl
			<< "Checksum:    0x" << setfill('0') << setw(2) << hex << (unsigned)bytes[10] << setw(2) << (unsigned)bytes[11] << endl;

		char t[16];
		inet_ntop(AF_INET, &ip.src, t, 16);
		os << "Source:      " << t << endl;
		inet_ntop(AF_INET, &ip.dst, t, 16);
		os << "Destination: " << t << endl;
		return os;
	}



	IP_Header() {
		auto bytes8 = static_cast<uint8_t*>((void*)this);

		version = 4;
		ihl = 5;

		DSCP = 0;
		ECN = 0;

		total_len = htons(ihl*4); //Header only
		ident = htons(1);

		flags = IP_FLAG_NONE;
		frag_offset = 0;
		bytes8[6] = flags << 5 | frag_offset;

		ttl = 64;
		proto = IPPROTO_HOPOPTS;
		header_chksum = 0;

		ip_to_uint32(src, "192.168.178.31");
		ip_to_uint32(dst, "192.168.178.1");

		header_chksum = header_checksum(this, ihl * 4);
	}


};


struct ICMP_Header {
	uint8_t Type, Code;

	uint16_t Checksum;
	uint32_t rest;

	friend ostream& operator<<(ostream& os, const ICMP_Header& icmp) {
		os << "+-------------------ICMP Header-------------------+" << endl;

		os << "Type:        0x" << setfill('0') << setw(2) << hex << (unsigned)icmp.Type << endl;
		os << "Code:        0x" << setfill('0') << setw(2) << hex << (unsigned)icmp.Code << endl;
		os << "Checksum:    0x" << setfill('0') << setw(4) << hex << icmp.Checksum << endl;
		os << "Rest:        0x" << setfill('0') << setw(8) << hex << icmp.rest;
		return os;
	}

	ICMP_Header() {
		Type = 8;
		Code = 0;
		rest = 0;
		Checksum = 0;
		Checksum = header_checksum(this, sizeof ICMP_Header);
	}
};
