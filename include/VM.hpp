#ifndef VM_HPP
#define VM_HPP
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;
class VM{
	public:
	VM();
	void initialize();
	void bootCode(string path);
	void start();
	private:
	unsigned short r[8], PC, SP, BP, SR;
	unsigned short memory[65536];
	unsigned char instructions[4096];//harvard architecture with code seperate from data
	//registers/RAM
};
#endif
