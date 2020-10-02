#pragma once
#include "general.h"
#include "ip_hdr.h"
#include "icmp_hdr.h"






inline SOCKET getRawSock(bool ip_hdrincl) {

	WSADATA wsaData;
	SOCKET s;
	const int optval = 1,
		wsaret = WSAStartup(MAKEWORD(2, 2), &wsaData);


	if (wsaret != 0) {
		cout << wsaret << endl << WSAGetLastError() << endl;
		return 1;
	}
	else {
		s = socket(AF_INET, SOCK_RAW, 0);
		if (s == INVALID_SOCKET) {
			cout << "Invalid socket " << WSAGetLastError() << endl;
			return 2;
		}
		if (ip_hdrincl)
			setsockopt(s, IPPROTO_IP, IP_HDRINCL, (const char*)&optval, sizeof optval); //define IP Header ourselves
	}
	return s;
}




const int sendL3(SOCKET sock, const void* buffer, unsigned int buflen, int flags = 0, ADDRESS_FAMILY fam = AF_INET) {

	sockaddr_in dst;
	dst.sin_family = fam;
	dst.sin_addr.S_un.S_addr = ((_ip_hdr*)buffer)->dst;



	DWORD hdr = 0;
	int len = sizeof hdr;

	getsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char*)&hdr, &len);
	if (hdr >= 0) {

		return sendto(sock, (const char*)buffer, buflen, flags, (sockaddr*)(&dst), sizeof sockaddr_in);
	}
	cout << "malformed packet" << endl;
	return 0;
}





/*class ICMP_Pinger {
private:
	const unsigned packetsize = sizeof _ip_hdr + sizeof _icmp_hdr;
	char* packet = nullptr;
	_ip_hdr* ip = nullptr;
	_icmp_hdr* icmp = nullptr;
	SOCKET rawsock = INVALID_SOCKET;

	void reroll_Id() {
		icmp->rest = htons(rand() % 0xffff);
	}
	void recalc_chksums() {
		ip->header_chksum = 0x0000;
		icmp->Checksum = 0x0000;
		ip->header_chksum = header_checksum(ip, ip->ihl * 4);
		icmp->Checksum = header_checksum(icmp, sizeof _icmp_hdr);
	}

public:

	ICMP_Pinger() {
		packet = static_cast<char*>(malloc(packetsize));
		rawsock = getRawSock(true);
		const uint32_t timeout = 500;
		setsockopt(rawsock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);


		ip = (_ip_hdr*)packet;
		icmp = (_icmp_hdr*)(packet + sizeof _ip_hdr);



		_ip_hdr _ip;
		_icmp_hdr _icmp;

		_ip.dst = 0x00000000;
		_ip.src = 0x00000000;

		_ip.total_len += htons(sizeof _icmp_hdr);
		_ip.proto = IPPROTO_ICMP;
		_ip.header_chksum = 0x0000;
		_ip.header_chksum = header_checksum(&_ip, _ip.ihl * 4);

		if (packet != nullptr) {
			memcpy(packet, &_ip, sizeof _ip_hdr);
			memcpy(packet + sizeof _ip_hdr, &_icmp, sizeof _icmp_hdr);
		}


	}
	~ICMP_Pinger() {
		delete[] packet;
		closesocket(rawsock);
	}
	
	
	
	bool ping(uint32_t dst, uint32_t src) {
		ip->dst = dst;
		ip->src = src;
		reroll_Id();
		recalc_chksums();



		const int sent = sendL3(rawsock, packet, packetsize);
		if (sent < packetsize)
			return false;



		sockaddr_in from;
		int reclen = sizeof from,
			recvd = -2;


		char recvbuf[sizeof _ip_hdr + sizeof _icmp_hdr];

		_ip_hdr* t1;


		for (int i = 0; i < 4; i++) {
			recvd = recvfrom(rawsock, recvbuf, packetsize, 0, (sockaddr*)&from, &reclen);
			if (recvd == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAETIMEDOUT)
					return false;
				else
					return false;
			}



			t1 = reinterpret_cast<_ip_hdr*>(recvbuf);
			if (t1->proto == IPPROTO_ICMP && t1->src == from.sin_addr.S_un.S_addr) {
				if (icmp->rest == reinterpret_cast<_icmp_hdr*>(recvbuf + sizeof _ip_hdr)->rest) {
					return true;
				}
			}
		}

		return false;
	}


	bool ping(const char* dst, const char* src) {
		uint32_t dst32, src32;
		ip_to_uint32(dst32, dst);
		ip_to_uint32(src32, src);
		return ping(dst32, src32);
	}


};*/