#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "SDL.h"
#include "SDL_audio.h"
#include "audio.h"
#include "log.h"
#include "cpu.h"


#define TEXTURE_WIDTH  (256)
#define TEXTURE_HEIGHT (240)
#define WIN_WIDTH      TEXTURE_WIDTH
#define WIN_HEIGHT     TEXTURE_HEIGHT

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


int main(const int argc, const char* const* const argv)
{
	return 0;
}
