#include <stdint.h>
#include "tia.h"

#define TICKS_PER_SCANLINE (228)


static unsigned scanline;
static unsigned clock;
static uint32_t screen[SCR_HEIGHT][SCR_WIDTH];


void resettia(void)
{
	scanline = 0;
	clock = 0;
}

void steptia(const unsigned ticks)
{
	clock += ticks;
	if (ticks >= TICKS_PER_SCANLINE) {
		
		clock = 0;
		++scanline;
		if (scanline >= SCR_HEIGHT)
			scanline = 0;
	}
}


