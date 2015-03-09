========================================================================

Copyright (C) 2015 by Rosario Antunez, Steven Eisinger, and Ethan Graber

Written by: Rosario Antunez (mrantunez@gmail.com), 
			Steven Eisinger (steven.a.eisinger@gmail.com), and 
			Ethan Graber (ethangraber@gmail.com), 2015

========================================================================

qntz.cpp:

	Compliling: Go to directory /qntz/ and type make in terminal.

				Alternatively, type the following in terminal:
					g++ qntz.cpp IPutil.cpp IP.h -o qntz

	Usage:		qntz <input_filename> <n> <output_filename>

	n is an integer representing the number of graylevels to quantize to.

histo_plot:

	Compliling: Go to directory /histo_plot/ and type make in terminal.

				Alternatively, type the following in terminal:
					g++ histo_plot.cpp IPutil.cpp IP.h -o histo_plot

	Usage:		histo_plot <input_filename> <output_filename> <flag>

	flag can be 0 or 1.

	Advantages/disadvantages of method when flag = 0
	------------  Advantage  ---------------------
	1- The lower frequency portion of the histogram can be observed in more detail. 
	2- Clipping eliminates outliers on the high side. 
	-------------  Disadvantage ------------------
	Since we are clipping at 255 then we lose the relative proportion between the frequencies if any 
	frequency is larger than 255. Therefore, the top of the histogram is clipped.

	Advantages/disadvantages of method when flag = 1 
	------------  Advantage  -----------------------
	The whole curve can be seen. The outline of the top of the histogram can be seen. 
	------------  Disadvantage ---------------------
	If some frequencies are very large compared to other frequencies then the fine details of the 
	histogram for the samller frequencies cannot be observed. 

histo_stretch:

	Compliling: Go to directory /histo_stretch/ and type make in terminal.

				Alternatively, type the following in terminal:
					g++ histo_stretch.cpp IPutil.cpp IP.h -o histo_stretch

	Usage:		histo_stretch <input_filename> <thresh1> <thresh2> <output_filename>

	thresh1 and thresh2 are the brightness values you want to stretch between. -1 to 255 are allowable.
	A negative number means stretch from the first non-zero value found.

histo_match:

	Compliling: Go to directory /histo_match/ and type make in terminal.

				Alternatively, type the following in terminal:
					g++ histo_match.cpp IPutil.cpp IP.h -o histo_match

	Usage:		histo_match <input_filename> <n> <output_filename>

	n is an integer defining the shape of the output histogram.
	The resultant histogram's shape will resemble v^n, where v is constant.
	Negative, and positive integers are acceptable. 0 will give a flat resultant histogram.