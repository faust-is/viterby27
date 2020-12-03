/*
 * Copyright 1995 Phil Karn, KA9Q
 * Copyright 2008,2018 Free Software Foundation, Inc.
 *
 */

/*
 * Some modifications from original Karn code by Matt Ettus
 */

#include <iostream>
#include "viterby27.h"
#include "tab.h"

/* 
 * Since these polynomials are known to be optimal for this constraint
 * length there is not much point in changing them. But if you do, you
 * will have to regenerate the BUTTERFLY macro calls in viterbi()
 */
#define POLYA 0xf
#define POLYB 0xd

/* The basic Viterbi decoder operation, called a "butterfly"
 * operation because of the way it looks on a trellis diagram. Each
 * butterfly involves an Add-Compare-Select (ACS) operation on the two nodes
 * where the 0 and 1 paths from the current node merge at the next step of
 * the trellis.
 *
 * The code polynomials are assumed to have 1's on both ends. Given a
 * function encode_state() that returns the two symbols for a given
 * encoder state in the low two bits, such a code will have the following
 * identities for even 'n' < 64:
 *
 * 	encode_state(n) = encode_state(n+65)
 *	encode_state(n+1) = encode_state(n+64) = (3 ^ encode_state(n))
 *
 * Any convolutional code you would actually want to use will have
 * these properties, so these assumptions aren't too limiting.
 *
 * Doing this as a macro lets the compiler evaluate at compile time the
 * many expressions that depend on the loop index and encoder state and
 * emit them as immediate arguments.
 * This makes an enormous difference on register-starved machines such
 * as the Intel x86 family where evaluating these expressions at runtime
 * would spill over into memory.
 */
#define BUTTERFLY(i, sym)                                         \
    {                                                             \
        int m0, m1;                                               \
                                                                  \
        /* ACS for 0 branch */                                    \
        m0 = state[i].metric + mets[sym];					      \
        m1 = state[i + 4].metric + mets[3 ^ sym];                 \
        if (m0 > m1) {                                            \
            next[2 * i].metric = m0;                              \
            next[2 * i].path = state[i].path << 1;                \
        } else {                                                  \
            next[2 * i].metric = m1;                              \
            next[2 * i].path = state[i + 4].path << 1;            \
        }                                                         \
        /* ACS for 1 branch */                                    \
        m0 = state[i].metric + mets[3 ^ sym];                     \
        m1 = state[i + 4].metric + mets[sym];                     \
        if (m0 > m1) {                                            \
            next[2 * i + 1].metric = m0;                          \
            next[2 * i + 1].path = (state[i].path << 1) | 1;      \
        } else {                                                  \
            next[2 * i + 1].metric = m1;                          \
            next[2 * i + 1].path = (state[i + 4].path << 1) | 1;  \
        }                                                         \
    }


/* Convolutionally encode data into binary symbols */
unsigned char encode(unsigned char* symbols,
                     unsigned char* data,
                     unsigned int nbytes,
                     unsigned char encstate)
{
    int i;

    while (nbytes-- != 0) {
        for (i = 7; i >= 0; i--) {
            encstate = (encstate << 1) | ((*data >> i) & 1);
            *symbols++ = Partab[encstate & POLYA];
            *symbols++ = Partab[encstate & POLYB];
        }
        data++;
    }

    return encstate;
}

/* Reverse the bits in a byte */
static uint8_t reverse(uint8_t byte)
{
	return (byte & 0x80) >> 7 | (byte & 0x40) >> 5 | (byte & 0x20) >> 3 | (byte & 0x10) >> 1 | (byte & 0x08) << 1 | (byte & 0x04) << 3 | (byte & 0x02) << 5 | (byte & 0x01) << 7;
}


int viterbi(unsigned long* metric,  /* Final path metric (returned value) */
            uint8_t *data,    /* Decoded output data */
			uint8_t *symbols, /* Raw deinterleaved input symbols */
            unsigned int nbits,     /* Number of output bits */
            int (&mettab)[2][256])      /* Metric table, [sent sym][rx symbol] */
{
    unsigned int bitcnt = 0;
    int mets[4];
    long bestmetric;
    int beststate, i;

    struct viterbi_state state0[8], state1[8], *state, *next;

    state = state0;
    next = state1;

	/// Исходное состояние регистра: 000, этому состоянию соответствует state[0].
	/// Присваиваем этому состоянию наибольшее значение метрики, равное 0,
	/// для всех остальных состояний метрика равна -999999

    state[0].metric = 0;
    for (i = 1; i < 8; i++)
        state[i].metric = -999999;
    state[0].path = 0;

    for (bitcnt = 0; bitcnt < nbits; bitcnt++) {
        /* Read input symbol pair and compute all possible branch
         * metrics
         */
        mets[0] = mettab[0][symbols[0]] + mettab[0][symbols[1]];
        mets[1] = mettab[0][symbols[0]] + mettab[1][symbols[1]];
        mets[2] = mettab[1][symbols[0]] + mettab[0][symbols[1]];
        mets[3] = mettab[1][symbols[0]] + mettab[1][symbols[1]];
        symbols += 2;


        BUTTERFLY(0, 0);
        BUTTERFLY(1, 2);
        BUTTERFLY(2, 3);
        BUTTERFLY(3, 1);

        /// Меняем местами два буффера
        if (bitcnt & 1) {
            state = state0;
            next = state1;
        } else {
            state = state1;
            next = state0;
        }

        /// Запись декодированного сообщения в ячейки по 8 бит, 
		/// при переполнении path (условимся, что path занимает 32 бита)

        if (bitcnt + 1 >=  32 && (bitcnt + 1) % 8 == 0) {
            /// Поиск состояния (path) с наилучшей метрикой
            bestmetric = state[0].metric;
            beststate = 0;
            for (i = 1; i < 8; i++) {
                if (state[i].metric > bestmetric) {
                    bestmetric = state[i].metric;
                    beststate = i;
                }
            }
#ifdef notdef
            printf("metrics[%d] = %d state = %lx\n",
                   beststate,
                   state[beststate].metric,
                   state[beststate].path);
#endif	
			// инверсия бит
            *data++ = reverse(state[beststate].path >> 24);
			
        }
    }
    /* Output remaining bits from 0 state */
    // Find best state instead
    bestmetric = state[0].metric;
    beststate = 0;
    for (i = 1; i < 8; i++) {
        if (state[i].metric > bestmetric) {
            bestmetric = state[i].metric;
            beststate = i;
        }
    }

#ifdef notdef
	printf("metrics[%d] = %d state = %lx\n",
		beststate,
		state[beststate].metric,
		state[beststate].path);

	//printf("metrics[%d] = %d (sfift = %d)state = %lx\n",
	//	beststate,
	//	state[beststate].metric, (bitcnt % 8),
	//	state[beststate].path << (8-(bitcnt % 8)));

#endif	

	if ((bitcnt % 8) != 0) {
		state[beststate].path <<= (8 - (bitcnt % 8));
		*data++ = reverse(state[beststate].path >> 24);
		
	}


    *data++ = reverse(state[beststate].path >> 16);
    *data++ = reverse(state[beststate].path >> 8);
    *data = reverse(state[beststate].path);

    *metric = state[beststate].metric;
    return 0;
}

/* Demaping:
*  1100 -> (255 + 255 - 0 - 0 + 510) / 4 = 255 -> 1
*  0011 -> (0 + 0 - 255 - 255 + 510) / 4 = 0 -> 0
*/
int patternDemapper(uint8_t *demap_symbols, uint8_t *symbols, int len_symbols)
{
	int len_demap_symbols = floor(len_symbols / 4);
	
	for (int i = 0; i < len_demap_symbols; i++)
	{
		demap_symbols[i] = (symbols[4 * i] + symbols[4 * i + 1] - symbols[4 * i + 2] - symbols[4 * i + 3] + 510) / 4;
	}

	return 0;
}


