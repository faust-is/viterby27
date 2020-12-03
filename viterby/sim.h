#pragma once
#include<stdint.h>

void modnoise(
	uint8_t *symbols,		/* Input and Output symbols, 8 bits each */
	unsigned int nsyms,		/* Symbol count */
	double amp,			/* Signal amplitude */
	double noise);			/* Noise amplitude */