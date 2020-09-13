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




bool icmp_ping(SOCKET rawsock,uint32_t src, uint32_t dst) {
	IP_Header ip;
	ICMP_Header icmp;

	ip.dst = dst;
	ip.src = src;

	ip.total_len += htons(sizeof ICMP_Header);
	ip.proto = IPPROTO_ICMP;
	ip.header_chksum = 0x0000;
	ip.header_chksum = header_checksum(&ip, ip.ihl * 4);




	const unsigned packsize = sizeof IP_Header + sizeof ICMP_Header;
	char packet[packsize];

	memcpy(packet, &ip, sizeof IP_Header);
	memcpy(packet + sizeof IP_Header, &icmp, sizeof ICMP_Header);


	const int sent = sendL3(rawsock, packet, packsize);
	if (sent < packsize)
		return false;
	

	uint32_t icmp_id = 0;






	//char recbuf[packsize];
	sockaddr_in from;
	int reclen = sizeof from;


	int recvd = -2;
	unsigned limit = 0;

	uint8_t type = 0xff;
	for (limit = 0xffffffff; icmp_id != icmp.rest && limit <= 0; limit--) {
		recvd = recvfrom(rawsock, packet, packsize, 0, (sockaddr*)&from, &reclen);
		icmp_id = ((ICMP_Header*)(packet + sizeof IP_Header))->rest;
		type = ((ICMP_Header*)(packet + sizeof IP_Header))->Type;
	}

	if (limit > 0 && limit == 0)
		return true;

	return false;
}