#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "SDL.h"
#include "SDL_audio.h"
#include "audio.h"
#include "log.h"
#include "rom.h"
#include "cpu.h"


#define TEXTURE_WIDTH  (160)
#define TEXTURE_HEIGHT (192)
#define WIN_WIDTH  TEXTURE_WIDTH
#define WIN_HEIGHT TEXTURE_HEIGHT

SDL_AudioDeviceID audio_device;
SDL_Renderer* renderer;
SDL_Texture* texture;
static SDL_Window* window;


static bool initialize_platform(void)
{
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) != 0) {
		logerror("Couldn't initialize SDL: %s\n", SDL_GetError());
		return false;
	}

	// video
	window = SDL_CreateWindow("µnes", SDL_WINDOWPOS_CENTERED,
				  SDL_WINDOWPOS_CENTERED,
				  WIN_WIDTH, WIN_HEIGHT,
				  SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		logerror("Failed to create SDL_Window: %s\n", SDL_GetError());
		goto Lquitsdl;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		logerror("Failed to create SDL_Renderer: %s\n", SDL_GetError());
		goto Lfreewindow;
	}

	SDL_RendererInfo info;
	SDL_GetRendererInfo(renderer, &info);
	texture = SDL_CreateTexture(renderer, info.texture_formats[0],
	                            SDL_TEXTUREACCESS_STREAMING,
				    TEXTURE_WIDTH, TEXTURE_HEIGHT);
	if (texture == NULL) {
		logerror("Failed to create SDL_Texture: %s\n", SDL_GetError());
		goto Lfreerenderer;
	}

	// audio
	SDL_AudioSpec want;
	SDL_zero(want);
	want.freq = AUDIO_FREQUENCY;
	want.format = AUDIO_S16SYS;
	want.channels = 1;
	want.samples = AUDIO_BUFFER_SIZE;
	if ((audio_device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0)) == 0) {
		logerror("Failed to open audio: %s\n", SDL_GetError());
		goto Lfreetexture;
	}

	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	SDL_PauseAudioDevice(audio_device, 0);
	return true;

Lfreetexture:
	SDL_DestroyTexture(texture);
Lfreerenderer:
	SDL_DestroyRenderer(renderer);
Lfreewindow:
	SDL_DestroyWindow(window);
Lquitsdl:
	SDL_Quit();
	return false;
}

static void terminate_platform(void)
{
	SDL_CloseAudioDevice(audio_device);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

static uint8_t* readfile(const char* filepath)
{
	FILE* const file = fopen(filepath, "r");
	if (file == NULL) {
		logerror("Couldn't open \'%s\'\n", filepath);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	const size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	uint8_t* data = malloc(size);
	if (data == NULL) {
		logerror("Couldn't allocate memory\n");
		goto Lfclose;
	}

	if (fread(data, 1, size, file) < size) {
		logerror("Couldn't read \'%s\'\n", filepath);
		free(data);
		data = NULL;
		goto Lfclose;
	}

Lfclose:
	fclose(file);
	return data;
}

static bool update_events(void)
{
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		case SDL_QUIT:
			return false;
		case SDL_KEYDOWN:
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				return false;
			//update_key(event.key.keysym.scancode, KEYSTATE_DOWN);
			break;
		case SDL_KEYUP:
			//update_key(event.key.keysym.scancode, KEYSTATE_UP);
			break;
		}
	}

	return true;
}


int main(const int argc, const char* const* const argv)
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [romfile]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if (!initialize_platform())
		return EXIT_FAILURE;

	int exitcode = EXIT_FAILURE;

	uint8_t* const rom = readfile(argv[1]);
	if (!rom)
		goto Lterminate_platform;
	
	loadrom(rom);
	resetcpu();
	while (update_events()) {
		unsigned clk = 0;
		Uint32 dt = SDL_GetTicks();
		do {
			clk += stepcpu();
		} while (clk < ATARI_CPU_FREQ);
		SDL_Delay(1000 - (SDL_GetTicks() - dt));
	}

	exitcode = EXIT_SUCCESS;
	unloadrom();
	free(rom);
Lterminate_platform:
	terminate_platform();
	return exitcode;
}

