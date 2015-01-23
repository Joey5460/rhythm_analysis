#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt
fd = open ('../vt_detect/detect.out', 'r')
data = [map(float, line.split()) for line in fd]
#print M[0:1]
m_data = np.array(data, np.float16)
#print m_data[:,1]
s = set()
l_th = []
cm_vt = []
cm_vf = []
cm_usk = []
ls_usk = []
ls_vt = []
ls_vf = []
num_bins = 100 
# the histogram of the data
vf_flag = 1
for val in m_data:
    if val[0] == 0:
        cm_usk.append(val[1])
        ls_usk.append([val[1], val[2], val[3]])
    elif val[0] == 1:
        cm_vt.append(val[1])
        ls_vt.append([val[1], val[2], val[3]])
    elif val[0] == 2:
        cm_vf.append(val[1])
        ls_vf.append([val[1], val[2], val[3]])
if 0 != len(cm_usk):
    n_usk, bins_usk, patches = plt.hist(cm_usk, num_bins, weights = np.ones_like(cm_usk)/len(cm_usk), facecolor='green', alpha=0.5, label='usk')
if 0 != len(cm_vt) and vf_flag == 0:
    n_vt, bins_vt, patches = plt.hist(cm_vt, num_bins, weights = np.ones_like(cm_vt)/len(cm_vt), facecolor='orange', alpha=0.5, label='vt')
if 0 != len(cm_vf) and vf_flag == 1:
    #n, bins, patches = plt.hist(cm_vf, num_bins, normed=1, facecolor='red', alpha=0.5)
    n_vf, bins_vf, patches = plt.hist(cm_vf, num_bins, weights = np.ones_like(cm_vf)/len(cm_vf), facecolor='red', alpha=0.5, label='vf')
spe = []
sen_vf = []
sen_vt = []
if vf_flag == 1:
   # for val in xrange(len(bins_vf)-1, -1, -1):
    for val in xrange(len(bins_vf)):
        x = bins_vf[val]
        i, = np.where(bins_usk <= x)
        j, = np.where(bins_vf ==  x)
        if 0 < len(i) and 0 < len(j):
            spe.append(np.sum(n_usk[:i[-1]]))
            sen_vf.append(np.sum(n_vf[j[-1]:]))
else:
    ls_vt.sort(key=lambda ls:ls[1])
    ls_usk.sort(key = lambda ls:ls[1])
    l_vt = len(ls_vt)
    l_usk = len(ls_usk) 
    sen_cnt = l_vt
    spe_cnt = 0
    begin_i = 0
    for i in xrange(0, l_usk):
        if ls_usk[i][2] < 100: 
            spe_cnt+=1
    for i in xrange(l_vt):
        if 100 > ls_vt[i][2]:
            sen_cnt-=1
    for val in xrange(l_vt):
        if ls_vt[val][2] >= 100:
            sen_cnt-=1
        for i in xrange(begin_i, l_usk):
            if  ls_usk[i][1] < ls_vt[val][1]:
                if ls_usk[i][2] >= 100:
                    spe_cnt+=1
            else:
                break
            begin_i+=1
        sen_vt.append((float)(sen_cnt)/l_vt)
        spe.append((float)(spe_cnt)/l_usk)    

#    for val in xrange(len(bins_vt)-1, -1, -1):
#        x = bins_vt[val]
#        i, = np.where(bins_usk <= x)
#        j, = np.where(bins_vt ==  x)
#        if 0 < len(i) and 0 < len(j):
#            spe.append(1 - np.sum(n_usk[:i[-1]]))
#            sen_vt.append(np.sum(n_vt[j[-1]:]))
plt.xlabel('complexity')
plt.ylabel('Probability')
plt.legend()
plt.figure(2)
if vf_flag == 1:
    plt.plot([1-x for x in spe[::-1]],  sen_vf[::-1])
else:
    plt.plot([1-x for x in spe[::-1]], sen_vt[::-1])
plt.xlabel('(1-specificity)')
plt.ylabel('sensitivity')
#plt.plot(sep, sen_vt)
plt.show()
#for q in sorted(set(q) for q in m_data[:,1]):
#for q in m_data[:,1]:
#   s.add(q) 
#for val_th in sorted(s):
#    l_th.append(val_th)
#    print val_th
