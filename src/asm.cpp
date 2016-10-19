#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

const string OPs[] = {"NOP", "ADD", "SUB", "MUL", "DIV", "SET", "PSH", "POP", "CMP",
	"AND", "AOR", "XOR", "JMP", "JIE", "JNE", "JIG", "JIL", "INT", "HLT"}; //all the supported OP codes

const unsigned char argument_size[] = {0, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0}; //parallel array for arguments

const int instructions_size = (const int)(sizeof(OPs) / sizeof(OPs[0]));

typedef struct{ //label saver
	string lbl;
	int offset;
}label;

int isNumber(string num){ //maybe add ###h support (hex)
	int ret = 0;
	if(num.length() == 0) return 65536; //return 2^16 error code because [0, 25565] are possible values
	else{
		for(unsigned int a = 0; a < num.length(); a++){
			if(num[a] < 58 && num[a] > 47){
				ret += ((int)num[a]-48) * pow(10, num.length()-1-a);
			}
			else return 65536;
		}
		return ret;
	}
	return 65536;
}
int isRegister(string reg){ //returns 2^16 if it isn't a register reference
	if(reg.length() > 2) return 65536;
	if(reg[0] == 'R'){
		if(reg[1] >= '0' && reg[1] < '8') return reg[1] - '0';
		return -1;
	}
	if(reg.compare("PC") == 0) return 0x8;
	if(reg.compare("SP") == 0) return 0x9;
	if(reg.compare("BP") == 0) return 0xA;
	if(reg.compare("SR") == 0) return 0xB;
	return 65536;
}
int main(int argc, char *argv[]){
	string path, current, program, arg[2];
	vector<label> labels;
	vector<label> cmds;
	int line = 0;
	bool found = false;
	if(argc == 3){ //a.sm src dst
		path = argv[1];
		cout << "Opening " << path << endl;
	}
	else{
		cout << "Input file name: ";
		cout.flush();
		cin >> path;
		cout << "Opening " << path << endl;
	}
	ifstream input(path);
	if(!input.is_open()){
		cout << "Failed to open " << path << endl;
		return -1;
	}
	while(!input.eof()){
		line++;
		input >> current;
		if(input.eof()) break;
		if(current[0] == '.'){ //label
			label l;
			l.offset = program.length();
			l.lbl = current.substr(1);
			labels.push_back(l);
			continue;
		}
		for(unsigned short a = 0; a < instructions_size; a++){
			if(current.compare(OPs[a]) == 0){ //we got an OP code match
				int words = 1;
				found = true;
				for(unsigned short b = 0; b < argument_size[a]; b++){
					input >> arg[b];
				}
				int op = (a << 11), opa = 0, opb = 0;
				if(argument_size[a] > 0){
					if((opa = isNumber(arg[0])) != 65536){ //#+
						op |= (0x03E0);
						words = 2;
					}
					else if((opa = isRegister(arg[0])) != 65536){
						if(opa < 0x8) op |= (opa << 5);
						else op |= (opa+15) << 5;
					}
					else if(arg[0][arg[0].length() - 1] == ']'){ //???+]
						if(arg[0][0] == '['){ //[??+]
							if((opa = isRegister(arg[0].substr(1, 2))) != 65536){ //[R#]
								if(opa < 0x8) op |= ((opa+8) << 5);
								else op |= ((opa+15) << 5);
							}
							else{ //maybe literal value, not supported
								cout << "unknown meaning " << arg[0] << endl;
								return 1;
							}
						}
						else if(arg[0][arg[0].length()-4] == '['){ //??+[??]
							if((opa = isNumber(arg[0].substr(0, arg[0].length()-4))) != 65536){ //##[??]
								unsigned short tmp;
								if((tmp = isRegister(arg[0].substr(arg[0].length()-3, 2))) != 65536){
									if(tmp < 8) op |= ((tmp+0x10) << 5);
									else if(tmp == 0xA) op |= (0x1B << 5);
									else cout << "wtf" << endl;
								}
							}
							else{
								cout << "unknown meaning " << arg[0] << endl;
								return 1;
							}
						}
						else{
							cout << "uknown meaning " << arg[0] << endl;
							return 1;
						}
					}
					else{ //assume label
						words = 2;
						label cm;
						cm.offset = program.length() + 2;
						op |= 0x03E0;
						cm.lbl = arg[0];
						cmds.push_back(cm);
					}
				}
				if(argument_size[a] == 2){
					if((opb = isNumber(arg[1])) != 65536){ //#+
						op |= (0x1F);
						words = 3;
					}
					else if((opb = isRegister(arg[1])) != 65536){
						if(opb < 0x8) op |= (opb);
						else op |= (opb+15);
					}
					else if(arg[1][arg[1].length() - 1] == ']'){ //???+]
						if(arg[1][1] == '['){ //[??+]
							if((opb = isRegister(arg[1].substr(1, 2))) != 65536){ //[R#]
								if(opb < 0x8) op |= ((opb+8));
								else op |= ((opb+15));
							}
							else{//maybe [##] ? not supported yet
								cout << "unknown meaning " << arg[1] << endl;
								return 1;
							}
						}
						else if(arg[1][arg[1].length()-4] == '['){ //??+[??]
							if((opb = isNumber(arg[1].substr(0, arg[1].length()-4))) != 65536){ //##[??]
								unsigned short tmp;
								if((tmp = isRegister(arg[1].substr(arg[1].length()-3, 2))) != -1){
									if(tmp < 8) op |= ((tmp+0x10));
									else if(tmp == 0xA) op |= (0x1B);
									else cout << "wtf" << endl;
								}
							}
							else{
								cout << "unknown meaning " << arg[1] << endl;
								return 1;
							}
						}
					}
					else{ //assume label if not op
						words = 3;
						label cm;
						cm.offset = program.length() + 4;
						op |= 0x1F;
						cm.lbl = arg[1];
						cmds.push_back(cm);
					}
				}
				program+=(char)(op >> 8); //save program to a string
				program+=(char)(op & 0xFF);
				if(words > 1){ //can be size 2 or 3
					program+=(char)(opa >> 8);
					program+=(char)(opa & 0xFF);
				}
				if(words == 3){//can only be size 3
					program+=(char)(opb >> 8);
					program+=(char)(opb & 0xFF);
				}
				break;
			}
		}
		if(!found){
			cout << "Unknown symbol " << current << " at line " << line << endl;
			return 1;
		}
		found = false;
	}
	for(unsigned int a = 0; a < cmds.size(); a++){ //reattach labels after full pass
		found = false;
		for(unsigned int b = 0; b < labels.size(); b++){
			if(cmds[a].lbl.compare(labels[b].lbl) == 0){ //found label
				program[cmds[a].offset] = (labels[b].offset >> 8);
				program[cmds[a].offset+1] = (labels[b].offset & 0xFF);
				found = true;
			}
		}
		if(!found){
			cout << "unknown label " << cmds[a].lbl << endl;
			return 1;
		}
	}
	ofstream output(argv[2]);
	output << program;
	output.close();
	input.close(); //you've made it.
	return 0;
}
