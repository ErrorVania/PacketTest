#include <iostream>
#include "general.h"
#include "netio.h"
#include "ip_hdr.h"
#include "icmp_hdr.h"


using std::cout;

inline const std::string toaddr(ULONG addr) {

	char f[16];
	inet_ntop(AF_INET, &addr, f, 16);
	std::string a = f;
	return a;

}




int main() {
	srand(static_cast<unsigned>(time(NULL)));
	//combine IP and ICMP Headers
	ip_hdr iphdr;
	icmp_hdr icmphdr;

	//_ip_hdr* i = iphdr.getHeader();

	iphdr.p_hdr->total_len += htons(sizeof _icmp_hdr);
	iphdr.p_hdr->proto = IPPROTO_ICMP;
	iphdr.p_hdr->ttl = 64;
	iphdr.p_hdr->src = ip("192.168.178.31");
	iphdr.p_hdr->dst = ip("192.168.178.1");

	iphdr.p_hdr->header_chksum = 0;
	iphdr.p_hdr->header_chksum = header_checksum(iphdr.p_hdr, iphdr.p_hdr->ihl * 4);

	cout << iphdr << endl << icmphdr << endl;



	std::tuple<char*,unsigned> tup = (iphdr + icmphdr).getBuffer();

	auto buff = std::shared_ptr<char>(std::get<char*>(tup),free);
	const unsigned siz = std::get<unsigned>(tup);

	




	cout << "Generated Packet:" << endl;
	display(buff.get(), siz);

	SOCKET s = getRawSock(true);

	//Sending the packet
	cout << endl << "Sending packet..." << endl;
	const int sent = sendL3(s, buff.get(), siz);
	if (sent < siz)
		cout << "sendto error: " << sent << " WSA: " << WSAGetLastError();
	else
		cout << std::dec << "Sent " << sent << " bytes" << endl;
	cout << endl;


	//getting response
	const int bufsize = sizeof _ip_hdr + sizeof _icmp_hdr;
	char recbuf[bufsize];
	sockaddr_in from;
	int reclen = sizeof from;


	int recvd = -2;
	while (recvd < 0) {
		recvd = recvfrom(s, recbuf, bufsize, 0, (sockaddr*)&from, &reclen);
	}
	cout << std::dec << "Received " << recvd << " bytes" << endl;
	display(recbuf, bufsize);


	//splitting buffer into IP and ICMP Headers

	_ip_hdr* ip2 = (_ip_hdr*)recbuf;
	_icmp_hdr* icmp2 = (_icmp_hdr*)(recbuf + sizeof _ip_hdr);

	//cout << *ip2 << endl << *icmp2 << endl;


	

	//this works! Let's scan the network...
	/*ICMP_Pinger pinger;
	cout << "Pinging network..." << endl;

	uint32_t privIP, netmask;
	ip_to_uint32(privIP, "192.168.178.31");
	ip_to_uint32(netmask, "255.255.255.0");

	ULONG start = privIP & netmask;
	ULONG end = privIP | ~netmask;


	for (ULONG i = htonl(start); i <= htonl(end); i++) {
		//cout << toaddr(ntohl(i)) << ": " << pinger.ping(ntohl(i), privIP) << endl;
		if (pinger.ping(ntohl(i), privIP)) 
			cout << toaddr(ntohl(i)) << " responded." << endl;
	}
	cout << endl << endl;*/




	return WSACleanup();
}