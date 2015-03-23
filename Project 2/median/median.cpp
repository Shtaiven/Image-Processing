#include "IP.h"
#include <vector>
#include <algorithm>
using namespace std;

// function prototype
void median(imageP, int, int, imageP);
void cbuf(uchar**, imageP, int, int, int);
void fillPaddedBuffer(uchar*, int, uchar*, int, int);

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// main:
//
// Main routine.
//
int main(int argc, char** argv)
{
	int       xsz, avg_nbrs;
	imageP    I1, I2;

	// error checking: proper usage
	if (argc != 5) {
		cerr << "Usage: median infile sz avg_nbrs outfile\n";
		exit(1);
	}

	// read input image (I1) and reserve space for output (I2)
	I1 = IP_readImage(argv[1]);
	I2 = NEWIMAGE;

	// read xsize and ysize
	xsz = atoi(argv[2]);
	avg_nbrs = atoi(argv[3]);

	// error checking: xsz and ysz are odd
	if (xsz % 2 == 0) {
		cerr << "median: sz must be odd\n";
		exit(1);
	}

	// call median and save result in file
	median(I1, xsz, avg_nbrs, I2);
	IP_saveImage(I2, argv[4]);

	// free up image structures/memory
	IP_freeImage(I1);
	IP_freeImage(I2);

	return 1;
}

void median(imageP I1, int xsz, int avg_nbrs, imageP I2) {

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
	int	padsz = (xsz-1)/2;
	int medianIndex = (xsz*xsz-1)/2;
	int denominator = 2*avg_nbrs + 1;
	vector<uchar> medianVector;

	// init buffer to be used by cbuf
	uchar** buffer = new uchar*[xsz];
	for (i = 0; i < xsz; ++i) {
		buffer[i] = new uchar[I1->width + xsz - 1];
	}

	// run the kernel over the circular buffer
	for (currentRow = 0; currentRow < I1->height; ++currentRow) {
		cbuf(buffer, I1, currentRow, padsz, xsz);
		// pass the kernel through the buffer
		for (currentCol = 0; currentCol < I1->width; ++currentCol) {
			medianVector.clear();
			sum = 0;
			for (i = 0; i < xsz; ++i) {
				for (j = -padsz; j <= padsz; ++j) {
					medianVector.push_back(buffer[i][j + currentCol + padsz]);
				}
			}
			sort(medianVector.begin(), medianVector.end());
			for (i = -avg_nbrs; i <= avg_nbrs; ++i) {
				sum += medianVector[i + medianIndex];
			}
			I2->image[currentRow*I1->width + currentCol] = sum/denominator;
		}
	}
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// cbuf:
//
// Circular buffer implementation.
//
void cbuf(uchar** buffer, imageP image, int currentRow, int padsz, int xsz) {
	int i;
	if(currentRow == 0) { // need to pad top if at beginning on the image
		// copy input array into buffer
		for(i = 0; i < padsz; ++i) {
			fillPaddedBuffer(buffer[i], sizeof(buffer[i]), image->image, image->width, padsz);
			fillPaddedBuffer(buffer[i+padsz+1], sizeof(buffer[i]), image->image + i*image->width, image->width, padsz);
		}
		fillPaddedBuffer(buffer[padsz], sizeof(buffer[i]), image->image, image->width, padsz);
	}
	else if(currentRow >= image->height - padsz) {
		uchar* temp_buf = buffer[0];
		for(i = 0; i < xsz - 1; ++i) {
			buffer[i] = buffer[i+1];
		}
		buffer[xsz] = temp_buf;
		fillPaddedBuffer(buffer[xsz], sizeof(buffer[xsz]), image->image + (image->height - 1)*image->width, image->width, padsz);
	}
	else {
		uchar* temp_buf = buffer[0];
		for(i = 0; i < xsz - 1; ++i) {
			buffer[i] = buffer[i+1];
		}
		buffer[xsz] = temp_buf;
		fillPaddedBuffer(buffer[xsz], sizeof(buffer[xsz]), image->image + (currentRow + padsz)*image->width, image->width, padsz);
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