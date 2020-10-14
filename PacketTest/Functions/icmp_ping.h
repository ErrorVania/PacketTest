#include "../Raw.h"
#include "../ip_hdr.h"
#include "../icmp_hdr.h"

#include <arpa/inet.h>
#include "../defs.h"

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