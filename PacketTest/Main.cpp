#include <iostream>
#include "general.h"
#include "Headers.h"
#include "netio.h"

using namespace std;

inline string toaddr(ULONG addr) {

	char f[16];
	inet_ntop(AF_INET, &addr, f, 16);
	string a = f;
	return a;

}




int main() {
	srand(time(NULL));
	//combine IP and ICMP Headers
	IP_Header ip;
	ICMP_Header icmp;

	ip.total_len += htons(sizeof ICMP_Header);
	ip.proto = IPPROTO_ICMP;
	ip.header_chksum = 0x0000;
	ip.header_chksum = header_checksum(&ip, ip.ihl * 4);




	const unsigned packsize = sizeof IP_Header + sizeof ICMP_Header; //28
	char packet[packsize];

	memcpy(packet, &ip, sizeof IP_Header);
	memcpy(packet + sizeof IP_Header, &icmp, sizeof ICMP_Header);



	SOCKET s = getRawSock();

	//Sending the packet
	cout << endl << "Sending packet..." << endl;
	const int sent = sendL3(s, packet, packsize);
	if (sent < packsize)
		cout << "sendto error: " << sent << " WSA: " << WSAGetLastError();
	else
		cout << dec << "Sent " << sent << " bytes" << endl;








	//getting response
	const int bufsize = sizeof IP_Header + sizeof ICMP_Header;
	char recbuf[bufsize];
	sockaddr_in from;
	int reclen = sizeof from;


	int recvd = -2;
	while (recvd < 0) {
		recvd = recvfrom(s, recbuf, bufsize, 0, (sockaddr*)&from, &reclen);
	}
	cout << dec << "Received " << recvd << " bytes" << endl;
	display(recbuf, bufsize);


	//splitting buffer into IP and ICMP Headers

	IP_Header* ip2 = (IP_Header*)recbuf;
	ICMP_Header* icmp2 = (ICMP_Header*)(recbuf + sizeof IP_Header);

	cout << *ip2 << endl << *icmp2 << endl;



	//this works! Let's scan the network...
	ICMP_Pinger pinger;
	cout << "Pinging network..." << endl;

	uint32_t privIP, netmask;
	ip_to_uint32(privIP, "192.168.178.31");
	ip_to_uint32(netmask, "255.255.255.0");

	ULONG start = privIP & netmask;
	ULONG end = privIP | ~netmask;


	for (ULONG i = htonl(start); i <= htonl(end); i++) {
		cout << toaddr(ntohl(i)) << ": " << pinger.ping(ntohl(i),privIP) << endl;
	}
	cout << endl << endl;







	WSACleanup();
	return 0;
}