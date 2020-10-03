#pragma once
#include "general.h"
#include <iostream>
#include <vector>

#include "Raw.h"
#include "icmp_hdr.h"


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


	void writeData(std::vector<char>& t) {

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


	_ip_hdr* getHeader() {
		return &hdr;
	}
	friend std::ostream& operator<<(std::ostream& os, const ip_hdr& ip) {
		auto bytes = static_cast<const uint8_t*>((void*)&(ip.hdr));

		os << "+--------------------IP Header--------------------+" << std::endl
			<< std::setfill('0') << std::hex << std::setw(2)
			<< "Version/IHL: 0x" << std::setw(2) << (unsigned)bytes[0] << std::endl
			<< "DSCP/ECN:    0x" << std::setw(2) << (unsigned)bytes[1] << std::endl
			<< "Length:      0x" << std::setw(2) << (unsigned)bytes[2] << std::setw(2) << (unsigned)bytes[3] << std::endl
			<< "Id:          0x" << std::setw(2) << (unsigned)bytes[4] << std::setw(2) << (unsigned)bytes[5] << std::endl
			<< "Flags/Frag:  0x" << std::setw(2) << (unsigned)bytes[6] << std::setw(2) << (unsigned)bytes[7] << std::endl
			<< "TTL:         0x" << std::setw(2) << (unsigned)bytes[8] << std::endl
			<< "Protocol:    0x" << std::setw(2) << (unsigned)bytes[9] << std::endl
			<< "Checksum:    0x" << std::setw(2) << (unsigned)bytes[10] << std::setw(2) << (unsigned)bytes[11] << std::endl;

		char t[16];
		inet_ntop(AF_INET, &ip.hdr.src, t, 16);
		os << "Source:      " << t << std::endl;
		inet_ntop(AF_INET, &ip.hdr.dst, t, 16);
		os << "Destination: " << t << std::endl;
		return os;
	}

	Raw operator+(icmp_hdr& other) {
		hdr.proto = IPPROTO_ICMP;
		hdr.ttl = 64;
		hdr.total_len += htons(sizeof _icmp_hdr);

		return *static_cast<Raw*>(this) + other;
	}


};