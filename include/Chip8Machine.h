/*
* Chip8Machine.h
*
*  Created on: Oct 18, 2017
*      Author: vmontero
*/

#ifndef CHIP8MACHINE_H_
#define CHIP8MACHINE_H_

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <time.h>
#ifdef LINUX
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

class Chip8Machine
{
public:
	unsigned short i;
	unsigned short pc;
	unsigned short sp;
	unsigned short stack[16];

	unsigned char mem[4096];
	unsigned char registers[16];
	unsigned char dt, st;
	char screen[2048];
	char* romName;
	char hexcodes[80];
	char keys[16];
	char waitKey;
	FILE* openedRom;
	bool isRunning;

	Chip8Machine();
	~Chip8Machine(){}
	bool LoadROM(const char*);
	long GetRomSize();
	void Clear();
	void ScreenToUint32(Uint32* result);
	void StepMachine();
	int IsKeyDown(char key);
};

#endif /* CHIP8MACHINE_H_ */
