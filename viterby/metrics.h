#pragma once
void gen_met(
	int (&mettab)[2][256],	/* Metric table, [sent sym][rx symbol] */
	int amp,		/* Signal amplitude, units */
	double noise,		/* Relative noise voltage */
	double bias,		/* Metric bias; 0 for viterbi, rate for sequential */
	int scale);		/* Scale factor */