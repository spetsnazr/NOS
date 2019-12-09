#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define RAM_SIZE 65536
#define VIDEO_MEMORY 0xEFF0
#define KEYBOARD 0xFFFF
#define HDD 0xFFFC
#define ROM 2048
#define PROGRAM_SIZE 0
#define FREQUENCY 2000000

/* TO DO LIST:
 * Popraviti greške pri kompajliranju
 * Implementirati traženu frekvenciju od 2 MHz upotrebom wait()
 * Implementirati tastaturu
 * Implementirati disk
 *      Izlazni port na adresi 0xFFFE sadrži komandu: 0 - Reset, 1 - Read, 2 - Write
 *      Izlazni port na adresi 0xFFFD sadrži izbor sektora
 *      Ulazno/Izlazni port na adresi 0xFFFC služi za prenos podataka
 * Implementirati video memoriju - 1b po pikselu, rezolucija 320x256 na adresi 8192
 * Implementirati interapt signale, svakih 20ms se generiše
 * Ukinuti pozive funkcija izlistanih ispod i ubaciti ih u glavni kod
*/

/* Instructions are coded like so: OOOO DDDD AAAA BBBB
   O - Opcode | D - Destination register | A and B - Source registers */
struct instr { unsigned short opcode, dest, src1, src2; };

void WriteToRAM(unsigned short data, unsigned short address, unsigned short *RAM, struct instr *code){
    if(address < ROM){
        printf("Can't write to reserved memory location");
        return;
    }
	else *(RAM+address) = data;
}
void InitialiseRAM(unsigned short *RAM, struct instr *code){

    return;
}
unsigned short ReadRAM(unsigned short address, unsigned short *RAM){
    return *(RAM+address);
}



void main(){
    unsigned short rs1, rs2, rd;
    int PC = 0, TPC = 0, i, counter = 0;
    int test = 4;
    char rotNum, rot;
    unsigned short temp;
    unsigned short RAM[RAM_SIZE];
    struct instr code[ROM];
    unsigned short *instructions[15];
    short regs[16];

    /* Frequency-related variables */
    struct timespec t1, t2;
    int dt;
    clock_t ct1, ct2;

    /* To prevent crashes in case a mistake is made regarding the program size */
    if(PROGRAM_SIZE > ROM) {
        printf("Program size exceeds ROM capacity.");
        return;
    }

    /* Adjust the pointers to the code labels */
    instructions[0]  = &&LOD; instructions[1]  = &&ADD; instructions[2]  = &&SUB; instructions[3]  = &&AND;
    instructions[4]  = &&OR;  instructions[5]  = &&XOR; instructions[6]  = &&SHR; instructions[7]  = &&MUL;
    instructions[8]  = &&STO; instructions[9]  = &&LDC; instructions[10] = &&GTU; instructions[11] = &&GTS;
    instructions[12] = &&LTU; instructions[13] = &&LTS; instructions[14] = &&EQU; instructions[15] = &&MAJ;

    /* Decode all instructions from the ROM and add them to the array */
    for(i = 0; i < PROGRAM_SIZE; i++){
        code[i].opcode = instructions[RAM[i] >> 12];
        code[i].dest = (RAM[i] >> 8) & 0x000F;
        code[i].src1 = (RAM[i] >> 4) & 0x000F;
        code[i].src2 = RAM[i] & 0x000F;
    }

    /* Start recording the used time and clock cycles to adjust the frequency later on */
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    ct1 = clock();
    PC+=4; goto *code[TPC++].opcode;

    /* Access registers like so: regs[code[TPC].dest/src1/src2]; */
LOD:
    regs[code[TPC].dest] = regs[code[TPC].src2];

    /* Wait to slow the program down to 2 MHz frequency, used in every instruction label */
    ct2 = clock();
    if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
ADD:
    regs[code[TPC].dest] = regs[code[TPC].src1] + regs[code[TPC].src2];
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
SUB:
    regs[code[TPC].dest] = regs[code[TPC].src1] - regs[code[TPC].src2];
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
AND:
    regs[code[TPC].dest] = regs[code[TPC].src1] & regs[code[TPC].src2];
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
OR:
    regs[code[TPC].dest] = regs[code[TPC].src1] | regs[code[TPC].src2];
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
XOR:
    regs[code[TPC].dest] = regs[code[TPC].src1] ^ regs[code[TPC].src2];
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
SHR:
    rotNum = regs[code[TPC].src2] & 0x07;
    rot = (regs[code[TPC].src2] >> 3) & 0x03;
    if(rot == 0){ /* Shift right, add sign to left side */
        regs[code[TPC].dest] = __rorw(regs[code[TPC].src1], rotNum);
    } else if(rot == 1){ /* Shift right, add 0 to left side */
        regs[code[TPC].dest] = (((unsigned short)regs[code[TPC].src1]) >> rotNum);
    } else if(rot == 2){ /* Shift left, rotate to right side */
        /* GCC-SPECIFIC COMMAND */
        regs[code[TPC].dest] = __rolw(regs[code[TPC].src1], rotNum);
    } else { /* Shift left, add 0 to right side */
        regs[code[TPC].dest] = (regs[code[TPC].src1] << rotNum);
    }

    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
MUL:
    regs[code[TPC].dest] = regs[code[TPC].src1] * regs[code[TPC].src2];
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
STO: //Ja ću ovu
    // Paziti na ROM upis
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
LDC:
    // Predznačno proširena od dva četverobitna broja???
    regs[code[TPC].dest] = (code[TPC].src1 << 4) + code[TPC].src2;
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
GTU:
    regs[code[TPC].dest] = (((unsigned short)regs[code[TPC].src1]) > ((unsigned short)regs[code[TPC].src2]));
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
GTS:
    regs[code[TPC].dest] = (regs[code[TPC].src1] > regs[code[TPC].src2]);
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
LTU:
    regs[code[TPC].dest] = (((unsigned short)regs[code[TPC].src1]) < ((unsigned short)regs[code[TPC].src2]));
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
LTS:
    regs[code[TPC].dest] = (regs[code[TPC].src1] < regs[code[TPC].src2]);
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
EQU:
    regs[code[TPC].dest] = (regs[code[TPC].src1] == regs[code[TPC].src2]);
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */
MAJ:
    regs[code[TPC].dest] = regs[code[TPC].src1];
    regs[15] = regs[code[TPC].src2];
    PC+=4; goto *code[TPC++].opcode;
    /* ------------------------------------------------------- */

END:
}
