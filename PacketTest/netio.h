#pragma once
#include <sys/socket.h>
#include <exception>
#include <arpa/inet.h>
#include "defs.h"
#include "ip_hdr.h"
#include "icmp_hdr.h"

SOCKET getRawSock(bool ip_hdrincl, int recvtimeout = 0) {
	int s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (s == INVALID_SOCKET) {
		return INVALID_SOCKET;
	}
	if (ip_hdrincl) {
		const int v = 1;
		setsockopt(s,IPPROTO_IP,IP_HDRINCL,&v,sizeof(v));
	}
	if (recvtimeout > 0) {
		const int v = recvtimeout;
		setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&v,sizeof(v));
	}
	return s;
}

int sendL3(SOCKET sock, const void* buffer, unsigned int buflen, int flags = 0, ADDRESS_FAMILY fam = AF_INET) {

	sockaddr_in dst;
	dst.sin_family = fam;
	dst.sin_addr.s_addr = ((_ip_hdr*)buffer)->dst;
	
	unsigned int hdr = 0,
				len = sizeof(hdr);

	getsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char*)&hdr, &len);
	if (hdr >= 0) {
		return sendto(sock, (const char*)buffer, buflen, flags, (sockaddr*)(&dst), sizeof(sockaddr_in));
	}
	throw std::runtime_error("not l3");
	return 0;
}
int sendL3(SOCKET sock, Raw buffer, int flags = 0, ADDRESS_FAMILY fam = AF_INET) {

	auto tup = buffer.getBuffer();
	std::shared_ptr<char> sh_buffer(std::get<char*>(tup));
	unsigned siz = std::get<unsigned>(tup);


	sendL3(sock,sh_buffer.get(),siz,0,fam);
}
