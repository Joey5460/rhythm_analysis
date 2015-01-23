#include <stdio.h>
#include <wfdb/wfdb.h>
#include <unistd.h>
#include "bf_lib/fifo.h"
#include "bf_lib/bf_math.h"
#include "cmplx.h"
#include "psr.h"
#include "cpsd.h"
#include "osea_lib/qrsdet.h"
#include "osea_lib/bdac.h"
int check_vt_ann(WFDB_Time begin_samp, WFDB_Time end_samp, WFDB_Annotation *begin_ann, WFDB_Annotation *end_ann);
int check_ann(WFDB_Time begin_samp, WFDB_Time end_samp,
        WFDB_Annotation *begin_ann, WFDB_Annotation *end_ann,
        const char* aux);
int main(int argc, char **argv)
{
    int c;
    int inflag = 0;
    int dbg = 0;
    int sr_ds = 200;
    char * rec_name = "vfdb/427";
    char * db_path = "/opt/physiobank/database";
    size_t win_sec = 8;
    /* r stands for record with folder
     * p for path
     * i for information of record
     * s downsample sr
     * w window length default:8
     * d debug
     */
    while ((c = getopt(argc, argv, "idr:p:s:w:")) != -1)
        switch (c){
            case 'i':
                inflag = 1; 
                break;
            case 'r':
                rec_name = optarg;
                break;
            case 'p':
                db_path = optarg;
                break;
            case 's':
                sr_ds= atoi(optarg);
                break;
            case'w':
                win_sec = atoi(optarg);
                break;
            case'd':
                dbg = 1;
                break;
            default:
                abort();
        }            
    int i, j, nsig;
    WFDB_Sample *v;
    WFDB_Siginfo *s;
    WFDB_Anninfo a;
    setwfdb(db_path);
    
    nsig = isigopen(rec_name, NULL, 0);
    if (nsig < 1){
        printf("nsig:%d\n",nsig);
        exit(1);
    }
    s = (WFDB_Siginfo *)malloc(nsig * sizeof(WFDB_Siginfo));
    if (isigopen(rec_name, s, nsig) != nsig)
        exit(1);
    v = (WFDB_Sample *)malloc(nsig * sizeof(WFDB_Sample));

    int orig_sr = sampfreq(rec_name);
    a.name = "atr"; a.stat = WFDB_READ;
    if (wfdbinit(rec_name, &a, 1, s, nsig) != nsig) exit(3);
    if(1 == inflag ){
        printf("sr:%d\n",orig_sr);
        printf("%d signals\n", nsig);
        for (i = 0; i < nsig; i++) {
            printf("Group %d, Signal %d:\n", s[i].group, i); 
            printf("File: %s\n", s[i].fname);
            printf("Description: %s\n", s[i].desc);
            printf("Gain: ");
            if (s[i].gain == 0.) 
                printf("uncalibrated; assume %g", WFDB_DEFGAIN);
            else printf("%g", s[i].gain);
            printf(" adu/%s\n", s[i].units ? s[i].units : "mV");
            printf(" Initial value: %d\n", s[i].initval);
            printf(" Storage format: %d\n", s[i].fmt);
            printf(" I/O: ");
            if (s[i].bsize == 0) printf("can be unbuffered\n");
            else printf("%d-byte blocks\n", s[i].bsize);
            printf(" ADC resolution: %d bits\n", s[i].adcres);
            printf(" ADC zero: %d\n", s[i].adczero);
            if (s[i].nsamp > 0L) {
                printf(" Length: %s (%ld sample intervals)\n",
                       timstr(s[i].nsamp), s[i].nsamp);
                printf(" Checksum: %d\n", s[i].cksum);
            }
            else printf(" Length undefined\n");
        }
    }

    fifo_t fifo_ecg;
    int fifo_ecg_buf[FIFO_SIZE];
    fifo_init(&fifo_ecg, fifo_ecg_buf, FIFO_SIZE);

    fifo_t fifo_bt;
    int fifo_bt_buf[FIFO_SIZE];
    fifo_init(&fifo_bt, fifo_bt_buf, FIFO_SIZE);

    int tmp = 0;
    int sr = 200;
    WFDB_Time begin_samp = 0;
    WFDB_Time end_samp = orig_sr*win_sec;
    WFDB_Annotation begin_ann;
    WFDB_Annotation end_ann;
    getann(0, &begin_ann);
    while(0 == getann(0, &end_ann))
         if ((end_ann.aux != NULL && *end_ann.aux > 0)
                    ||0 == strcmp(annstr(end_ann.anntyp), "[")
                    ||0 == strcmp(annstr(end_ann.anntyp), "]")
                    ||0 == strcmp(annstr(end_ann.anntyp), "~")
                ){
            break;
        }
    
    int * pBt_len = (int*)calloc(win_sec,sizeof(int));
    ResetBDAC();
    for (; ;) {
        if (getvec(v) < 0)
            break;
//        for (j = 0; j < nsig; j++){
//        }
        tmp = v[nsig-1];
        tmp = v[0];
        int vout1 = 0;
        static int bt_i = 0;
        static unsigned int samplecnt = 0;
        int idx = 0;
        if(down_sample(tmp, &vout1, orig_sr, sr)) {
                samplecnt ++;
                fifo_write(&fifo_ecg, &vout1, 1);
                int beatType, beatMatch;
                long ltmp = vout1-s[0].adczero;
                ltmp *= 200; ltmp /= s[0].gain;
                int bdac_dly = BeatDetectAndClassify(ltmp, &beatType, &beatMatch);
                idx = bt_i/sr;
                if (0 != bdac_dly ) {
                    pBt_len[idx]++;
                    fifo_write(&fifo_bt, &beatType, 1);
                }
                bt_i = ++bt_i%(win_sec*sr);
        }

        double cm = 0.0;

        int size = win_sec*sr;
        if(fifo_len(&fifo_ecg) >= sr*win_sec){
            int * win_data = (int*)malloc(win_sec*sr*sizeof(int));
            int * ds_data = (int*)malloc(win_sec*sr_ds*sizeof(int));
            
            int len = fifo_len(&fifo_bt);
            int * p = (int*)calloc(len, sizeof(int));
           
            if (0 != len){
               //printf("bt_i:%d\n", bt_i);
                fifo_read_steps(&fifo_bt, p, len, pBt_len[bt_i/sr]);
                //for (i = 0;i< len;i++) printf("%d ", p[i]);
                //printf("\n");

            }
            pBt_len[bt_i/sr] = 0;
            
            fifo_read_steps(&fifo_ecg, win_data, size, sr);
            filtering(win_data, size, sr);
            int i = 0;
            int ds_size = 0;
            int vout;
            for(i = 0;i < size;i++){
               // if(down_sample(win_data[i], &vout, sr, sr_ds)) ds_data[ds_size++] = vout;
            }
            double dven = 0.0;

            if (0 != len){
                int tmp_cnt = 0;
                for(i = 0; i < len ; i++){
                    if(5 == p[i]) dven ++;
                }
                //printf("div:%d %d\n", tmp_cnt, len);
                dven /= len; 
            }
                         
            //cm = ecg_complexity_measure(win_data, size);
            //cm = ecg_complexity_measure(ds_data, ds_size);
            //cm = cpsd(ds_data, ds_size, 0.5*sr_ds);
            //cm = calc_grid(ds_data, ds_size, 0.5*sr_ds);
            cm = calc_grid(win_data, size, 0.5*sr);


        
            if (-1 == cm ) continue;
            //VT print 1;
            //VF print 2;
            
            int hr =  (int)((double)(len*60)/win_sec+0.5);
            int ret = check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(VT");
            if (1 == ret){
                printf("%d %lf %lf %d\n", 1, cm, dven, hr);
            } else if (0 == ret) {
                ret = check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(VFL");
                int ret2 = check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "[");
                if (1 == ret || 1 == ret2){
                    printf("%d %lf %lf %d\n", 2, cm, dven, hr);
                } else if (0 == ret){
//                    ret = check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(N");
//                    int ret3 = check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "N");
//                    if ((1 == ret  || 1 == ret3) && -1 != begin_ann.subtyp) printf ("%d %lf %lf %d\n", 0, cm, dven, hr); 

                    if (1 == check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(AFIB") 
                        || 1 == check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(AFL")
                        || 1 == check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(IVR")
                        || 1 == check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(SVTA")
                        || 1 == check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(SBR")
                        || 1 == check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(BII")
                       ) printf ("%d %lf %lf %d\n", 0, cm, dven, hr); 
                    
                   // int ret = check_ann(begin_samp, end_samp, &begin_ann, &end_ann, "(SVTA");
                   // if (1 == ret)  printf ("%d %lf\n", 0, cm); 
                }
            }
            //iannsettime(begin_samp);
            begin_samp += orig_sr;
            end_samp += orig_sr;
            if(end_ann.time < begin_samp){
                begin_ann = end_ann;
                
                while(1)
                    if (0 != getann(0, &end_ann)){
                        end_ann = begin_ann;
                        /*the last sample of the signal*/
                        end_ann.time = s[nsig-1].nsamp;
                        break;
                    }else if ((end_ann.aux != NULL && *end_ann.aux > 0)
                                ||0 == strcmp(annstr(end_ann.anntyp), "[")
                                ||0 == strcmp(annstr(end_ann.anntyp), "]")
                                ||0 == strcmp(annstr(end_ann.anntyp), "~")
                            ){
                        break;
                    }
                if(dbg){
                    printf("begin tm:%s type:%s ", mstimstr(begin_ann.time), annstr(begin_ann.anntyp));
                    if(begin_ann.aux != NULL)
                        printf("begin aux:%s", begin_ann.aux+1);
                    printf("\n");
                    printf("end tm:%s type:%s ", mstimstr(end_ann.time), annstr(end_ann.anntyp));
                    if (end_ann.aux != NULL)
                        printf("end aux:%s", end_ann.aux+1);
                    printf("\n");

                    printf("begin sample:%s\n", mstimstr(begin_samp));
                    printf("ann diff:%s\n", mstimstr(end_ann.time-begin_ann.time));
                }
            }
            free(p);
            free(ds_data);
            free(win_data);
        }

    }
    free(pBt_len);
    wfdbquit();
    return 0;
}
/* The intial begin_ann time equal to 0;
 * Get a ann from begin_samp, and set the ann to end_ann;
 * If the begin_ann is vt:
 *      end_ann time is greater equal than end_samp, the result is 1.(VT)
 *      end_ann time is less than the end_samp, the result is 2.(anyway)
 * If the begin_ann is not vt:
 *      end_ann is vt and its time is greater than end_samp, the result is 0.(not VT)
 *      end_ann is vt and its time is less than end_samp, the result is 2.(anyway)
 *      end_ann is not vt, the result is 0.(not VT) 
 */

int check_ann(WFDB_Time begin_samp, WFDB_Time end_samp,
        WFDB_Annotation *begin_ann, WFDB_Annotation *end_ann,
        const char* aux)
{   
    if (NULL != begin_ann->aux 
            && *begin_ann->aux > 0 
            && 0 == strcmp(aux, begin_ann->aux+1)
            || 0 == strcmp(aux, annstr(begin_ann->anntyp))
             
            ){
        if (end_ann->time >= end_samp) return 1;
        else return 2;
    } else if(NULL != end_ann->aux 
                && *end_ann->aux > 0 
                && 0 == strcmp(aux, end_ann->aux+1)
                || 0 == strcmp(aux, annstr(begin_ann->anntyp))
                ) {
            if (end_ann->time > end_samp) return 0;
            else return 2;
    } else return 0;
}
