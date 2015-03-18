// ============================================================================
// blur.cpp - Blurring program.
// Copyright (C) 2015 by Rosario Antunez, Steven Eisinger, and Ethan Graber
//
// Written by: Rosario Antunez, Steven Eisinger, and Ethan Graber, 2015
// ============================================================================

#include "IP.h"
using namespace std;

// function prototype
void blur(imageP, int, int, imageP);
void fillPaddedBuffer(uchar*, int, uchar*, int, int, int);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine to blur image.
//
int main(int argc, char** argv)
{
	int       xsz, ysz;
	imageP    I1, I2;

	// error checking: proper usage
	if (argc != 5) {
		cerr << "Usage: blur infile xsz ysz outfile\n";
		exit(1);
	}

	// read input image (I1) and reserve space for output (I2)
	I1 = IP_readImage(argv[1]);
	I2 = NEWIMAGE;

	// read xsize and ysize
	xsz = atoi(argv[2]);
	ysz = atoi(argv[3]);

	// error checking: xsz and ysz are odd
	if (xsz % 2 == 0 || ysz % 2 == 0) {
		cerr << "xsz and ysz must be odd\n";
		exit(1);
	}

	// blur image and save result in file
	blur(I1, xsz, ysz, I2);
	IP_saveImage(I2, argv[4]);

	// free up image structures/memory
	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// blur:
//
// Blurs the image using a kernel of size xsz*ysz
//
void blur(imageP I1, int xsz, int ysz, imageP I2)
{

	int	     x, y;
	uchar    *in, *out, sum;

	// total number of pixels in image
	int total = I1->width * I1->height;
	

	// init I2 dimensions and buffer
	I2->width = I1->width;
	I2->height = I1->height;
	I2->image = (uchar *)malloc(total);
	if (I2->image == NULL) {
		cerr << "blur: insufficient memory\n";
		exit(1);
	}

	// init horizontal and vertical blur buffers
	in = I1->image;    // input  image buffer
	out = I2->image;    // output image buffer
	int hpad = (xsz-1)/2; // calculate horizontal padding
	int vpad = (ysz-1)/2; // calculate vertical padding
	int hbuf_width = I1->width + (xsz-1); // xsz-1 accounts for padding on both sides
	int vbuf_width = I1->height + (ysz-1); // ysz-1 accounts for padding on both sides
	uchar hbuf[hbuf_width]; // initialize horizontal buffer
	uchar vbuf[vbuf_width]; // initialize vertican buffer

	// blur horizontally
	int i;
	for(y = 0; y < total; y += I1->width) {
		fillPaddedBuffer(hbuf, hbuf_width, in + y, I1->width, hpad, 0);
		sum = 0;
		for(i = -hpad; i <= hpad; ++i) {
			sum += hbuf[hpad+i];
		}
		for(x = 0; x < I1->width; ++x) {
			out[x+y] = sum/xsz;
			sum += (hbuf[x+xsz]-hbuf[x]);
		}
	}

	// blur vertically

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// fillPaddedBuffer:
//
// Fills a single buffer with padding for use by blur().
//
// offset is the distance between array elements we want in the buffer.
// insz is the width or height of the image.
void fillPaddedBuffer(uchar *buffer, int bsz, uchar *in, int insz, int pad, int offset)
{
	// padding (value extension) + copy array + padding (value extension)
	int i;
	for(i = 0; i < pad; ++i) {
		buffer[i] = in[0]; // pad front of buffer with first value of in[]
		buffer[bsz - i - 1] = in[insz - 1]; // pad end of buffer with last value of in[]
	}
	for(i = 0; i < insz; ++i) {
		buffer[pad + i] = in[i + offset]; // insert in[] in between padding
	}
}