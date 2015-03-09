// ============================================================================

// histo_plot.cpp - Outputs the evaluation of an image's histogram.
// Copyright (C) 2015 by Rosario Antunez, Steven Eisinger, and Ethan Graber
//
// Written by: Rosario Antunez, Steven Eisinger, and Ethan Graber, 2015

// ============================================================================

//#include "stdafx.h"
#include "IP.h"
using namespace std;

// function prototype
void createfreqtable(imageP, int[]);
void drawHistorgram(imageP, int[]);
void histo_plot(imageP, imageP, int);
int computeAvg(int[], int);
void scaleft(float, int[]);
int findmax(int[]);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine to create histogram image.
//
int
main(int argc, char** argv)
{
	int flg;
	imageP    I1, I2;

	// error checking: proper usage
	if (argc != 4) {
		cerr << "Usage: histo_plot infile outfile flag\n";
		exit(1);
	}

	// read input image (I1) and reserve space for output (I2)
	I1 = IP_readImage(argv[1]);
	I2 = NEWIMAGE;

	// read flag
	flg = atoi(argv[3]);

	//  image and save result in file
	histo_plot(I1, I2, flg);
	IP_saveImage(I2, argv[2]);

	// free up image structures/memory
	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// histo_plot: 
//
// Routine generate the histogram output.
//
void histo_plot(imageP I1, imageP I2, int flg)
{
	int     totalimg2;
	int freqtable[256], ftavg, maxfreq;
	float factor;

	// total number of pixels in image
	totalimg2 = 256 * 256;

	// init I2 dimensions and buffer
	I2->width = 256;
	I2->height = 256;
	I2->image = (uchar *)malloc(totalimg2);
	if (I2->image == NULL) {
		cerr << "histo_plot: Insufficient memory\n";
		exit(1);
	}

	// create frequency table
	createfreqtable(I1, freqtable);
	ftavg = computeAvg(freqtable, I1->width * I1->height);
	factor = 128.0 / ftavg;
	if (flg == 0)
	{
		factor = (256.0 * 128.0) / (I1->width * I1->height);
		scaleft(factor, freqtable);
	}
	else
	{
		maxfreq = findmax(freqtable);
		factor = 256.0 / maxfreq;
		scaleft(factor, freqtable);
	}
	// create histogram image for freqtable
	drawHistorgram(I2, freqtable);


}

int findmax(int ft[])
{
	int i, max;

	max = ft[0];
	for (i = 1; i < 256;i++)
		if (max < ft[i]) { max = ft[i]; }

	return(max);
}

void scaleft(float factor, int ft[])
{
	int i;

	for (i = 0; i < 256; i++)
		ft[i] = (int) (factor * ft[i]);
}

int computeAvg(int ft[], int numpixels)
{
	int i, sum,avg;

	sum = 0;
	for (i = 0; i < 256; i++) sum = sum + (i*ft[i]);
	avg = sum / numpixels;
	return(avg);
}

void drawHistorgram(imageP I2, int ft[])
{
	int x, y, i, freq;
	uchar* img = I2->image;
	int size = I2->height*I2->width;

	// initialize all pixels of I2 to be black
	for (i = 0; i < size; i++)
		img[i] = 0;

	// create white columns for the frequency of each intensity
	for (x = 0; x < 256; x++)
	{
		freq = ft[x];
		for (y = 255; (y > (255 - freq) && y > 0); y--) // no clippling when all elements of ft are in 0..255 (like for when input flg = 1)
			img[x+(y*256)] = 255;
	}

}

void createfreqtable(imageP I1, int ft[])
{
	int totalimg1 = I1->width * I1->height;
	uchar* img = I1->image;
	int i;

	// intialize all frequencies to 0
	for (i = 0; i < 256; i++)
		ft[i] = 0;

	// count frequencies of image pixels
	for (i = 0; i < totalimg1; i++)
		ft[img[i]]++;
}

// Advantages/disadvantages of method when flag = 0
// ------------  Advantage--------------------
// 1- The lower frequency portion of the histogram can be observed in more detail. 
// 2- Clipping eliminates outliers on the high side. 
// -------------  Disadvantage ------------------
// Since we are clipping at 255 then we lose the relative proportion between the frequencies if any 
// frequency is larger than 255. Therefore, the top of the histogram is clipped.

// Advantages/disadvantages of method when flag = 1 
// ------------  Advantage  -----------------------
// The whole curve can be seen. The outline of the top of the histogram can be seen. 
// ------------  Disadvantage ---------------------
// If some frequencies are very large compared to other frequencies then the fine details of the 
// histogram for the samller frequencies cannot be observed. 