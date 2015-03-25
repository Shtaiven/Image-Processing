#include "IP.h"
#include <vector>
#include <algorithm>
using namespace std;

// function prototypes
void sharpen(imageP, int, int, imageP);
void blur(imageP, int, int, imageP);
void cbuf(uchar**, imageP, int, int, int, int, int);
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
	// TODO: write function
	/*
		1. Blur input image I1 into I2 with xsz = ysz = sz
		2. Subtract blur image (stored in I2) from I1 and store in I2. Make sure to clip values [0, 255]
		3. Scale I2 by fctr
		4. Add I1 to I2
	*/
	// init variables
	int total = I1->width * I1->height;
	int i;
	float p;

	// Blur input image I1 into I2 with xsz = ysz = sz
	blur(I1, sz, sz, I2);

	// Subtract blur image (stored in I2) from I1 and store in I2. Clip values to [0, 255] and scale I2 by fctr
	for (i = 0; i < total; ++i) {
		p = (float) fctr * (I1->image[i] - I2->image[i]);
		p += (float) I1->image[i];
		if (p < 0) I2->image[i] = 0;
		else if (p > 255) I2->image[i] = 255;
		else I2->image[i] = (uchar) p;
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// blur:
//
// Blur implementation.
//
void blur(imageP I1, int xsz, int ysz, imageP I2) {

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


	// init variables
	int i, j, currentRow, currentCol, sum;
	int	xpadsz = (xsz-1)/2;
	int ypadsz = (ysz-1)/2;
	//int medianIndex = (xsz*ysz-1)/2;
	//int denominator = 2*avg_nbrs + 1;
	vector<uchar> blurVector;

	// init buffer to be used by cbuf
	uchar** buffer = new uchar*[xsz];
	for (i = 0; i < ysz; ++i) {
		buffer[i] = new uchar[I1->width + xsz - 1];
	}

	// run the kernel over the circular buffer
	for (currentRow = 0; currentRow < I1->height; ++currentRow) {
		cbuf(buffer, I1, currentRow, xpadsz, ypadsz, xsz, ysz);
		// pass the kernel through the buffer
		for (currentCol = 0; currentCol < I1->width; ++currentCol) {
			blurVector.clear();
			sum = 0;
			for (i = 0; i < ysz; ++i) {
				for (j = 0; j <= sizeof(buffer[i])-xsz; ++j) {
					blurVector.push_back(buffer[i][j + currentCol + xpadsz]);
				}
			}
			//sort(medianVector.begin(), medianVector.end());
			for (i = 0; i <= sizeof(blurVector); ++i) {
				sum += blurVector[i];
			}
			I2->image[currentRow*I1->width + currentCol] = sum/sizeof(blurVector);
		}
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// cbuf:
//
// Circular buffer implementation.
//
void cbuf(uchar** buffer, imageP image, int currentRow, int xpadsz, int ypadsz, int xsz, int ysz) {
	int i;
	if(currentRow == 0) { // need to pad top if at beginning on the image with ypadsz rows
		for(i = 0; i < ypadsz; ++i) {
			fillPaddedBuffer(buffer[i], sizeof(buffer[i]), image->image, image->width, xpadsz); // create top padding rows
			fillPaddedBuffer(buffer[i+ypadsz+1], sizeof(buffer[i]), image->image + i*image->width, image->width, xpadsz); // pad rows after first
		}
		fillPaddedBuffer(buffer[ypadsz], sizeof(buffer[i]), image->image, image->width, xpadsz); // pad first row of input image
	}
	else if(currentRow >= image->height - ypadsz) { // need to pad bottom of image if close to bottom of image
		uchar* temp_buf = buffer[0];
		for(i = 0; i < ysz - 1; ++i) {
			buffer[i] = buffer[i+1];
		}
		buffer[xsz] = temp_buf;
		fillPaddedBuffer(buffer[ysz], sizeof(buffer[ysz]), image->image + (image->height - 1)*image->width, image->width, xpadsz); //
	}
	else {
		uchar* temp_buf = buffer[0];
		for(i = 0; i < ysz - 1; ++i) {
			buffer[i] = buffer[i+1];
		}
		buffer[ysz] = temp_buf;
		fillPaddedBuffer(buffer[ysz], sizeof(buffer[ysz]), image->image + (currentRow + xpadsz)*image->width, image->width, xpadsz);
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