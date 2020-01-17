#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define RAM_SIZE 65536
#define VIDEO_MEMORY 0xEFFB
#define KEYBOARD 0xFFFF
#define HDD 0xFFFC

unsigned int PROGRAM_SIZE  = 175;
#define DISK_NUM_SECTORS 100
#define FREQUENCY 2000000
unsigned short ROM = 2048;

/*kompajlirati sa gcc Emulator.c -o ER -lgdi32, nakon što se 
gdi32.dll fajl prebaci iz system32 u aktuelni folder*/

/* Instructions are coded like so: OOOO DDDD AAAA BBBB
   O - Opcode | D - Destination register | A and B - Source registers */
struct instr { unsigned short *opcode, dest, src1, src2; };


void main(){
	FILE *fp;
	unsigned short dly = 0;
    unsigned short rs1, rs2, rd;
    int PC = 0, TPC = 0, i, counter = 0;
    int test = 4;
    char rotNum, rot;
    unsigned short temp;
    unsigned short RAM[RAM_SIZE];
    struct instr code[ROM];
    unsigned short *instructions[16];
    unsigned short regs[16];
    regs[15] = 0;
	int an = 0;
	
    /* Frequency-related variables */
    struct timespec t1, t2;
    int dt,j,rowPixel;
    clock_t ct1, ct2,inter,nextinter;
	unsigned char diskInstr = 0;
	int br;
	for(br = VIDEO_MEMORY; br < 0xFFFC;br++) RAM[br] = 0;
	int lim = 65532/2;
	/* UPIS U DISK ZA ISPISVANJE HELLO 
	
	for(br = VIDEO_MEMORY; br < 0xFFFC-3072;br+=16)  RAM[br] = 0xC003;
	
	RAM[VIDEO_MEMORY + 512] = 0xFFFF;
	RAM[VIDEO_MEMORY + 528] = 0xFFFF;

	for(br = VIDEO_MEMORY+2; br < 0xFFFC-3072;br+=16) RAM[br] = 0x000C;

    RAM[VIDEO_MEMORY + 2] = 0xFFFC;
	RAM[VIDEO_MEMORY + 18] = 0xFFFC;
	RAM[VIDEO_MEMORY + 514] = 0xFFFC;
	RAM[VIDEO_MEMORY + 530] = 0xFFFC;
	RAM[VIDEO_MEMORY + 1026] = 0xFFFC;
	RAM[VIDEO_MEMORY + 1042] = 0xFFFC;
	for(br = VIDEO_MEMORY+4; br < 0xFFFC-3072;br+=16) RAM[br] = 0x000C;
	for(br = VIDEO_MEMORY+6; br < 0xFFFC-3072;br+=16) RAM[br] = 0x000C;
	for(br = VIDEO_MEMORY+8; br < 0xFFFC-3072;br+=16) RAM[br] = 0x000C;
	for(br = VIDEO_MEMORY+10; br < 0xFFFC-3072;br+=16) RAM[br] = 0x000C;
	
	RAM[VIDEO_MEMORY + 8] = 0xFFFC;
	RAM[VIDEO_MEMORY + 24] = 0xFFFC;
	RAM[VIDEO_MEMORY + 9] = 0xFFFF;
	RAM[VIDEO_MEMORY + 25] = 0xFFFF;
	RAM[VIDEO_MEMORY + 10] = 0x000F;
	RAM[VIDEO_MEMORY + 26] = 0x000F;
	
	RAM[VIDEO_MEMORY + 1028] = 0xFFFC;
	RAM[VIDEO_MEMORY + 1044] = 0xFFFC;
	RAM[VIDEO_MEMORY + 1030] = 0xFFFC;
	RAM[VIDEO_MEMORY + 1046] = 0xFFFC;
	
	RAM[VIDEO_MEMORY + 1032] = 0xFFFC;
	RAM[VIDEO_MEMORY + 1048] = 0xFFFC;
	RAM[VIDEO_MEMORY + 1033] = 0xFFFF;
	RAM[VIDEO_MEMORY + 1049] = 0xFFFF;
	//br-=16;
	RAM[br] = 0x000F;
	RAM[br+16] = 0x000F;
	fp = fopen("HDD.bin","wb+");
	fseek(fp,0,SEEK_SET);
	fwrite(&RAM[VIDEO_MEMORY],2,4096,fp);
			//RAMWriteTemp = RAM[0xFFFC];
	fclose(fp);*/
	unsigned short dskRead = 0;
    unsigned int RAMWriteTemp = 0;
	unsigned short tempsrc2;
	
	HWND consoleWindow = GetConsoleWindow();

	
    HDC consoleDC = GetDC(consoleWindow);		
    ct1 = clock();
	inter = clock();
	MSG msg;
	short iter = 100;
    
     RAM[iter++] = 0x9000;       /* LDC R0,  0             R0 = 0 */
    RAM[iter++] = 0x9101;       /* LDC R1,  1             R1 = 1 */
    RAM[iter++] = 0x9200;       /* LDC R2,  0             R2 = 0 */
    RAM[iter++] = 0x9AFF;       /* LDC R10, FF            R10 = FF */
    RAM[iter++] = 0x9BFE;       /* LDC R11, FE            R11 = FE */
    RAM[iter++] = 0x9C38;       /* LDC R12, 38            R12 = 38      Za shiftanje za prosirivanje sa 8b na 16b */
    RAM[iter++] = 0x6AAC;       /* SHR R10, R10, R12      R10 = FF00 */
    RAM[iter++] = 0x1AAB;       /* ADD R10, R10, R11      R10 = FFFE */
    RAM[iter++] = 0x861A;       /* STO R1,  R10          [R10] = 1 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x860A;       /* STO R2,  R10          [R10] = 0 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFC */
    RAM[iter++] = 0x9DEF;       /* LDC R13, EF            R13 = EF      Postavljanje pocetne adrese video memorije */
    RAM[iter++] = 0x9EFB;       /* LDC R14, FB            R14 = FB */
    RAM[iter++] = 0x6DDC;       /* SHR R13, R13, R12      R13 = EF00    UPOZORENJE: R12 (C) MORA BITI 0038 */
    RAM[iter++] = 0x1DDE;       /* ADD R13, R13, R14      R13 = EFFB */
    RAM[iter++] = 0x86DA;       /* STO R13,  R10         [R10] = EFFB */


    RAM[iter++] = 0x9502;       /* LDC R5,  2              R5 = 2 */
    RAM[iter++] = 0x9837;       /* LDC R8,  7              R8 = 7      Za shiftanje na 256 */
    RAM[iter++] = 0x6558;       /* SHR R5,  R5, R8         R5 = 0100 */





    RAM[iter++] = 0x1001;       /* ADD R0,  R0,  R1       R0 = 1 */
    RAM[iter++] = 0x1AA1;       /* ADD R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x1AA1;       /* ADD R10, R10, R1       R10 = FFFE */
    RAM[iter++] = 0x861A;       /* STO R1,  R10          [R10] = 1 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x860A;       /* STO R2,  R10          [R10] = 0 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFC */
    RAM[iter++] = 0x9DEF;       /* LDC R13, EF            R13 = EF      Postavljanje pocetne adrese video memorije */
    RAM[iter++] = 0x9EFB;       /* LDC R14, FB            R14 = FB */
    RAM[iter++] = 0x6DDC;       /* SHR R13, R13, R12      R13 = EF00    UPOZORENJE: R12 (C) MORA BITI 0038 */
    RAM[iter++] = 0x1DDE;       /* ADD R13, R13, R14      R13 = EFFB */
    RAM[iter++] = 0x1DD5;       /* ADD R13, R13, R5       R13 += 256 */
    RAM[iter++] = 0x86DA;       /* STO R13,  R10         [R10] = VIDEO */





    RAM[iter++] = 0x1001;       /* ADD R0,  R0,  R1       R0 = 2 */
    RAM[iter++] = 0x1AA1;       /* ADD R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x1AA1;       /* ADD R10, R10, R1       R10 = FFFE */
    RAM[iter++] = 0x861A;       /* STO R1,  R10          [R10] = 1 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x860A;       /* STO R2,  R10          [R10] = 0 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFC */
    RAM[iter++] = 0x1DD5;       /* ADD R13, R13, R5       R13 += 256 */
    RAM[iter++] = 0x86DA;       /* STO R13,  R10         [R10] = VIDEO */





    RAM[iter++] = 0x1001;       /* ADD R0,  R0,  R1       R0 = 3 */
    RAM[iter++] = 0x1AA1;       /* ADD R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x1AA1;       /* ADD R10, R10, R1       R10 = FFFE */
    RAM[iter++] = 0x861A;       /* STO R1,  R10          [R10] = 1 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x860A;       /* STO R2,  R10          [R10] = 0 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFC */
    RAM[iter++] = 0x1DD5;       /* ADD R13, R13, R5       R13 += 256 */
    RAM[iter++] = 0x86DA;       /* STO R13,  R10         [R10] = VIDEO */





    RAM[iter++] = 0x1001;       /* ADD R0,  R0,  R1       R0 = 4 */
    RAM[iter++] = 0x1AA1;       /* ADD R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x1AA1;       /* ADD R10, R10, R1       R10 = FFFE */
    RAM[iter++] = 0x861A;       /* STO R1,  R10          [R10] = 1 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFD */
    RAM[iter++] = 0x860A;       /* STO R2,  R10          [R10] = 0 */

    RAM[iter++] = 0x2AA1;       /* SUB R10, R10, R1       R10 = FFFC */
    RAM[iter++] = 0x1DD5;       /* ADD R13, R13, R5       R13 += 256 */
    RAM[iter++] = 0x86DA;       /* STO R13,  R10         [R10] = VIDEO */
	PROGRAM_SIZE = iter;

	RECT rekt;
	GetWindowRect(consoleWindow ,&rekt);

		
	
    goto START;
	
    /* Start recording the used time and clock cycles to adjust the frequency later on */
    
    

    /* Access registers like so: regs[code[TPC-100].dest/src1/src2]; */
LOD:
    regs[code[TPC-100].dest] = regs[code[TPC-100].src2];
	dly++;
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
    /* Wait to slow the program down to 2 MHz frequency, used in every instruction label */
    
   /* */
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC,  rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			
			if(rowPixel == 16) rowPixel = 0;
		}
		
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
ADD:
	dly++;
    regs[code[TPC-100].dest] = regs[code[TPC-100].src1] + regs[code[TPC-100].src2];
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
	
    /**/
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
SUB:
	dly++;
    regs[code[TPC-100].dest] = regs[code[TPC-100].src1] - regs[code[TPC-100].src2];
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
   /* */
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
AND:
	dly++;
    regs[code[TPC-100].dest] = regs[code[TPC-100].src1] & regs[code[TPC-100].src2];
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
    /**/
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
OR:
	dly++;
    regs[code[TPC-100].dest] = regs[code[TPC-100].src1] | regs[code[TPC-100].src2];
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
   /* */
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
XOR:
	dly++;
    regs[code[TPC-100].dest] = regs[code[TPC-100].src1] ^ regs[code[TPC-100].src2];
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
   /* */
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
SHR:
	dly++;
    rotNum = regs[code[TPC-100].src2] & 0x0F;
    rot = (regs[code[TPC-100].src2] >> 4) & 0x03;
    if(rot == 0){ /* Shift right, add sign to left side */
        regs[code[TPC-100].dest] = _lrotr(regs[code[TPC-100].src1], rotNum);
    } else if(rot == 1){ /* Shift right, add 0 to left side */
        regs[code[TPC-100].dest] = (((unsigned short)regs[code[TPC-100].src1]) >> rotNum);
    } else if(rot == 2){ /* Shift left, rotate to right side */
        /* GCC-SPECIFIC COMMAND */
        regs[code[TPC-100].dest] = _lrotl(regs[code[TPC-100].src1], rotNum);
    } else { /* Shift left, add 0 to right side */
	    
        regs[code[TPC-100].dest] = (((unsigned short)regs[code[TPC-100].src1]) << rotNum);
		//printf("ok%d\n",regs[code[TPC-100].dest]);
    }
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
  /*  */
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
MUL:
	dly++;
    regs[code[TPC-100].dest] = regs[code[TPC-100].src1] * regs[code[TPC-100].src2];
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
    /**/
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
STO: 
	dly++;//Ja ću ovu
    // Paziti na ROM upis
	tempsrc2 = code[TPC-100].src2;
	if(regs[tempsrc2] < (unsigned short)ROM){
		printf("Error: Can't write into ROM");
		
		goto END;
	} else if(regs[tempsrc2] == 0xFFFF){
		printf("Error: Reserved address for keyboard input");
		goto END;
	}

	
	regs[code[TPC-100].dest] = RAM[regs[tempsrc2]] = regs[code[TPC-100].src1];
	if(regs[tempsrc2] >= 0xFFFC) diskInstr++;
	//;
	//printf("%d\n",diskInstr);
	if(diskInstr == 3){ 
		//printf("CITAM\n");
		diskInstr = 0;
		switch(RAM[0xFFFE]){
			
			case 0:
			fp = fopen("HDD.bin","wb+");
			fseek(fp,RAM[0xFFFD]*512,SEEK_SET);
			for(br = 0; br < 512; br++){
				
				fseek(fp,br,SEEK_CUR);
				fwrite(0,1,1,fp);	
			}
			fclose(fp);
			break;
			case 1:
			fp = fopen("HDD.bin","rb+");
			fseek(fp,RAM[0xFFFD]*512,SEEK_SET);
			RAMWriteTemp = RAM[0xFFFC];
			if(RAMWriteTemp < 2048){
				printf("Error: Can't write to ROM");
				
				goto END;
			} else if(regs[tempsrc2] == 0xFFFF){
				printf("Error: Reserved address for keyboard input");
				goto END;
			}

			fread(&RAM[RAMWriteTemp],2,256,fp);

			fclose(fp);
			break;
			case 2:
			fp = fopen("HDD.bin","wb+");
			fseek(fp,RAM[0xFFFD]*512,SEEK_SET);
			RAMWriteTemp = RAM[0xFFFC];
			
			fwrite(&RAM[RAMWriteTemp],2,256,fp);

			fclose(fp);
		}
		
	}
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
   /* */
	
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
LDC:
	dly++;
    // Predznačno proširena od dva četverobitna broja???
    regs[code[TPC-100].dest] = (code[TPC-100].src1 << 4) + code[TPC-100].src2;
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	//printf("%d",rekt.right);
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16 + 400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16){
				rowPixel = 0;		
			}
		}
		inter = clock();
	}
	
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
GTU:
	dly++;
    regs[code[TPC-100].dest] = (((unsigned short)regs[code[TPC-100].src1]) > ((unsigned short)regs[code[TPC-100].src2]));
    if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
    /**/
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
GTS:
	dly++;
    regs[code[TPC-100].dest] = (regs[code[TPC-100].src1] > regs[code[TPC-100].src2]);
    if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
   /* */
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
LTU:
	dly++;
    regs[code[TPC-100].dest] = (((unsigned short)regs[code[TPC-100].src1]) < ((unsigned short)regs[code[TPC-100].src2]));
    if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
    /**/
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
LTS:
	dly++;
    regs[code[TPC-100].dest] = (regs[code[TPC-100].src1] < regs[code[TPC-100].src2]);
    if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
    /**/
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	nextinter = clock();
	if(!((nextinter-inter)/(CLOCKS_PER_SEC*100)%20)||1){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, 200+rowPixel*16 + 500 + i, 800-j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
EQU:
	dly++;
    regs[code[TPC-100].dest] = (regs[code[TPC-100].src1] == regs[code[TPC-100].src2]);
    ct2 = clock();
    /**/
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */
MAJ:
	dly++;
    regs[code[TPC-100].dest] = regs[code[TPC-100].src1];
    regs[15] = regs[code[TPC-100].src2];
	TPC = regs[15];
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
    /**/
	/*Dobivamo WM_KEYDOWN ili WM_KEYUP message*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Prevoditmo taj message*/
	TranslateMessage(&msg);
	/*Nakon prevođenja dobivamo WM_CHAR*/
	PeekMessage(&msg,consoleWindow, 0, 0,1);
	/*Koristmo sam bite 16-23 od WM_CHAR jer je to karakter koji nam treba*/
	RAM[0xFFFF] = (msg.lParam >> 15) & 0x0000FFFF;
	if(dly > 20000){
		dly = 0;
		ct2 = clock();
		while(ct2 - ct1 < 1000) ct2 = clock();
		ct1 = clock();
	}
	if(((nextinter)/(CLOCKS_PER_SEC/50)%20) < 10){
		
		j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 16) rowPixel = 0;
		}
		inter = clock();
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    goto *code[TPC-100].opcode;
    /* ------------------------------------------------------- */

START:

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
	
    for(i = 100; i < PROGRAM_SIZE; i++){
        code[i-100].opcode = instructions[RAM[i] >> 12];
        code[i-100].dest = (RAM[i] >> 8) & 0x000F;
        code[i-100].src1 = (RAM[i] >> 4) & 0x000F;
        code[i-100].src2 = RAM[i] & 0x000F;
    }
	TPC = 100;
	regs[15] = 100;
	if(TPC >= PROGRAM_SIZE) goto END;

	regs[15]+=1; goto *code[TPC-100].opcode;
END:

	j = 0;
		rowPixel = 0;
		for(br = VIDEO_MEMORY; br < 0xFFFC; br++){
			for(i = 0; i < 16; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC,  rowPixel*16 + 500 + i, j/16+400, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			
			if(rowPixel == 16) rowPixel = 0;
		}
	
	scanf("%d",&i);
	

	return;
	
}
