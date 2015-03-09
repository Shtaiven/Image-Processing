// ============================================================================
//
// histo_match.cpp - Match one image's histogram to a predetermined one.
// Copyright (C) 2015 by Rosario Antunez, Steven Eisinger, and Ethan Graber
//
// Written by: Rosario Antunez, Steven Eisinger, and Ethan Graber, 2015
//
// ============================================================================

#include "IP.h"
#include <cmath>
#include <cstdio>
using namespace std;

// function prototype
void histo_match(imageP, int, imageP);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine to match image histogram.
//
int main(int argc, char** argv) {
	int	n;
	imageP	I1, I2;

	// error checking: proper usage
	if (argc != 4)
	{
	cerr << "Usage: histo_match infile n outfile\n";
	exit(1);
	}

	// read input image (I1) and reserve space for output (I2)
	I1 = IP_readImage(argv[1]);
	I2 = NEWIMAGE;

	// read n
	n = atoi(argv[2]);

	// match the image histogram to a new one and save result in file
	histo_match(I1, n, I2);
	IP_saveImage(I2, argv[3]);

	// free up image structures/memory
	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// histo_match:
//
// Generate the histogram we want to match; match the output image's histogram to it.
//
//
void histo_match(imageP I1, int n, imageP I2)
{
	int i, p, R;
	int left[MXGRAY], right[MXGRAY], maxInBin[MXGRAY];
	int total, Hsum, h1[MXGRAY], h2[MXGRAY];
	unsigned char *in, *out;
	double scale, Havg;

	/* total number of pixels in image */
	total = (long)I1->height * I1->width;

	/* init I2 dimensions and buffer */
	I2->width = I1->width;
	I2->height = I1->height;
	I2->image = (unsigned char *)malloc(total);

	in = I1->image;	/* input image buffer */
	out = I2->image;	/* output image buffer */

	for (i = 0; i<MXGRAY; i++) h1[i] = 0; /* clear histogram */
	for (i = 0; i<total; i++) h1[in[i]]++;/* eval histogram */

	/* The following three cases set up the shape of our histogram */
	double average = (double) total / (double) MXGRAY;
	if (n == 0) { // flat histogram
		for (i = 0; i < MXGRAY-1; ++i) h2[i] = (int) average;
		h2[MXGRAY-1] = total - (int) average*(MXGRAY-1);
		//for (i = 0; i < MXGRAY; ++i) cout << h2[i] << ", "; // debug
	}
	else if (n>0) { // positive exponential histogram
		Havg = 0;
		for (i = 0; i < MXGRAY; ++i) {
			h2[i] = ROUND(pow((double) i/MXGRAY, (double) n) * MXGRAY); // determines the shape of the histogram
			Havg += h2[i];
		}
		scale = (double) total / Havg; // scales the histogram
		if(scale != 1) for(i=0; i<MXGRAY; i++) h2[i] *= scale;
		//for (i = 0; i < MXGRAY; ++i) cout << h2[i] << ", "; // debug
	}
	else if (n<0) { // negative exponential histogram
		Havg = 0;
		for (i = 0; i < MXGRAY; ++i) {
			h2[i] = ROUND(pow(1- (double) i/MXGRAY, (double) abs(n)) * MXGRAY); // determines the shape of the histogram
			Havg += h2[i];
		}
		scale = (double) total / Havg; // scales the histogram
		if(scale != 1) for(i=0; i<MXGRAY; i++) h2[i] *= scale;
		//for (i = 0; i < MXGRAY; ++i) cout << h2[i] << ", "; // debug
	}

	R = 0;
	Hsum = 0;
	
	for (int i = 0; i < MXGRAY; ++i) maxInBin[i] = h2[i];

	/* evaluate remapping of all input gray levels;
	Each input gray value maps to an interval of valid output values.
	The endpoints of the intervals are left[] and right[] */
	for (i = 0; i<MXGRAY; ++i) {
		left[i] = R;	/* left end of interval */
		Hsum += h1[i];	/* cumulative value for interval */
		while (Hsum>h2[R] && R<MXGRAY - 1) { /* compute width of interval */
			Hsum -= h2[R];	/* adjust Hsum as interval widens */
			R++;	/* update */
		}
		right[i] = R;	/* init right end of interval */
		maxInBin[R] = h2[i] - Hsum; // store maximum pixels of intensity h2[i] allowed in left bin 
	}

	/* clear h1 and reuse it below */
	for (i = 0; i<MXGRAY; ++i) h1[i] = 0;

	/* visit all input pixels */
	for (i = 0; i<total; ++i) {
		p = left[in[i]];
		if (h1[p] < h2[p]) 	/* mapping satisfies h2 */
			out[i] = p;

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~			
		if (h1[p] < h2[p] - maxInBin[p]) {
				left[in[i]] = p + 1;
		}
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~			
	
		else out[i] = p = left[in[i]] = MIN(p + 1, right[in[i]]);
		h1[p]++;
	}
}
