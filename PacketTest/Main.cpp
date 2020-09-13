#include <iostream>
#include "general.h"
#include "Headers.h"

using namespace std;


inline string toaddr(ULONG addr) {

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








	//WSA start

	WSADATA wsaData;
	SOCKET s;
	const int optval = 1,
			  wsaret = WSAStartup(MAKEWORD(2, 2), &wsaData);


	if (wsaret != 0) {
		cout << wsaret << endl << WSAGetLastError() << endl;
		return 1;
	} else {
		s = socket(AF_INET, SOCK_RAW, 0);
		if (s == INVALID_SOCKET) {
			cout << "Invalid socket " << WSAGetLastError() << endl;
			return 2;
		}
		setsockopt(s, IPPROTO_IP, IP_HDRINCL, (const char*)&optval, sizeof optval); //define IP Header ourselves
	}




	//Sending the packet
	cout << endl << "Sending packet..." << endl;
	sockaddr_in dst;
	dst.sin_family = AF_INET;
	dst.sin_addr.S_un.S_addr = ip.dst;


	const int sent = sendto(s, packet, packsize, 0, (sockaddr*)(&dst), sizeof sockaddr_in);
	if (sent < packsize) {
		cout << "sendto error: " << sent << " WSA: " << WSAGetLastError();
	}
	cout << dec << "Sent " << sent << " bytes" << endl;



	//getting response
	const int bufsize = sizeof IP_Header + sizeof ICMP_Header;
	char recbuf[bufsize];
	sockaddr_in from;
	int reclen = sizeof from;


	const int recvd = recvfrom(s, recbuf, bufsize, 0, (sockaddr*)&from, &reclen);
	if (recvd < 0) {
		cout << "recvfrom error: " << recvd << " WSA: " << WSAGetLastError();
		return recvd;
	}

	cout << dec << "Received " << recvd << " bytes" << endl;
	display(recbuf, bufsize);



	//splitting buffer into IP and ICMP Headers

	IP_Header* ip2 = (IP_Header*)recbuf;
	ICMP_Header* icmp2 = (ICMP_Header*)(recbuf + sizeof IP_Header);

	cout << *ip2 << endl << *icmp2 << endl;


	WSACleanup();
	return 0;
}