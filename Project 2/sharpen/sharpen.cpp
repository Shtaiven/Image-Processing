#include "IP.h"
#include <vector>
#include <algorithm>
using namespace std;

// function prototypes
void sharpen(imageP, int, int, imageP);
void blur(imageP, int, int, imageP);
void fillPaddedBuffer(uchar*, int, uchar*, int, int);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine.
//
int main(int argc, char** argv)
{
	int       sz, fctr;
	imageP    I1, I2;

	// error checking: proper usage
	if (argc != 5) {
		cerr << "Usage: sharpen infile sz fctr outfile\n";
		exit(1);
	}

	// read input image (I1) and reserve space for output (I2)
	I1 = IP_readImage(argv[1]);
	I2 = NEWIMAGE;

	// read sz and fctr
	sz = atoi(argv[2]);
	fctr = atoi(argv[3]);

	// error checking: sz is odd
	if (sz % 2 == 0) {
		cerr << "sharpen: sz must be odd\n";
		exit(1);
	}

	// call median and save result in file
	sharpen(I1, sz, fctr, I2);
	IP_saveImage(I2, argv[4]);

	// free up image structures/memory
	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Sharpen:
//
// Sharpen implementation.
//
void sharpen(imageP I1, int sz, int fctr, imageP I2) {
	// init variables
	int total = I1->width * I1->height;
	int i;
	float p;

	// Blur input image I1 into I2 with xsz = ysz = sz
	blur(I1, sz, sz, I2);

	// Subtract blur image (stored in I2) from I1 and store in I2. Clip values to [0, 255] and scale I2 by fctr
	for (i = 0; i < total; ++i) {
		p = (float) fctr * (I1->image[i] - I2->image[i]); // subract I2 value from I1 value and scale
		p += (float) I1->image[i]; // add difference back to original
		if (p < 0) I2->image[i] = 0; // clipping when < 0
		else if (p > 255) I2->image[i] = 255; // clipping when > 255
		else I2->image[i] = (uchar) p; // place value into I2
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// blur:
//
// Blurs the image using a kernel of size xsz*ysz
//
void blur(imageP I1, int xsz, int ysz, imageP I2)
{

	int	     i, j, sum, currentColumn, currentRow;
	uchar    *in, *out;

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
	in                         = I1->image;            // input  image buffer
	out                        = I2->image;            // output image buffer
	int hpad                   = (xsz-1)/2;            // calculate horizontal padding
	int vpad                   = (ysz-1)/2;            // calculate vertical padding
	int hbuf_width             = I1->width + (xsz-1);  // xsz-1 accounts for padding on both sides
	int vbuf_width             = I1->height + (ysz-1); // ysz-1 accounts for padding on both sides
	float p;				   // stores value of output pixel
	uchar hbuf[hbuf_width];    // initialize horizontal buffer
	uchar vbuf[vbuf_width];    // initialize vertical buffer
	uchar temp_in[I1->height]; // used the make vertical blur simpler ot implement

	// blur horizontally
	for(currentRow = 0; currentRow < I1->height; ++currentRow) {
		fillPaddedBuffer(hbuf, sizeof(hbuf), in + (currentRow * I1->width), I1->width, hpad); // pads our input row of the image and puts it in a temp buffer
		sum = 0;
		for(i = -hpad; i <= hpad; ++i) { // start computing the sum within the horizonal kernel area (starting from the left of the image)
			sum += (int) hbuf[hpad+i];
		}
		for(currentColumn = 0; currentColumn < I1->width; ++currentColumn) {
			p = (float) sum/xsz;
			out[currentColumn + (currentRow * I1->width)] = (uchar) p;
			sum += ((int) hbuf[currentColumn+xsz] - (int) hbuf[currentColumn]);
		}
	}

	//store horizontally blurred output into in[] and vertically blur that output
	for(i = 0; i < sizeof(in); ++i) {
		in[i] = out[i];
	}

	// blur vertically
	for(currentColumn = 0; currentColumn < I1->width; ++currentColumn) {

		// fill temp_in with a column of the image: {x, x+w, x+2w, x+3w, .. , x+(h-1)*w}
		for (i = 0; i < I1->height; ++i) {
			temp_in[i] = in[currentColumn+(i*I1->width)]; // i*I1->width is how far the next pixel in the column is from the first
		}

		fillPaddedBuffer(vbuf, sizeof(vbuf), temp_in, sizeof(temp_in), vpad); // pads our input column of the image and puts it in a temp buffer
		sum = 0;
		for(i = -vpad; i <= vpad; ++i) {
			sum += (int) vbuf[vpad+i];
		}
		for(currentRow = 0; currentRow < I1->height; ++currentRow) {
			p = (float) sum/ysz;
			out[currentColumn + (currentRow * I1->width)] = (uchar) p;
			sum += ((int) vbuf[currentRow+ysz] - (int) vbuf[currentRow]);
		}
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// fillPaddedBuffer:
//
// Fills a single buffer with padding for use by blur().
//
// insz is the width or height of the image.
//
void fillPaddedBuffer(uchar *buffer, int bsz, uchar *in, int insz, int pad)
{
	// padding (value extension) + copy array + padding (value extension)
	int i;
	for(i = 0; i < pad; ++i) {
		buffer[i] = in[0]; // pad front of buffer with first value of in[]
		buffer[bsz - i - 1] = in[insz - 1]; // pad end of buffer with last value of in[]
	}
	for(i = 0; i < insz; ++i) {
		buffer[pad + i] = in[i]; // insert in[] into buffer[] in between padding
	}
}