// ============================================================================
// histo_stretch.cpp - Histogram Stretching.
// Copyright (C) 2015 by Rosario Antunez, Steven Eisinger, and Ethan Graber
//
// Written by: Rosario Antunez, Steven Eisinger, and Ethan Graber, 2015
// ============================================================================


//#include "stdafx.h"
#include "IP.h"
using namespace std;

// function prototype
void createfreqtable(imageP, int[]);
void histo_stretch(imageP, int, int, imageP);
int firstNonZero(int[]);
int lastNonZero(int[]);
void pointTransformation(uchar*, uchar*, uchar*, int);
void init_lut(int, int, uchar*);
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine to create histogram image.
//
int
main(int argc, char** argv)
{
	int t1, t2;
	imageP    I1, I2;

	// error checking: proper usage
	if (argc != 5) {
		cerr << "Usage: histo_stretch infile t1 t2 outfile\n";
		exit(1);
	}

	// read input image (I1) and reserve space for output (I2)
	I1 = IP_readImage(argv[1]);
	I2 = NEWIMAGE;

	// read flag
	t1 = atoi(argv[2]);
	t2 = atoi(argv[3]);

	//  image and save result in file
	histo_stretch(I1, t1, t2, I2);
	IP_saveImage(I2, argv[4]);

	// free up image structures/memory
	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 
//
//

int firtsNonZero(int ft[])
{
	int i;

	i = 0;
	while (ft[i] == 0)
	{
		i++;
	}

	return(i);
}

int lastNonZero(int ft[])
{
	int i;

	i = 255;
	while (ft[i] == 0)
	{
		i--;
	}

	return(i);
}

void histo_stretch(imageP I1, int t1, int t2, imageP I2)
{
	int i, min, max, size, ft[256];
	uchar *img, lut[256];

	size = I1->width * I1->height;

	createfreqtable(I1, ft);

	if (t1 >= 0)
	{
		min = t1;
	}
	else
	{
		min = firtsNonZero(ft);
	}
	if (t2 >= 0)
	{
		max = t2;
	}
	else
	{
		max = lastNonZero(ft);
	}

	// init I2 dimensions and buffer
	I2->width = I1->width;
	I2->height = I1->height;
	I2->image = (uchar *)malloc(size);
	img = I2->image;

	init_lut(min, max, lut);

	pointTransformation(I1->image, img, lut, size);

	
}

void init_lut(int min, int max, uchar lut[])
{
	int i;
	int range;

	range = max - min;

	for (i = 0; i < min; i++)
		lut[i] = 0;

	for (; i <= max; i++)
		lut[i] = (int)((255.0 * (i - min)) / range);

	for (; i < MXGRAY; i++)
		lut[i] = 255;

}

void pointTransformation(uchar *in, uchar *out, uchar lut[], int total)
{
	int i;

	for (i = 0; i < total; i++)
	{
		out[i] = lut[in[i]];
	}
}


int findmax(int ft[])
{
	int i, max;

	max = ft[0];
	for (i = 1; i < 256; i++)
		if (max < ft[i]) { max = ft[i]; }

	return(max);
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

