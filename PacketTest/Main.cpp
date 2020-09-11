#include <iostream>
#include "IP_Header.h"
#include "ICMP_Header.h"

using namespace std;

typedef unsigned int uint;

string toaddr(ULONG addr) {

	char f[16];
	inet_ntop(AF_INET, &addr, f, 16);
	string a = f;
	return a;

}


void scanNetwork() {
	uint32_t privIP, netmask;
	ip_to_uint32(privIP,"192.168.178.31");
	ip_to_uint32(netmask,"255.255.255.0");

	ULONG start = privIP & netmask;
	ULONG end = privIP | ~netmask;


	for (ULONG i = htonl(start); i <= htonl(end); i++) {
		cout << toaddr(ntohl(i)) << endl;
	}
	cout << endl << endl;
}


/*
TODO: Wrapper class for IP/ICMP Header to recalc checksum after every change?
raw sockets lol



*/

int main() {

	IP_Header a;
	populateHeaderDefault(&a);

	a.total_len += htons(sizeof ICMP_Header);
	a.proto = IPPROTO_ICMP;
	a.header_chksum = 0x0000;
	a.header_chksum = header_checksum(&a, a.ihl * 4);


	ICMP_Header i;
	populateICMPDefault(&i);



	cout << endl;




	cout << "----------IP Header----------" << endl;
	displayHeader(&a);
	display(&a, sizeof IP_Header);
	cout << endl;
	cout << "---------ICMP Header---------" << endl;

	displayICMP(&i);
	display(&i, sizeof ICMP_Header);




	const UINT packsize = sizeof IP_Header + sizeof ICMP_Header;
	char packet[packsize];

	memcpy(packet, &a, sizeof IP_Header);
	memcpy(packet + sizeof IP_Header, &i, sizeof ICMP_Header);

	cout << endl;
	cout << "---------IP + ICMP---------" << endl;
	display(packet, packsize);






	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);


	SOCKET s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (s == INVALID_SOCKET) {
		cout << "Invalid socket " << WSAGetLastError() << endl;
		return 0;
	}


	char buffer[2048];










	WSACleanup();








	return 0;
}