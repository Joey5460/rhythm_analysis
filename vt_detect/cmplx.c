/*Implement VT detection method proposed by the paper of detecting ventirbular tachycardia and fibrillation by complexity measure
 * 
 * Joey
 * Sep 28, 2014
 */
#include <stdio.h>/*include for NULL*/
#include <stdlib.h>/*include for malloc*/
#include <string.h>/*include for memset and strstr*/
#include "bf_lib/bf_math.h"
double ecg_complexity_measure(const int *data, size_t size)
{
    char *S;
    char *Q;
    int *buf;
    S = (char*)malloc(size + 1);
    Q = (char*)malloc(size + 1);
    buf = (int*)malloc(size*sizeof(int));
    
    memset(S, '\0', size);

    int  nMax, nMin;
    double fMean; 

    fMean = bf_mean(data, size);


    unsigned int i = 0;
    /*substract the mean from the original data*/
    for(i = 0; i < size; i++){
        buf[i] = data[i] - fMean;       
    }

    bf_max_min(buf, size, &nMax, &nMin);

    /*calculate the positive count and negative count*/
    unsigned int pc = 0, nc = 0;
    for (i = 0; i < size; i++){
        if (buf[i] > 0 && buf[i] < 0.1*nMax) pc++;
        else if (buf[i] < 0 && buf[i] > 0.1*nMin) nc++;
    }
    
    /*calulate the threshold*/
    double td = 0;
    if ((pc + nc) < 0.4*size) td = 0.0;
    else if (pc < nc) td = 0.2*nMin;
    else td = 0.2*nMax;

    /*coding the date into 0-1 and calculating complexity measure*/
    if (buf[0] < td) S[0] = '0';
    else S[0] = '1';
    
    unsigned int cm = 1;/*complexity measure*/

    unsigned int j = 0;
    for(i = 1; i < size; i++){
        if (buf[i] < td) {
            Q[j] = '0';
        } else {
            Q[j] = '1';
        }
        
        /*find the Q in the S*/
        Q[j+1] = '\0';
        char * ret = strstr(S,Q);
        S[i] = Q[j++];

        /*The Q not exist in the S*/
        if (NULL == ret){
            cm ++;
            j = 0;
        }
    }

    free(S);
    free(Q);

   /*normalizing the complexity measure*/ 
    double b = 0.0;
    b = size/bf_log2((double)size);
    return (double)(cm/b);
}

