#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#define RAM_SIZE 65536
#define VIDEO_MEMORY 0xEFF0
#define KEYBOARD 0xFFFF
#define HDD 0xFFFC
#define ROM 2048
#define PROGRAM_SIZE 1
#define DISK_NUM_SECTORS 100
#define FREQUENCY 2000000


/*kompajlirati sa gcc Emulator.c -o ER -lgdi32, nakon što se 
gdi32.dll fajl prebaci iz system32 u aktuelni folder*/
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
struct instr { unsigned short *opcode, dest, src1, src2; };



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
    regs[15] = 0;
	
    /* Frequency-related variables */
    struct timespec t1, t2;
    int dt,j,rowPixel;
    clock_t ct1, ct2,inter,nextinter;
	unsigned char diskInstr = 0;
	int br;
	for(br = VIDEO_MEMORY; br < 0xFFFC;br++) RAM[br] = 0;
	unsigned short dskRead = 0;
    unsigned int RAMWriteTemp = 0;
	unsigned short tempsrc2;
	FILE *fp;
	HWND consoleWindow = GetConsoleWindow();
	HDC consoleDC = GetDC(consoleWindow);
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    ct1 = clock();
	inter = clock();
	MSG msg;
	RAM[0] = 0x9022;
	RAM[0xEFF0] = 0xFFFF;
	RAM[0xEFF1] = 0xFFFF;
	RAM[0xEFF2] = 0xFFFF;
	RAM[0xEFF3] = 0xFFFF;
	RAM[0xEFF4] = 0xFFFF;
	RAM[0xEFF5] = 0xFFFF;
	RAM[0xEFF6] = 0xFFFF;
	RAM[0xEFF7] = 0xFFFF;
	RAM[0xEFF8] = 0xFFFF;
	RECT rekt;
	for(br=0; br < 8; br++) printf("\n\n");
	GetWindowRect(consoleWindow ,&rekt);
	//DWORD dw = GetLastError(); 
	//printf("NIGGERS%s",dw);
	/*RAM[0xEFF8+1] = 0xFFFF;
	RAM[0xEFF8+2] = 0xFFFF;
	RAM[0xEFF8+3] = 0xFFFF;
	RAM[0xEFF8+4] = 0xFFFF;
	RAM[0xEFF8+5] = 0xFFFF;
	RAM[0xEFF8+6] = 0xFFFF;
	RAM[0xEFF8+7] = 0xFFFF;
	RAM[0xEFF8+8] = 0xFFFF;
	RAM[0xEFF8+9] = 0xFFFF;
	RAM[0xEFF8+10] = 0xFFFF;
	RAM[0xEFF8+11] = 0xFFFF;
	RAM[0xEFF8+12] = 0xFFFF;*/
    goto START;
	
    /* Start recording the used time and clock cycles to adjust the frequency later on */
    
    

    /* Access registers like so: regs[code[TPC].dest/src1/src2]; */
LOD:
    regs[code[TPC].dest] = regs[code[TPC].src2];

    /* Wait to slow the program down to 2 MHz frequency, used in every instruction label */
    ct2 = clock();
   /* if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i + rekt.right/2, j/32 + rekt.bottom , RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
ADD:
    regs[code[TPC].dest] = regs[code[TPC].src1] + regs[code[TPC].src2];
	ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
SUB:
    regs[code[TPC].dest] = regs[code[TPC].src1] - regs[code[TPC].src2];
	ct2 = clock();
   /* if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
AND:
    regs[code[TPC].dest] = regs[code[TPC].src1] & regs[code[TPC].src2];
	ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
OR:
    regs[code[TPC].dest] = regs[code[TPC].src1] | regs[code[TPC].src2];
	ct2 = clock();
   /* if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
XOR:
    regs[code[TPC].dest] = regs[code[TPC].src1] ^ regs[code[TPC].src2];
	ct2 = clock();
   /* if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
SHR:
    rotNum = regs[code[TPC].src2] & 0x07;
    rot = (regs[code[TPC].src2] >> 3) & 0x03;
    if(rot == 0){ /* Shift right, add sign to left side */
        regs[code[TPC].dest] = _lrotr(regs[code[TPC].src1], rotNum);
    } else if(rot == 1){ /* Shift right, add 0 to left side */
        regs[code[TPC].dest] = (((unsigned short)regs[code[TPC].src1]) >> rotNum);
    } else if(rot == 2){ /* Shift left, rotate to right side */
        /* GCC-SPECIFIC COMMAND */
        regs[code[TPC].dest] = _lrotl(regs[code[TPC].src1], rotNum);
    } else { /* Shift left, add 0 to right side */
        regs[code[TPC].dest] = (regs[code[TPC].src1] << rotNum);
    }
	ct2 = clock();
  /*  if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
MUL:
    regs[code[TPC].dest] = regs[code[TPC].src1] * regs[code[TPC].src2];
	ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
STO: //Ja ću ovu
    // Paziti na ROM upis
	tempsrc2 = code[TPC].src2;
	if(tempsrc2 < ROM){
		printf("Error: Can't write into ROM");
		goto END;
	} else if(tempsrc2 == 0xFFFF){
		printf("Error: Reserved address for keyboard input");
		goto END;
	}
	if(RAM[0xFFFD] >  DISK_NUM_SECTORS){ 
		printf("Not enough disk space");
		goto END;
	}
	regs[tempsrc2] = RAM[tempsrc2] = code[TPC].src1;
	if(tempsrc2 >= 0xFFFC) diskInstr++;
	if(diskInstr == 2){ 
		diskInstr = 0;
		switch(RAM[0xFFFE]){
			case 0:
			fp = fopen("HDD.dsk","w+");
			fseek(fp,RAM[0xFFFD]*512,SEEK_SET);
			for(br = 0; br < 512; br++){
				fseek(fp,br,SEEK_CUR);
				fwrite(0,1,1,fp);	
			}
			fclose(fp);
			break;
			case 1:
			fp = fopen("HDD.dsk","r+");
			fseek(fp,RAM[0xFFFD]*512,SEEK_SET);
			RAMWriteTemp = RAM[0xFFFC];
			if(RAMWriteTemp < 2048){
				printf("Error: Can't write to ROM");
				goto END;
			} else if(tempsrc2 == 0xFFFF){
				printf("Error: Reserved address for keyboard input");
				goto END;
			}
			for(br = 0; br < 512; br+=2){
				fseek(fp,br,SEEK_CUR);
				fread(&dskRead,2,1,fp);
				if(RAMWriteTemp > RAM_SIZE) break;
				RAM[RAMWriteTemp++] = dskRead;
			}
			fclose(fp);
			break;
			case 2:
			fp = fopen("HDD.dsk","w+");
			fseek(fp,RAM[0xFFFD]*512,SEEK_SET);
			RAMWriteTemp = RAM[0xFFFC];
			for(br = 0; br < 512; br+=2){
				fseek(fp,br,SEEK_CUR);
				//može se u jednoj liniji ali ovako se obezbjeđuje da se ne izađe van
				//opsega RAM-a
				fwrite(&RAM[RAMWriteTemp++],2,1,fp);
				if(RAMWriteTemp > RAM_SIZE) break;
			}
			fclose(fp);
		}
	}
	ct2 = clock();
   /* if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
	
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
LDC:
    // Predznačno proširena od dva četverobitna broja???
	//printf("COON");
	//printf("COON%d",code[TPC].src2);
    regs[code[TPC].dest] = (code[TPC].src1 << 4) + code[TPC].src2;
	ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_REALTIME, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_REALTIME, &t1);
    }*/
	//printf("reg %d",regs[0]);
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32){
				rowPixel = 0;		
			}
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
GTU:
    regs[code[TPC].dest] = (((unsigned short)regs[code[TPC].src1]) > ((unsigned short)regs[code[TPC].src2]));
    ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
GTS:
    regs[code[TPC].dest] = (regs[code[TPC].src1] > regs[code[TPC].src2]);
    ct2 = clock();
   /* if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
LTU:
    regs[code[TPC].dest] = (((unsigned short)regs[code[TPC].src1]) < ((unsigned short)regs[code[TPC].src2]));
    ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
LTS:
    regs[code[TPC].dest] = (regs[code[TPC].src1] < regs[code[TPC].src2]);
    ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, 200+rowPixel*8 + i, 800-j/32, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
EQU:
    regs[code[TPC].dest] = (regs[code[TPC].src1] == regs[code[TPC].src2]);
    ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC].opcode;
    /* ------------------------------------------------------- */
MAJ:
    regs[code[TPC].dest] = regs[code[TPC].src1];
    regs[15] = regs[code[TPC].src2];
	TPC = regs[15];
	ct2 = clock();
    /*if(ct2 - ct1 >= FREQUENCY){
        ct1 = ct2;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
        dt = 1000.0*t2.tv_sec + 1e-6*t2.tv_nsec - (1000.0*t1.tv_sec + 1e-6*t1.tv_nsec);
        if(dt < 1000) Sleep(1000 - dt);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    }*/
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
			for(i = 0; i < 8; i++){
				if(RAM[br] >> i & 0x0001) SetPixel(consoleDC, rowPixel*8 + i+ rekt.right/4, j/32+rekt.bottom/2, RGB(255, 255, 255));
			}
			j++;
			rowPixel++;
			if(rowPixel == 32) rowPixel = 0;
		}
	}
	if(++TPC >= PROGRAM_SIZE) goto END;
    goto *code[TPC].opcode;
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

    /* Decode all instructions from the ROM and add them to the array */
    for(i = 0; i < PROGRAM_SIZE; i++){
        code[i].opcode = instructions[RAM[i] >> 12];
        code[i].dest = (RAM[i] >> 8) & 0x000F;
        code[i].src1 = (RAM[i] >> 4) & 0x000F;
        code[i].src2 = RAM[i] & 0x000F;
    }
	//printf("PINGPONGCHINGCHONG%d",code[0].src1);
	if(TPC >= PROGRAM_SIZE) goto END;
	regs[15]+=1; goto *code[TPC].opcode;
END:
	//printf("Press any key to quit...");
	scanf("%d",&i);
	
	return;
	
}
