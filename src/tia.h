#ifndef SEATARI_TIA_H_
#define SEATARI_TIA_H_

#define SCR_WIDTH  (160)
#define SCR_HEIGHT (228)

void resettia(void);
void steptia(unsigned ticks);
void writetia(uint8_t val, uint8_t addr);

#endif
