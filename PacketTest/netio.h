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
int sendL3(SOCKET sock, Raw buffer, int flags = 0, ADDRESS_FAMILY fam = AF_INET) {



	auto tup = buffer.getBuffer();
	std::shared_ptr<char> sh_buffer(std::get<char*>(tup));
	unsigned siz = std::get<unsigned>(tup);


	sockaddr_in dst;
	dst.sin_family = fam;
	dst.sin_addr.S_un.S_addr = ((_ip_hdr*)sh_buffer.get())->dst;

	DWORD hdr = 0;
	int len = sizeof hdr;

	getsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char*)&hdr, &len);
	if (hdr >= 0) {
		return sendto(sock, sh_buffer.get(), siz, flags, (sockaddr*)(&dst), sizeof sockaddr_in);
	}
	throw std::exception("not L3");
	return 0;
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




//WIP
std::tuple<uint32_t, uint32_t> getIPInfo() {
	SOCKET s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	sockaddr_in dst;
	dst.sin_family = AF_INET;
	dst.sin_addr.S_un.S_addr = toip("");
	icmp_hdr i;

	sendto(s, (const char*)i.p_hdr, sizeof _icmp_hdr, 0,(sockaddr*)(&dst), sizeof sockaddr_in);






	return std::make_tuple(0, 0);
}