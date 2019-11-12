#include <stdio.h>
#include <stdlib.h>
#define ramsize 65536
#define videomemory 0xEFF0
#define keyboard 0xFFFF
#define HDD 0xFFFC
#define ROM 2048

struct instr {
	unsigned short opcode;
	unsigned short dest,src1,src2;
};

//to do ukinut pozivanje funkcija i ovdje pa prebacit dole
/*void WriteToRAM(unsigned short data,unsigned short address, unsigned short *RAM, instr *tempRAM){
	if(address < 2049) printf("Can't Write to reserved memory location"), return;
	else *(RAM+address) = data;
}*/
/*void InitialiseRAM(unsigned short *RAM, instr *tempRAM){
	
	return;
}*/
unsigned short ReadRAM(unsigned short address, unsigned short *RAM){
	return *(RAM+address);
}




int main(){
	
	
	unsigned short usedup = 0;
	int PC,TC;
	int test = 4;
	unsigned short temp;
	unsigned short RAM[ramsize];
	struct instr *tempRAM[ramsize];
	int i = 0;
	for(i = 0; i < ramsize; i++){
		tempRAM[i] = (struct instr*)malloc(sizeof(struct instr));
	}
	short registers[16];
	

	for (i = 0; i < ramsize; i++) {
		free(tempRAM[i]);
	}
	
	
	goto START;
	//registrima se može jednostavno pristupit kao registers[tempRAM[TC]->dest/src1/src2];
	
	
	
	LOD: //Ja ću ovu
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	ADD:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	SUB:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	ORA:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	XOR:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	SHR:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	MUL:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	STO: //Ja ću ovu
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	LDC:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	GTU:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	GTS:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	LTU:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	LTS:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	EQU:
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	MAJ: //Ovu ćemo kasnije
	
	
	PC+=4;
	TC++;
	goto *tempRAM[TC++]->opcode;
	//////////////////////////////////////////////
	
	START:
		unsigned short **instructions;
		instructions = malloc(sizeof(short)*15);
		instructions[0] = &&LOD,instructions[1] = &&ADD,instructions[2] = &&SUB,instructions[3] = &&ORA,
		instructions[4] = &&XOR,instructions[5] = &&SHR,instructions[6] = &&MUL,instructions[7] = &&STO,
		instructions[8] = &&LDC,instructions[9] = &&GTU,instructions[10] = &&GTS,instructions[11] = &&LTU,
		instructions[12] = &&LTS,instructions[13] = &&EQU,instructions[14] = &&MAJ;
		for(i = 0; i < usedup; i++){
			tempRAM[i]->opcode = instructions[RAM[i] >> 12];
			tempRAM[i]->dest = instructions[(RAM[i] >> 8) & 0x000F];
			tempRAM[i]->src1 = instructions[(RAM[i] >> 4) & 0x000F];
			tempRAM[i]->src2 = instructions[RAM[i] & 0x000F];
		}
		free(instructions);
		PC++;
		goto *tempRAM[TC++]->opcode;
	END:	
	
	return 0;
}
