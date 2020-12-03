/* Simulate AWGN channel
 * Copyright 1994 Phil Karn, KA9Q
 * May be used under the terms of the GNU Public License
 */
#include "sim.h"
#include <math.h>

#include <time.h>
#include <stdlib.h> // srand()

#define	OFFSET	128

double normal_rand(double mean, double std_dev);

/* Turn binary symbols into individual 8-bit channel symbols
 * with specified noise and gain
 */
void modnoise(
uint8_t *symbols,		/* Input and Output symbols, 8 bits each */
unsigned int nsyms,		/* Symbol count */
double amp,			/* Signal amplitude */
double noise)			/* Noise amplitude */
{
  double s;

  while(nsyms-- != 0){
    s = normal_rand(*symbols ? 1.0 : -1.0,noise);
    s *= amp;
    s += OFFSET;
    if(s > 255)
      s = 255;	/* Clip to 8-bit range */
    if(s < 0)
      s = 0;
    *symbols++ = floor(s+0.5);	/* Round to int */
  }
}



double normal_rand(double mean, double std_dev)
{//Box muller method
	static double n2 = 0.0;
	static int n2_cached = 0;
	if (!n2_cached)
	{
		double x, y, r;
		do
		{
			x = 2.0*rand() / RAND_MAX - 1;
			y = 2.0*rand() / RAND_MAX - 1;

			r = x * x + y * y;
		} while (r == 0.0 || r > 1.0);
		{
			double d = sqrt(-2.0*log(r) / r);
			double n1 = x * d;
			n2 = y * d;
			double result = n1 * std_dev + mean;
			n2_cached = 1;
			return result;
		}
	}
	//else
	{
		n2_cached = 0;
		return n2 * std_dev + mean;
	}
}
