#include <VM.hpp>

VM::VM(){

}

void VM::initialize(){
	PC = 0; //set registers to 0
	SP = 0;
	BP = 0;
	SR = 1;
}

void VM::bootCode(string code){
	ifstream input(code, ios::binary);
	if(!input.is_open()){
		cout << "failed to read input file" << endl;
		return;
	}
	auto fsize = input.tellg(); //check ROM size
	input.seekg(0, ios::end);
	fsize = input.tellg() - fsize;
	input.seekg(0);
	if(fsize > 4096){
		cout << "filesize overflow" << endl;
	}
	input.read((char*)&instructions[0], 4096);
	input.close();
}
short translateInstruction(unsigned char a, unsigned char b){ 
	//fix this in the future (assembler must save things in big endian mode)
	short byte = (a << 8); //flip byte order
	return byte + b;
}
void VM::start(){
	unsigned short words[3];
	unsigned short a, b, *aptr, *bptr, size = 1;
	//using aptr and bptr allows the switch statements to be much simpler
	while(1){
		size = 1; //read 3 words from instructions just in case
		words[0] = translateInstruction(instructions[PC], instructions[PC+1]);
		words[1] = translateInstruction(instructions[PC+2], instructions[PC+3]);
		words[2] = translateInstruction(instructions[PC+4], instructions[PC+5]);
		if((SR & 0x0001) == 0x0001){ //core 1 is on
			char op = ((words[0] & 0xF800) >> 11); //get bits 0-4
			a = ((words[0] & 0x03E0) >> 5); //get bits 6-10
			b = ((words[0] & 0x001F)); //get bits 11-15
			if(a < 0x08) aptr = &r[a]; //register a
			else if(a < 0x10) aptr = &memory[r[a-0x08]]; //memory[register a]
			else if(a < 0x18){ //memory[register a + next word]
				aptr = &memory[r[a-0x10] + words[1]];
				size = 2; //this OP needs 2 words
			}
			else if(a == 0x18) aptr = &PC; //special registers
			else if(a == 0x19) aptr = &SP;
			else if(a == 0x1A) aptr = &BP;
			else if(a == 0x1B) aptr = &SR;
			else if(a == 0x1C){ //memory[BP + next word]
				aptr = &memory[BP+words[1]];
				size = 2;
			}
			else if(a == 0x1F){ //literal value in next word
				aptr = &words[1];
				size = 2;
			}
			if(b < 0x08) bptr = &r[b]; //do the same for b from a
			else if(b < 0x10) bptr = &memory[r[b-0x08]];
			else if(b < 0x18){
				bptr = &memory[r[b-0x10] + words[2]];
				size = 3;
			}
			else if(b == 0x18) bptr = &PC; //special registers
			else if(b == 0x19) bptr = &SP;
			else if(b == 0x1A) bptr = &BP;
			else if(b == 0x1B) bptr = &SR;
			else if(b == 0x1C){
				bptr = &memory[BP+words[2]];
				size = 3;
			}
			else if(b == 0x1F){
				bptr = &words[2];
				size = 3;
			}
			switch(op){
				case 0x00: //nop
				break;
				case 0x01: //add
					*aptr += *bptr;
				break;
				case 0x02: //sub
					*aptr -= *bptr;
				break;
				case 0x03: //mul
					*aptr *= *bptr;
				break;
				case 0x04: //div
					*aptr /= *bptr;
				break;
				case 0x05: //set
					*aptr = *bptr;
				break;
				case 0x06: //psh
					memory[SP] = *aptr;
					SP+=2;
				break;
				case 0x07: //pop
					*aptr = memory[SP-2];
					SP-=2;
				break;
				case 0x08: //cmp
					SR &= 0xFFC1;
					if(*aptr == *bptr) SR |= 0x2;
					else if(*aptr > *bptr) SR |= 0x4;
					else if(*aptr < *bptr) SR |= 0x8;
				break;
				case 0x09: //and
					*aptr &= *bptr;
				break;
				case 0x0A: //aor
					*aptr |= *bptr;
				break;
				case 0x0B: //xor
					*aptr ^= *bptr;
				break;
				case 0x0C: //jmp
					PC = *aptr - (size*2);
				break;
				case 0x0D: //jie
					if((SR&0x2) == 0x2) PC = *aptr - (size*2);
				break;
				case 0x0E: //jne
					if((SR&0x2) != 0x2) PC = *aptr - (size*2);
				break;
				case 0x0F: //jig
					if((SR&0x4) == 0x4) PC = *aptr - (size*2);
				break;
				case 0x10: //jil
					if((SR&0x8) == 0x8) PC = *aptr - (size*2);
				break;
				case 0x11: //int
					//interrupt with code a, not implemented
				break;
				case 0x12: //hlt
					SR &= 0xFFFE; //turn off 'running' bit
				break;
				case 0x13: //the rest are still unused
				case 0x14:
				case 0x15:
				case 0x16:
				case 0x17:
				case 0x18:
				case 0x19:
				case 0x1A:
				case 0x1B:
				case 0x1C:
				case 0x1D:
				case 0x1E:
				case 0x1F:
				break;
			} 
			PC+=(size*2);
		}
		if((SR & 0x1) == 0) break;
		if(memory[65535] != 0){ //this is output to the console
			cout << (char)memory[65535]; //if its non-zero print to console and reset
			memory[65535] = 0;
		}
	}
}
