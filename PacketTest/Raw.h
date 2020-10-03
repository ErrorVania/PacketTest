#pragma once
#include "general.h"
#include <tuple>
#include <vector>


class Raw {


protected:
	virtual void writeData(std::vector<char>& t) {
		t.push_back(0);
	}

	std::vector<char> data;

public:

	Raw() {

	}

	std::vector<char>& getraw() {
		return data;
	}

	Raw operator+(Raw& other) {
		std::vector<char> a, b;
		this->writeData(a);
		other.writeData(b);


		data.reserve(a.size() + b.size());
		data.insert(data.end(), a.begin(), a.end());
		data.insert(data.end(), b.begin(), b.end());

		Raw p;
		p.data = data;

		return p;

	}



	std::tuple<char*, unsigned> getBuffer() {
		char* b = (char*)malloc(data.size());
		if (b != nullptr) {
			memcpy(b, data.data(), data.size());
		}


		return std::make_tuple(b,data.size());
	}

};