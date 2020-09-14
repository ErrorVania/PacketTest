#pragma once
#include "general.h"
#include "Headers.h"







inline SOCKET getRawSock() {

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
		setsockopt(s, IPPROTO_IP, IP_HDRINCL, (const char*)&optval, sizeof optval); //define IP Header ourselves
	}
	return s;
}




const int sendL3(SOCKET sock, const char* buffer, unsigned int buflen, int flags = 0, ADDRESS_FAMILY fam = AF_INET) {
	
	sockaddr_in dst;
	dst.sin_family = fam;
	dst.sin_addr.S_un.S_addr = ((IP_Header*)buffer)->dst;

	return sendto(sock, buffer, buflen, flags, (sockaddr*)(&dst), sizeof sockaddr_in);
}





class ICMP_Pinger {
private:
	const unsigned packetsize = sizeof IP_Header + sizeof ICMP_Header;
	char* packet = nullptr;
	SOCKET rawsock = INVALID_SOCKET;
	IP_Header* ip = nullptr;
	ICMP_Header* icmp = nullptr;

	void reroll_Id() {
		icmp->rest = htons(rand() % 0xffff);
	}
	void recalc_chksums() {
		ip->header_chksum = 0x0000;
		icmp->Checksum = 0x0000;
		ip->header_chksum = header_checksum(ip, ip->ihl * 4);
		icmp->Checksum = header_checksum(icmp, sizeof ICMP_Header);
	}

public:

	ICMP_Pinger() {
		packet = static_cast<char*>(malloc(packetsize));
		rawsock = getRawSock();
		const uint32_t timeout = 1000;
		setsockopt(rawsock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);


		ip = (IP_Header*)packet;
		icmp = (ICMP_Header*)(packet + sizeof IP_Header);



		IP_Header _ip;
		ICMP_Header _icmp;

		_ip.dst = 0x00000000;
		_ip.src = 0x00000000;

		_ip.total_len += htons(sizeof ICMP_Header);
		_ip.proto = IPPROTO_ICMP;
		_ip.header_chksum = 0x0000;
		_ip.header_chksum = header_checksum(&_ip, _ip.ihl * 4);

		memcpy(packet, &_ip, sizeof IP_Header);
		memcpy(packet + sizeof IP_Header, &_icmp, sizeof ICMP_Header);



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


		uint32_t icmp_id = 0;

		sockaddr_in from;
		int reclen = sizeof from;


		int recvd = -2;

		while (icmp_id != icmp->rest) {
			recvd = recvfrom(rawsock, packet, packetsize, 0, (sockaddr*)&from, &reclen);
			if (recvd == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAETIMEDOUT)
					return false;
				else
					return false;
			}
			icmp_id = ((ICMP_Header*)(packet + sizeof IP_Header))->rest;
		}

		return true;
}
	bool ping(const char* dst, const char* src) {
		uint32_t dst32, src32;
		ip_to_uint32(dst32, dst);
		ip_to_uint32(src32, src);
		return ping(dst32, src32);
	}


};