#pragma once

#include <iostream>
#include <iomanip>

inline bool isPrintable(unsigned char c) {
	if (c == 0x00)
		return false;
	auto b = static_cast<signed char>(c);
	return isprint(c);
}


//right display is broken, displays wrong bytes
void display(const void* object, size_t siz, const unsigned offs = 8)
{
	auto bytes = static_cast<const uint8_t*>(object);
	size_t i;

	uint8_t* buf = (uint8_t*)malloc(offs);

	std::cout << std::hex << std::setfill('0');
	for (i = 0; i < siz; i++) {

		if (i % offs == 0) {
			if (i > 0) {
				std::cout << "| ";
				/*memcpy(buf, bytes + i - offs, offs);
				for (int x = 0; x < offs; x++) {
					if (isPrintable(buf[x]) != 0) {
						cout << buf[x];
					}
					else {
						cout << ".";
					}
				}*/
			}
			std::cout << std::endl << "0x" << std::setw(offs) << i << " | ";
		}
		std::cout << std::setw(2) << (unsigned)bytes[i] << " ";
	}


	std::cout << std::setw(1);
	size_t backup = i;
	while (i % offs != 0) {
		std::cout << "   ";
		i++;
	}
	std::cout << "| ";
	//memcpy(buf, bytes + siz - offs, offs);

	unsigned t = backup % offs;
	t = t == 0 ? t = 8 : backup % offs;


	/*for (unsigned x = 0; x < t; x++) {

		if (isPrintable(buf[x])) {
			cout << buf[x];
		}
		else {
			cout << ".";
		}
	}*/

	free(buf);
	std::cout << std::endl;
}