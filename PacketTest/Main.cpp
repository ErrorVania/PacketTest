#include <iostream>
#include "general.h"
#include "netio.h"
#include "ip_hdr.h"
#include "icmp_hdr.h"

using std::cout;
using std::endl;


int main() {
	srand(static_cast<unsigned>(time(NULL)));

	SOCKET s = getRawSock(true, 1000);

	cout << "Pinging network..." << endl;

	uint32_t privIP = toip("192.168.178.31");
	uint32_t netmask = toip("255.255.255.0");

	ULONG start = privIP & netmask;
	ULONG end = privIP | ~netmask;


	for (ULONG i = htonl(start); i <= htonl(end); i++) {
		cout << toaddr(ntohl(i)) << " " << icmp_ping(s, ntohl(i), privIP, 1) << endl;
	}
	cout << endl << endl;










	return WSACleanup();
}