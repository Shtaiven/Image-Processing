// ============================================================================

// qntz.cpp - Quantization program.
// Copyright (C) 2015 by Rosario Antunez, Steven Eisinger, and Ethan Graber
//
// Written by: Rosario Antunez, Steven Eisinger, and Ethan Graber, 2015

// ============================================================================

//#include "stdafx.h"
#include "IP.h"
using namespace std;



// function prototype
void qntz(imageP, int, imageP);
void initLUT(uchar[], int);
void pointTransformation(uchar*, uchar*, uchar[], int);



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine to quantize image.
//
int
main(int argc, char** argv)
{
	int    n;
	imageP    I1, I2;

	// error checking: proper usage
	if (argc != 4) {
		cerr << "Usage: qntz infile n outfile\n";
		exit(1);
	}

	// read input image (I1) and reserve space for output (I2)
	I1 = IP_readImage(argv[1]);
	I2 = NEWIMAGE;

	// read lower and upper thresholds
	n = atoi(argv[2]);

	// threshold image and save result in file
	qntz(I1, n, I2);
	IP_saveImage(I2, argv[3]);

	// free up image structures/memory
	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}



// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// qntz:
//
// 
// The following rule is used for quantizing:
//
//    v_out = ceiling(v_in*n/MXGRAY)*ceiling(128/n)
//   
//    No quantizing occurs when n = MXGRAY.
//
//

void qntz(imageP I1, int n, imageP I2)
{
	int     total;
	uchar    *in, *out, lut[MXGRAY];

	// total number of pixels in image
	total = I1->width * I1->height;

	// init I2 dimensions and buffer
	I2->width = I1->width;
	I2->height = I1->height;
	I2->image = (uchar *)malloc(total);
	if (I2->image == NULL) {
		cerr << "qntz: Insufficient memory\n";
		exit(1);
	}

	// init lookup table
	initLUT(lut, n);


	// visit all input pixels and apply lut to quantize
	in = I1->image;    // input  image buffer
	out = I2->image;    // output image buffer

	pointTransformation(in, out, lut, total);

}

// initialize look-up table
void initLUT(uchar lut[], int n)
{
	int i, interval;
	
	int scale = (int) (MXGRAY/n);
	int bias = (int)((MXGRAY / 2) / n);

	for (i = 0; i < (scale*n); i++)
	{ 
		interval = (int)(i / scale);
		lut[i] = bias + interval * scale;
	}

	for (; i < MXGRAY; i++)
		lut[i] = (int)((MXGRAY + (n* scale)) / 2);

		
}

void pointTransformation(uchar *in, uchar *out, uchar lut[], int total)
{
	int i;

/*	cout << 50 << " " << (int) lut[50] << "\n";
	cout << 51 << " " << (int) lut[51] << "\n";
	cout << 110 << " " << (int) lut[110] << "\n";
	cout << 200 << " " << (int) lut[200] << "\n";
	cout << 250 << " " << (int)lut[250] << "\n";
	cout << 255 << " " << (int) lut[255] << "\n";
*/
	for (i = 0; i < total; i++)
	{
		out[i] = lut[in[i]];
	}
}
