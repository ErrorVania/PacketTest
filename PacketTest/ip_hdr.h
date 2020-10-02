#pragma once
#include "general.h"
#include <iostream>
#include <vector>

#include "Raw.h"

using std::vector;

struct _ip_hdr {
#ifdef LITTLEENDIAN
	uint8_t ihl : 4,
		version : 4;

#elif defined(BIGENDIAN)
	uint8_t version : 4,
		ihl : 4;
#endif

	uint8_t tos;

	uint16_t total_len, ident;

	uint16_t flag_frag;


	uint8_t ttl, proto;

	uint16_t header_chksum;
	uint32_t src, dst;
};

class ip_hdr : public Raw {
protected:
	_ip_hdr hdr;
	char options[128];


	void writeData(vector<char>& t) {

		const int siz = hdr.ihl * 4;
		t.resize(siz);

		hdr.header_chksum = 0;
		hdr.header_chksum = header_checksum(&hdr, hdr.ihl * 4);

		memcpy(t.data(), &hdr, siz); //options is right after hdr
	}

public:
	_ip_hdr* const p_hdr = &hdr;
	ip_hdr() {
		memset(options, 0, 128);
		hdr.version = 4;
		hdr.ihl = 5;
		hdr.tos = 0;
		hdr.ident = 0;
		hdr.flag_frag = 0;
		hdr.ttl = 0;
		hdr.proto = IPPROTO_HOPOPTS;
		hdr.src = 0;
		hdr.dst = 0;
		hdr.total_len = htons(hdr.ihl * 4);
		hdr.header_chksum = 0;

	}


	/*_ip_hdr* getHeader() {
		return &hdr;
	}*/
	friend std::ostream& operator<<(std::ostream& os, const ip_hdr& ip) {
		auto bytes = static_cast<const uint8_t*>((void*)&(ip.hdr));

		os << "+--------------------IP Header--------------------+" << endl
			<< setfill('0') << hex << setw(2)
			<< "Version/IHL: 0x" << setw(2) << (unsigned)bytes[0] << endl
			<< "DSCP/ECN:    0x" << setw(2) << (unsigned)bytes[1] << endl
			<< "Length:      0x" << setw(2) << (unsigned)bytes[2] << setw(2) << (unsigned)bytes[3] << endl
			<< "Id:          0x" << setw(2) << (unsigned)bytes[4] << setw(2) << (unsigned)bytes[5] << endl
			<< "Flags/Frag:  0x" << setw(2) << (unsigned)bytes[6] << setw(2) << (unsigned)bytes[7] << endl
			<< "TTL:         0x" << setw(2) << (unsigned)bytes[8] << endl
			<< "Protocol:    0x" << setw(2) << (unsigned)bytes[9] << endl
			<< "Checksum:    0x" << setw(2) << (unsigned)bytes[10] << setw(2) << (unsigned)bytes[11] << endl;

		char t[16];
		inet_ntop(AF_INET, &ip.hdr.src, t, 16);
		os << "Source:      " << t << endl;
		inet_ntop(AF_INET, &ip.hdr.dst, t, 16);
		os << "Destination: " << t;
		return os;
	}


};