#ifndef SEATARI_VIDEO_H_
#define SEATARI_VIDEO_H_
#include <stdint.h>
#include "SDL.h"
#include "log.h"

#define SCREEN_WIDTH  (160)
#define SCREEN_HEIGHT (192)

static void render(const uint8_t* restrict const screen)
{
	extern SDL_Renderer* renderer;
	extern SDL_Texture* texture;

	int pitch;
	Uint32* pixels;
	SDL_LockTexture(texture, NULL, (void**)&pixels, &pitch);

	for (unsigned i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
		pixels[i] = screen[i];

	SDL_UnlockTexture(texture);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


#endif
