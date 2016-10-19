#include <iostream>
#include <string>
#include "VM.hpp"

using namespace std;

int main(int argv, char **argc){
	string path;
	if(argv != 2)
		cin >> path;
	else
		path = argc[1];
	VM machine;
	machine.initialize();
	machine.bootCode(path);
	machine.start();
	return 0;
}
