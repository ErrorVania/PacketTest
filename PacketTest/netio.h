#pragma once
#include <tuple>
#include "general.h"
#include "ip_hdr.h"
#include "icmp_hdr.h"
#pragma comment(lib,"Ws2_32.lib")

SOCKET getRawSock(bool ip_hdrincl, int recvtimeout = 0) {

	WSADATA wsaData;
	SOCKET s;
	const int wsaret = WSAStartup(MAKEWORD(2, 2), &wsaData);


	if (wsaret != 0) {
		std::cout << wsaret << std::endl << WSAGetLastError() << std::endl;
		return 1;
	}
	else {
		s = socket(AF_INET, SOCK_RAW, 0);
		if (s == INVALID_SOCKET) {
			std::cout << "Invalid socket " << WSAGetLastError() << std::endl;
			return 2;
		}
		if (ip_hdrincl) {
			const int optval = 1;
			setsockopt(s, IPPROTO_IP, IP_HDRINCL, (const char*)&optval, sizeof optval); //define IP Header ourselves
		}
		if (recvtimeout > 0) {
			int t = recvtimeout;
			setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&t, sizeof t);

		}
	}
	return s;
}

int sendL3(SOCKET sock, const void* buffer, unsigned int buflen, int flags = 0, ADDRESS_FAMILY fam = AF_INET) {

	sockaddr_in dst;
	dst.sin_family = fam;
	dst.sin_addr.S_un.S_addr = ((_ip_hdr*)buffer)->dst;

	DWORD hdr = 0;
	int len = sizeof hdr;

	getsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char*)&hdr, &len);
	if (hdr >= 0) {
		return sendto(sock, (const char*)buffer, buflen, flags, (sockaddr*)(&dst), sizeof sockaddr_in);
	}
	throw std::exception("not L3");
	return 0;
}




bool icmp_ping(SOCKET s,uint32_t dst, uint32_t src) {
	ip_hdr p1;
	icmp_hdr p2;

	p1.p_hdr->src = src;
	p1.p_hdr->dst = dst;

	std::tuple<char*, unsigned> tup = (p1 + p2).getBuffer();
	char* snd_buf = std::get<char*>(tup);
	unsigned snd_siz = std::get<unsigned>(tup);

	if (sendL3(s, snd_buf, snd_siz) != snd_siz) {
		std::cerr << "sendL3 return less than buffer size" << std::endl;
	}

	const int bufsize = 1500;
	std::shared_ptr<char> recbuffer((char*)malloc(bufsize), free);
	sockaddr_in from;
	int reclen = sizeof from;

	for (int i = 0; i < 4; i++) {
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
			display(ipresp, ipresp->ihl * 4);

			if (ipresp->proto == IPPROTO_ICMP) {
				_icmp_hdr* icmpresp = reinterpret_cast<_icmp_hdr*>(recbuffer.get() + ipresp->ihl * 4);
				display(icmpresp, sizeof _icmp_hdr);

				return icmpresp->Code == 0 && icmpresp->Type == 0;
			}
		}
	}
}