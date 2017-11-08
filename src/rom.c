#include <stdlib.h>
#include "rom.h"

const uint8_t* rom_data = NULL;

void loadrom(const uint8_t* const data)
{
	rom_data = data;
}

void unloadrom(void)
{
	rom_data = NULL;
}

