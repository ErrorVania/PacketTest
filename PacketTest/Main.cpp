#include <iostream>
#include "hdisplay.h"
#include "netio.h"
//#include "Functions/icmp_ping.h"

using std::cout;
using std::endl;

char* toaddr(uint32_t v) {
	in_addr i;
	i.s_addr = v;
	return inet_ntoa(i);
}

int main() {
	srand(static_cast<unsigned>(time(NULL)));

	SOCKET s = getRawSock(true, 1000);

	cout << "Pinging network..." << endl;

	uint32_t privIP = inet_addr("192.168.178.31");
	uint32_t netmask = inet_addr("255.255.255.0");

	uint32_t start = privIP & netmask;
	uint32_t end = privIP | ~netmask;

	
	for (uint32_t i = htonl(start); i <= htonl(end); i++) {
		//cout << toaddr(ntohl(i)) << " " << icmp_ping(s, ntohl(i), privIP, 1) << endl;
	}
	cout << endl << endl;










	return 0;
}