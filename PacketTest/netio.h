#pragma once
#include <tuple>
#include <sys/socket.h>
#include <exception>
#include "general.h"
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
	// dst.sin_addr.S_un.S_addr = ((_ip_hdr*)buffer)->dst;
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





bool icmp_ping(SOCKET s, uint32_t dst, uint32_t src, int retries = 4) {
	ip_hdr p1;
	icmp_hdr p2;

	p1.p_hdr->src = src;
	p1.p_hdr->dst = dst;

	Raw f = p1 + p2;

	if (sendL3(s, f) != f.getraw().size()) {
		std::cerr << "sendL3 return less than buffer size" << std::endl;
	}

	const int bufsize = 1500;
	std::shared_ptr<char> recbuffer((char*)malloc(bufsize), free);
	sockaddr_in from;
	int reclen = sizeof from;

	for (int i = 0; i < retries; i++) {
		const int recvd = recvfrom(s, recbuffer.get(), bufsize, 0, (sockaddr*)&from, &reclen);
		if (recvd == SOCKET_ERROR) {
			int error = WSAGetLastError();
			if (error == WSAETIMEDOUT) {
				//return false;
				continue;
			}
			else {
				throw std::exception("icmp_ping socket_error != timeout");
			}
		}

		if (recvd > bufsize) {
			throw std::exception("icmp_ping received jumbo pong?");
		}


		if (from.sin_addr.S_un.S_addr == p1.getHeader()->dst) {
			//assume we recieved ip + icmp stack
			_ip_hdr* ipresp = reinterpret_cast<_ip_hdr*>(recbuffer.get());

			if (ipresp->proto == IPPROTO_ICMP) {
				_icmp_hdr* icmpresp = reinterpret_cast<_icmp_hdr*>(recbuffer.get() + ipresp->ihl * 4);

				return icmpresp->Code == 0 && icmpresp->Type == 0;
			}
		}
	}
}
