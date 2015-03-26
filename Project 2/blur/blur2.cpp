#include "IP.h"
#include <vector>
#include <algorithm>
using namespace std;

// function prototype
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
		cerr << "blur: xsz and ysz must be odd\n";
		exit(1);
	}

	// call median and save result in file
	blur(I1, xsz, ysz, I2);
	IP_saveImage(I2, argv[4]);

	// free up image structures/memory
	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}

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
			fillPaddedBuffer(buffer[i], sizeof(buffer[i]), image->image, image->width, xpadsz); // top padded rows
			fillPaddedBuffer(buffer[i+ypadsz+1], sizeof(buffer[i]), image->image + i*image->width, image->width, xpadsz); // rows after first
		}
		fillPaddedBuffer(buffer[ypadsz], sizeof(buffer[i]), image->image, image->width, xpadsz); // first row
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