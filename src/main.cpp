/*
 * main.cpp
 *
 *  Created on: Oct 18, 2017
 *      Author: vmontero
 */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
#ifdef LINUX
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif
#include <Chip8Machine.h>

int main(int args, char* argv[])
{
	SDL_Event evt;
	SDL_Surface* surf = NULL;
	bool isRunning = true;
	Uint32 LastTicks = 0;
	int cycles = 0;

	Chip8Machine cpu;
	if (!cpu.LoadROM("roms/INVADERS"))
	{
		fprintf(stderr, "Error al cargar\n");
		return EXIT_FAILURE;
	}

	srand(time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("%s\n", SDL_GetError());
		return EXIT_FAILURE;
	}
	atexit(SDL_Quit);
	SDL_Window* win = SDL_CreateWindow("Chip-8 Emulator",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

	SDL_Renderer* renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture* tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	//-------------------------------------------------------------------------------

	surf = SDL_CreateRGBSurface(0, 64, 32, 32,
		0x00FF0000,
		0x0000FF00,
		0x000000FF,
		0xFF000000);

	if (SDL_LockTexture(tex, NULL, &surf->pixels, &surf->pitch) < 0)
	{
		fprintf(stderr, "%s", SDL_GetError());
	}
	cpu.ScreenToUint32((Uint32*)surf->pixels);
	SDL_UnlockTexture(tex);
	//-------------------------------------------------------------------------------

	while (isRunning)
	{
		while (SDL_PollEvent(&evt))
		{
			switch (evt.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;
			}
		}

		if (SDL_GetTicks() - cycles > 1)
		{
			cpu.StepMachine();
			cycles = SDL_GetTicks();
		}

		if (SDL_GetTicks() - LastTicks > (1000 / 16))
		{
			if (cpu.dt) cpu.dt--;
			if (cpu.st) cpu.st--;

			if (SDL_LockTexture(tex, NULL, &surf->pixels, &surf->pitch) < 0)
			{
				fprintf(stderr, "%s", SDL_GetError());
			}
			cpu.ScreenToUint32((Uint32*)surf->pixels);
			SDL_UnlockTexture(tex);

			SDL_RenderCopy(renderer, tex, NULL, NULL);
			SDL_RenderPresent(renderer);

			LastTicks = SDL_GetTicks();
		}

	}

	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(win);

	return 0;
}