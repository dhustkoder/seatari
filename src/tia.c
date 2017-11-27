#include <stdint.h>
#include "video.h"
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
	if (clock >= TICKS_PER_SCANLINE) {	
		clock = 0;
		++scanline;
		if (scanline >= SCR_HEIGHT) {
			render(&screen[0][0]);
			scanline = 0;
		}
	}
}

void writetia(const uint8_t val, const uint8_t addr)
{
	screen[scanline][(int)((double)clock / ((double)TICKS_PER_SCANLINE / (double)SCR_WIDTH))] = val;
}

