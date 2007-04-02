#include <edelib/String.h>
#include "timer.hpp"
#include <stdlib.h>
#include <iostream>

template <typename T>
void test_str(int repeat, int loop)
{
	double result = 0;
	double total = 0;
	boost::timer tim;
	boost::timer tt;

	T s;
	T stmp1, stmp2;

	std::cout << " operator+=   : ";
	tt.restart();
	for(int i = 0; i < repeat; i++) {
		tim.restart();
		for(int j = 0; j < loop; j++) {
			s += "sample string with no allocation";
		}
		result += tim.elapsed();
	}
	total = tt.elapsed();
	std::cout << "total = " << total << " average = " << result / repeat << std::endl;

	s.clear();
	std::cout << " operator=(1) : ";
	tt.restart();
	for(int i = 0; i < repeat; i++) {
		tim.restart();
		for(int j = 0; j < loop; j++) {
			s = "sample string with no allocation";
		}
		result += tim.elapsed();
	}
	total = tt.elapsed();
	std::cout << "total = " << total << " average = " << result / repeat << std::endl;

	s.clear();
	std::cout << " operator=(2) : ";
	tt.restart();
	for(int i = 0; i < repeat; i++) {
		tim.restart();
		for(int j = 0; j < loop; j++) {
			s = "sample string with no allocation";
			s = "sample short string";
			s = "sample short string that will require allocation again";
			s = "sample short string that will require allocation again sample short string that will require allocation";
			s = "sample";
			s = "sample short string that will require allocation again sample short string that will require allocation";

			stmp1 = s;
			stmp2 = s;
			stmp1 = stmp2;
		}
		result += tim.elapsed();
	}
	total = tt.elapsed();
	std::cout << "total = " << total << " average = " << result / repeat << std::endl;

	s.clear();
	std::cout << " operator+(1) : ";
	tt.restart();
	for(int i = 0; i < repeat; i++) {
		tim.restart();
		for(int j = 0; j < loop; j++) {
			s = s + "sample string with no allocation";
		}
		result += tim.elapsed();
	}
	total = tt.elapsed();
	std::cout << "total = " << total << " average = " << result / repeat << std::endl;

	s.clear();
	std::cout << " operator+(2) : ";
	tt.restart();
	for(int i = 0; i < repeat; i++) {
		tim.restart();
		for(int j = 0; j < loop; j++) {
			s = s + "sample string with no allocation";
			s = s + "sample";
			s = s + "sample string with no allocation";
			s = s + "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		}
		result += tim.elapsed();
	}
	total = tt.elapsed();
	std::cout << "total = " << total << " average = " << result / repeat << std::endl;
}

int main(int argc, char** argv)
{
	//int repeat = 10;
	//int loops = 100;
	int repeat = atoi(argv[1]);
	int loops = atoi(argv[2]);

	std::cout << "repeat: " << repeat << " loops: " << loops << "\n";
	std::cout << "-------------------------------------------------\n";
	std::cout << "Test for edelib::String" << std::endl;
	test_str<edelib::String>(repeat, loops);

	std::cout << std::endl;

	std::cout << "Test for std::string" << std::endl;
	test_str<std::string>(repeat, loops);
	return 0;
}
