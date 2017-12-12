/*
 * Chip8Machine.cpp
 *
 *  Created on: Oct 18, 2017
 *      Author: vmontero
 */

#include <Chip8Machine.h>

Chip8Machine::Chip8Machine()
{
	{
		isRunning = true;
		Clear();
		//0
		hexcodes[0] = 0xF0;
		hexcodes[1] = 0x90;
		hexcodes[2] = 0x90;
		hexcodes[3] = 0x90;
		hexcodes[4] = 0xF0;
		//1
		hexcodes[5] = 0x20;
		hexcodes[6] = 0x60;
		hexcodes[7] = 0x20;
		hexcodes[8] = 0x20;
		hexcodes[9] = 0x70;
		//2
		hexcodes[10] = 0xF0;
		hexcodes[11] = 0x10;
		hexcodes[12] = 0xF0;
		hexcodes[13] = 0x80;
		hexcodes[14] = 0xF0;
		//3
		hexcodes[15] = 0xF0;
		hexcodes[16] = 0x10;
		hexcodes[17] = 0xF0;
		hexcodes[18] = 0x10;
		hexcodes[19] = 0xF0;
		//4
		hexcodes[20] = 0x90;
		hexcodes[21] = 0x90;
		hexcodes[22] = 0xF0;
		hexcodes[23] = 0x10;
		hexcodes[24] = 0x10;
		//5
		hexcodes[25] = 0xF0;
		hexcodes[26] = 0x80;
		hexcodes[27] = 0xF0;
		hexcodes[28] = 0x10;
		hexcodes[29] = 0xF0;
		//6
		hexcodes[30] = 0xF0;
		hexcodes[31] = 0x80;
		hexcodes[32] = 0xF0;
		hexcodes[33] = 0x90;
		hexcodes[34] = 0xF0;
		//7
		hexcodes[35] = 0xF0;
		hexcodes[36] = 0x10;
		hexcodes[37] = 0x20;
		hexcodes[38] = 0x40;
		hexcodes[39] = 0x40;
		//8
		hexcodes[40] = 0xF0;
		hexcodes[41] = 0x90;
		hexcodes[42] = 0xF0;
		hexcodes[43] = 0x90;
		hexcodes[44] = 0xF0;
		//9
		hexcodes[45] = 0xF0;
		hexcodes[46] = 0x90;
		hexcodes[47] = 0xF0;
		hexcodes[48] = 0x10;
		hexcodes[49] = 0xF0;
		//A
		hexcodes[50] = 0xF0;
		hexcodes[51] = 0x90;
		hexcodes[52] = 0xF0;
		hexcodes[53] = 0x90;
		hexcodes[54] = 0x90;
		//B
		hexcodes[55] = 0xE0;
		hexcodes[56] = 0x90;
		hexcodes[57] = 0xE0;
		hexcodes[58] = 0x90;
		hexcodes[59] = 0xE0;
		//C
		hexcodes[60] = 0xF0;
		hexcodes[61] = 0x80;
		hexcodes[62] = 0x80;
		hexcodes[63] = 0x80;
		hexcodes[64] = 0xF0;
		//D		 
		hexcodes[65] = 0xE0;
		hexcodes[66] = 0x90;
		hexcodes[67] = 0x90;
		hexcodes[68] = 0x90;
		hexcodes[69] = 0xE0;
		//E
		hexcodes[70] = 0xF0;
		hexcodes[71] = 0x80;
		hexcodes[72] = 0xF0;
		hexcodes[73] = 0x80;
		hexcodes[74] = 0xF0;
		//F		 
		hexcodes[75] = 0xF0;
		hexcodes[76] = 0x80;
		hexcodes[77] = 0xF0;
		hexcodes[78] = 0x80;
		hexcodes[79] = 0x80;
		memcpy(mem + 0x50, hexcodes, 80);

		keys[0] = SDL_SCANCODE_X;
		keys[1] = SDL_SCANCODE_1;
		keys[2] = SDL_SCANCODE_2;
		keys[3] = SDL_SCANCODE_3;
		keys[4] = SDL_SCANCODE_Q;
		keys[5] = SDL_SCANCODE_W;
		keys[6] = SDL_SCANCODE_E;
		keys[7] = SDL_SCANCODE_A;
		keys[8] = SDL_SCANCODE_S;
		keys[9] = SDL_SCANCODE_D;
		keys[10] = SDL_SCANCODE_Z; //A
		keys[11] = SDL_SCANCODE_C; //B
		keys[12] = SDL_SCANCODE_4; //C
		keys[13] = SDL_SCANCODE_R; //D
		keys[14] = SDL_SCANCODE_F; //E
		keys[15] = SDL_SCANCODE_V; //F
	}
}

bool Chip8Machine::LoadROM(const char* filename)
{
	openedRom = fopen(filename, "rb");
	if (!openedRom)
		return false;

	int length = GetRomSize();

	if (length > 3584)
	{
		fprintf(stderr, "ROM too large.\n");
		return 1;
	}

	fread(mem + 0x200, length, 1, openedRom);
	fclose(openedRom);
	return true;
}

long Chip8Machine::GetRomSize()
{
	fseek(openedRom, 0, SEEK_END);
	long length = ftell(openedRom);
	//fseek(openedRom, 0, SEEK_SET);
	rewind(openedRom);

	return length;
}

void Chip8Machine::Clear()
{
	i = sp = dt = st = 0x00;
	pc = 0x200;
	memset(mem, 0x00, sizeof(char) * 4096);
	memset(stack, 0x00, sizeof(short) * 16);
	memset(registers, 0x00, sizeof(char) * 16);
	memset(screen, 0, sizeof(char) * 2048);
	waitKey = -1;
}

void Chip8Machine::ScreenToUint32(Uint32* result)
{
	for (int i = 0; i < 2048; ++i)
	{
		// printf("screen %d %x\n",screen[i],screen[i]);
		result[i] = (screen[i]) ? -1 : 0;
	}
}

void Chip8Machine::StepMachine()
{
	unsigned short opcode = (mem[pc] << 8) | mem[pc + 1]; //Lee 2 bytes.
	pc = (pc + 2) & 0xFFF;

	// if (pc == 4096)
	// pc = 0;

	unsigned short nnn = opcode & 0x0FFF;
	unsigned char kk = opcode & 0xFF;
	unsigned char n = opcode & 0xF;
	unsigned char x = (opcode >> 8) & 0xF;
	unsigned char y = (opcode >> 4) & 0xF;
	unsigned char inst = (opcode >> 12);

	switch (inst)
	{
	case 0:
		if (opcode == 0x00E0)
		{
			#ifdef DEBUG
			printf("CLS\n");
			#endif
			// memset(screen, 0x00, sizeof(screen));
			memset(screen, 0, 2048);
		}
		else if (opcode == 0x00EE)
		{
			#ifdef DEBUG
			printf("RET\n");
			#endif
			if (sp > 0)
				pc = stack[--sp];
		}
		break;

	case 1:
		#ifdef DEBUG
		printf("JP %x\n", nnn);
		#endif
		pc = nnn;
		break;

	case 2:
		#ifdef DEBUG
		printf("CALL %x\n", nnn);
		#endif
		if (sp < 16) {
			stack[sp++] = pc;
			pc = nnn;
		}
		break;

	case 3:
		#ifdef DEBUG
		printf("SE %x,%x\n", x, kk);
		#endif
		if (registers[x] == kk)
			pc = (pc + 2) & 0xFFF;
		break;

	case 4:
		#ifdef DEBUG
		printf("SNE %x,%x\n", x, kk);
		#endif
		if (registers[x] != kk)
			pc = (pc + 2) & 0xFFF;
		break;

	case 5:
		#ifdef DEBUG
		printf("SE %x,%x\n", x, y);
		#endif
		if (registers[x] == registers[y])
			pc = (pc + 2) & 0xFFF;
		break;

	case 6:
		#ifdef DEBUG
		printf("LD %x,%x\n", x, kk);
		#endif
		registers[x] = kk;
		break;

	case 7:
		#ifdef DEBUG
		printf("ADD %x,%x\n", x, kk);
		#endif
		registers[x] += kk;
		// registers[x] = (registers[x] + kk) & 0xFF;
		break;

	case 8:
		switch (n)
		{
		case 0:
			#ifdef DEBUG
			printf("LD %x,%x\n", x, y);
			#endif
			registers[x] = registers[y];
			break;
		case 1:
			#ifdef DEBUG
			printf("OR %x,%x\n", x, y);
			#endif
			registers[x] |= registers[y];
			break;
		case 2:
			#ifdef DEBUG
			printf("AND %x,%x\n", x, y);
			#endif
			registers[x] &= registers[y];
			break;
		case 3:
			#ifdef DEBUG
			printf("XOR %x,%x\n", x, y);
			#endif
			registers[x] ^= registers[y];
			break;
		case 4:
			#ifdef DEBUG
			printf("ADD %x,%x\n", x, y);
			#endif
			registers[0xF] = (registers[x] > registers[x] + registers[y]);
			registers[x] += registers[y];
			break;
		case 5:
			#ifdef DEBUG
			printf("SUB %x,%x\n", x, y);
			#endif
			registers[0xF] = (registers[x] > registers[y]);
			registers[x] -= registers[y];
			break;
		case 6:
			#ifdef DEBUG
			printf("SHR %x\n", x);
			#endif
			registers[0xF] = (registers[x] & 1);
			registers[x] >>= 1;
			break;
		case 7:
			#ifdef DEBUG
			printf("SUBN %x,%x\n", x, y);
			#endif
			registers[0xF] = (registers[y] > registers[x]);
			registers[x] = registers[y] - registers[x];
			break;
		case 0xE:
			#ifdef DEBUG
			printf("SHL %x,%x\n", x, y);
			#endif
			registers[0xF] = ((registers[x] & 0x80) != 0);
			registers[x] <<= 1;
			break;
		}
		break;
	case 9:
		#ifdef DEBUG
		printf("SNE %x,%x\n", x, y);
		#endif
		if (registers[x] != registers[y])
			pc = (pc + 2) & 0xFFF;
		break;
	case 0xA:
		#ifdef DEBUG
		printf("LD I,%x\n", nnn);
		#endif
		i = nnn;
		break;
	case 0xB:
		#ifdef DEBUG
		printf("JP V0,%x\n", nnn);
		#endif
		pc = (nnn + registers[0]) & 0xFFF;
		break;
	case 0xC:
		#ifdef DEBUG
		printf("RND %x,%x\n", x, kk);
		#endif
		registers[x] = rand() & kk;
		break;
	case 0xD:
		/*
			DRW x,y,n:
			Dibuja un sprite en el pixel v[x],v[y].
			El numero de filas a dibujar se indica con n.
			El sprite se saca de la direccion de memoria [I].
			*/
		#ifdef DEBUG
		printf("DRW %x,%x,%x\n", x, y, n);
		#endif
		registers[15] = 0;
		for (int j = 0; j < n; j++)
		{
			unsigned char sprite = mem[i + j];
			for (int i = 0; i < 8; i++)
			{
				int px = (registers[x] + i) & 63;
				int py = (registers[y] + j) & 31;
				int pos = 64 * py + px;
				int pixel = (sprite & (1 << (7 - i))) != 0;
				registers[15] |= (screen[pos] & pixel);
				screen[pos] ^= pixel;
			}
		}
		break;
	case 0xE:
		if (kk == 0x9E)
		{
			#ifdef DEBUG
			printf("SKP %x\n", x);
			#endif
			if (IsKeyDown(registers[x]))
				pc = (pc + 2) & 0xFFF;
		}
		else if (kk == 0xA1)
		{
			#ifdef DEBUG
			printf("SKNP %x\n", x);
			#endif
			if (!IsKeyDown(registers[x]))
				pc = (pc + 2) & 0xFFF;
		}
		break;
	case 0xF:
		switch (kk)
		{
		case 0x07:
			#ifdef DEBUG
			printf("LD %x, DT\n", x);
			#endif
			registers[x] = dt;
			break;
		case 0x0A:
			#ifdef DEBUG
			printf("LD %x,K\n", x);
			#endif
			waitKey = x;
			break;
		case 0x15:
			#ifdef DEBUG
			printf("LD DT,%x\n", x);
			#endif
			dt = registers[x];
			break;
		case 0x18:
			#ifdef DEBUG
			printf("LD ST,%x\n", x);
			#endif
			st = registers[x];
			break;
		case 0x1E:
			#ifdef DEBUG
			printf("ADD I,%x\n", x);
			#endif
			i += registers[x];
			break;
		case 0x29:
			#ifdef DEBUG
			printf("LD F,%x\n", x);
			#endif
			i = 0x50 + (registers[x] & 0xF) * 5;
			break;
		case 0x33:
			#ifdef DEBUG
			printf("LD B,%x\n", x);
			#endif
			mem[i + 2] = registers[x] % 10;
			mem[i + 1] = (registers[x] / 10) % 10;
			mem[i] = registers[x] / 100;

			break;
		case 0x55:
			#ifdef DEBUG
			printf("LD [I],%x\n", x);
			#endif
			for (int index = 0; index <= x; index++)
			{
				if (index > 15)
					break;

				mem[i + index] = registers[index];
			}
			break;
		case 0x65:
			//Guardar los registros desde 0 hasta x en la memoria comenzando con I
			#ifdef DEBUG
			printf("LD %x,[I]", x);
			#endif
			for (int index = 0; index <= x; index++)
			{
				if (index > 15)
					break;

				registers[index] = mem[i + index];
			}
			break;
		}
		break;
	default:
		break;
	}
}

int Chip8Machine::IsKeyDown(char key)
{
	const Uint8* sdlKeys = SDL_GetKeyboardState(NULL);
	Uint8 realKeys = keys[key];
	return sdlKeys[realKeys];
}