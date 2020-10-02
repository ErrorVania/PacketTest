#pragma once
#include "general.h"
#include "Raw.h"


using std::setw;
using std::endl;
using std::ostream;
using std::hex;
using std::setfill;

struct _icmp_hdr {
	uint8_t Type, Code;

	uint16_t Checksum;
	uint32_t rest;


};

class icmp_hdr : public Raw {
protected:
	_icmp_hdr hdr;
	char icmp_data[576];


	void writeData(vector<char>& t) {
		const int siz = sizeof _icmp_hdr;

		t.resize(siz);

		hdr.Checksum = 0;
		hdr.Checksum = header_checksum(&hdr, sizeof _icmp_hdr);

		memcpy(t.data(), &hdr, siz);

	}

public:
	icmp_hdr() {
		memset(icmp_data, 0, 576);
		hdr.Type = 8;
		hdr.Code = 0;
		hdr.rest = htons(rand() % 0xffff);
		hdr.Checksum = 0;
		hdr.Checksum = header_checksum(this, sizeof _icmp_hdr);
	}
	_icmp_hdr* getHeader() {
		return &hdr;
	}


	friend std::ostream& operator<<(ostream& os, const icmp_hdr& icmp) {
		os << "+-------------------ICMP Header-------------------+" << endl;
		os << setfill('0') << hex;

		os << "Type:        0x" << setw(2) << (unsigned)icmp.hdr.Type << endl;
		os << "Code:        0x" << setw(2) << (unsigned)icmp.hdr.Code << endl;
		os << "Checksum:    0x" << setw(4) << icmp.hdr.Checksum << endl;
		os << "Rest:        0x" << setw(8) << icmp.hdr.rest;
		return os;
	}


};