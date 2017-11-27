#ifndef SEATARI_VIDEO_H_
#define SEATARI_VIDEO_H_
#include <stdint.h>
#include "SDL.h"
#include "log.h"

#define SCREEN_WIDTH  (160)
#define SCREEN_HEIGHT (192)

static void render(const uint32_t* const screen)
{
	extern SDL_Renderer* renderer;
	extern SDL_Texture* texture;

	int pitch;
	Uint32* pixels;
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	memcpy(pixels, screen, SCREEN_WIDTH * SCREEN_HEIGHT);

	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


#endif
