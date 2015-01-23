#include <math.h>
#include <string.h>
#include <stdlib.h>
void bf_max_min(const int * buf, unsigned int  size, int *max, int *min)
{
    *max = buf[0];
    *min = buf[0];
    unsigned int i = 0;
    for (i = 0; i < size; i++){
        if (buf[i] > *max) *max = buf[i];
        if (buf[i] < *min) *min = buf[i];
    }
}

double bf_mean(const int *buf, unsigned int size)
{
    double sum = 0;
    unsigned int i = 0;
    for (i = 0; i < size; i++){
        sum += buf[i];   
    }
    return sum / (double)size;
}

double bf_log2(double x)
{
    return log(x)/log(2);
}

//fliter:a[1]y[n] = b[1]x[n]+b[2]x[n-1]+...+b[N]x[n-B+1]-a[2]y[n-1]-...-a[N]y[n-N+1]
void filter(int * dataY,int sizeY,double *b,int sizeB,double *a,int sizeA)
{   
    int * dataX =  (int *)malloc(sizeof(int)*sizeY);
    int i = 0;
    for(i = 0;i < sizeY;i++) {
        dataX[i] = dataY[i];
        dataY[i] = 0;
    }
    int j = 0;
    for(i = 0;i < sizeY;i++) {
        for(j = 0;j < sizeB && j <= i;j++)
            dataY[i] += b[j]*dataX[i-j];

        for(j = 1;j < sizeA && j <= i;j++ )
            dataY[i] -= a[j]*dataY[i-j];

        dataY[i] = dataY[i]/a[0];
    }


    free(dataX);
}

void filtering(int * dataY,unsigned int size,int sr)
{
	//butterworth filter coff:
	//sr:200
	//cutoff:30

//	double B30[] = {0.0913,0.1826,0.0913};
//	double A30[] = {1.0000,-0.9824,0.3477};
	double B30[] = {0.1311, 0.2622, 0.1311};
	double A30[] = {1.0000, -0.7478, 0.2722};
    double dMean = bf_mean(dataY,size);
    int i = 0;
    for(i = 0;i < size;i++) dataY[i] = dataY[i] - dMean;
    const double pi = 3.14;

    double b[] = {0.2,0.2,0.2,0.2,0.2};
    double a[] = {1,0,0};
    int sizeB = sizeof(b)/sizeof(double);
    int sizeA = sizeof(a)/sizeof(double);

    filter(dataY,size,b,sizeB,a,1);

    double T = (double)1/sr;
    double Fc = 1;
    double c1 = (double)1/(1+tan(Fc*pi*T));
    double c2 = (1-tan(Fc*pi*T))/(1+tan(Fc*pi*T));

    b[0] = c1,b[1] = -c1,a[0] = 1,a[1] = -c2;
    sizeB = 2, sizeA = 2;
    
    filter(dataY,size,b,sizeB,a,sizeA);
    filter(dataY,size,B30,sizeof(B30)/sizeof(double),A30,sizeof(A30)/sizeof(double));
}

int gcd(int x, int y)
{
    while (x != y) {
        if (x > y) x-=y;
        else y -= x;
    }
    return (x);
}

int down_sample(int vin, int *vout, int ifreq, int ofreq)
{	
    static int m = 0, n = 0, mn = 0, it = 0, ot = 0;
    static int last_val = 0;
    static int ngcd = 0;
    static int init = 1;
    if (init){
        ngcd = gcd (ifreq, ofreq);
        m = ifreq/ngcd;
        n = ofreq/ngcd;
        mn = n*m;
        last_val = vin;
        init = 0;
        return 0;
    } else{
        if (ot > mn){
            ot -= mn;
            it -= mn;
        }
        ot += n;

        if (it > ot){
            last_val = vin;
            return 0;
        }
        *vout = last_val + (it%n)*(vin - last_val)/n;
        it +=m;
        return 1;
    }
}

