#ifndef SEATARI_CPU_H_
#define SEATARI_CPU_H_
#include <stdint.h>
#include <stdbool.h>


#define ATARI_CPU_FREQ (1193182)

extern void resetcpu(void);
extern unsigned stepcpu(void);


#endif
