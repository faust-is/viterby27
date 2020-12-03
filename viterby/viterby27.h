#pragma once

#include <stdint.h>

const int Rate = 2;

// FEC block 1 coded using S = 8
const int lenBlock1 = 296;

int patternDemapper(uint8_t *demap_symbols, uint8_t *symbols, int len_symbols);

struct viterbi_state {
	unsigned long path; /* Decoded path to this state */
	long metric;        /* Cumulative metric to this state */
};


unsigned char encode(unsigned char* symbols,
	unsigned char* data,
	unsigned int nbytes,
	unsigned char encstate);

/* Viterbi decoder */
int viterbi(unsigned long* metric,  /* Final path metric (returned value) */
	uint8_t * data,    /* Decoded output data */
	uint8_t* symbols, /* Raw deinterleaved input symbols */
	unsigned int nbits,     /* Number of output bits */
	int (&mettab)[2][256]);     /* Metric table, [sent sym][rx symbol] */


