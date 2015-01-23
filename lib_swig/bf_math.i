%module bf_math
%{
#define SWIG_FILE_WITH_INT
#include "bf_math.h"
%}
%include "carrays.i"
%array_class(int, intArray);
void filtering(int * dataY,unsigned int size,int sr);
%include "typemaps.i"
%apply int *OUTPUT {int *vout};
int down_sample(int vin, int *vout, int ifreq, int ofreq);

