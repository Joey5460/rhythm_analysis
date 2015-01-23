#!/usr/bin/env python
#
# File: rdann.py       I. Henry      28 March 2005
#                      last revised: 13 January 2013
#
# Minimal WFDB sample reader written in Python, based on rdsamp.c
# 							   
# Copyright (C) 2013 Isaac C. Henry (ihenry42@gmail.org)
# This file is part of wfdb-swig.
#
# wfdb-swig is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# wfdb-swig is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with wfdb-swig.  If not, see <http://www.gnu.org/licenses/>.

import getopt, wfdb, sys, os
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider
lib_path = os.path.abspath('../lib_swig')
sys.path.append(lib_path)
import bf_math
from scipy.signal import butter, lfilter,filtfilt
import numpy as np
#global ln
#global sld_samp
#global y_axes 
def filtering(data, fs):
    fs = fs/2.0
    b,a = butter(1, 0.1/fs)
    data = lfilter(b, a, data)
#    b,a = butter(2, 35/fs)
#    data = lfilter(b, a, data)
    b, a=butter(2, 0.5/fs, btype='highpass')
    y = lfilter(b, a, data)
    return y
def filtering2(data, sf, kind=1):
    data = data - np.mean(data)
    b = [0.2,0.2,0.2,0.2,0.2]
    a=[1]
    if kind == 1:
        data = lfilter(b,a,data)
    elif kind == 2:
        data = filtfilt(b,a,data)
    T = 1.0/sf
    Fc = 1
    c1 = 1.0/(1+np.tan(Fc*3.14*T))
    c2 = (1-np.tan(Fc*3.14*T))/(1+np.tan(Fc*3.14*T))
    b= [c1, -c1]
    a = [1,-c2]
    if kind == 1:
        data = lfilter(b,a,data)
    elif kind == 2:
        data = filtfilt(b,a,data)
    fh = sf/2.0
    mb = 2
    b,a = butter(mb,30/fh)
    if kind == 1:
        data = lfilter(b,a,data)
    elif kind == 2:
        data = filtfilt(b,a,data)
    return  data
def main(argv):
    db_path = "/opt/physiobank/database"
    record = ''
    wfdb.setwfdb(db_path)
    # Parse the arguments
    try:
        opts, args = getopt.getopt(argv, "hr:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        elif opt == '-r':
            record = arg

    # Read the number of signals in the record
    nsig = wfdb.isigopen(record, None, 0)

    # Exit if the record is not found, or there are no signals
    if nsig < 1:
        usage()
        sys.exit(2)

    siarray = wfdb.WFDB_SiginfoArray(nsig)
    wfdb.isigopen(record, siarray, nsig)

    n = 0
    v = wfdb.intArray(nsig)
    lf = bf_math.intArray(siarray[0].nsamp)
    global sld_samp
    global ln
    global y_axes
    global sr_ds  
    global subplt
    sr = (int)(wfdb.sampfreq(record))
    print "sr:",sr
    sr_ds =sr 
    y_axes = []
    # Loop over each sample and print the signal values.
    while wfdb.getvec(v) > 0:
        for i in range(0,1):
            y_axes.append (v[i])
        n = n + 1
    y_axes =filtering2(y_axes,sr)
    _nsamp = siarray[0].nsamp
    
    x_axes = xrange(_nsamp*sr_ds/sr)
    print "x_axes:", _nsamp*sr_ds/sr
    print "y_axes:", len(y_axes)
    plt.figure(1)
    subplt = plt.subplot(111)
    ln, = plt.plot(xrange(0,5*sr_ds), y_axes[0:5*sr_ds] )
    #plt.axis([-100, 100])
    plt.ylim(-800,800)
    #subplt.autoscale_view(True,True,True)
    axcolor = 'lightgoldenrodyellow'
    axsamp = plt.axes([0.25, 0.15, 0.65, 0.03], axisbg=axcolor)
    sld_samp = Slider(axsamp, 'samp', 0.1, _nsamp/sr-10 ,valinit=1)

    sld_samp.on_changed(update)
    plt.show()    
    wfdb.wfdbquit()
def update(val):
    global sld_samp
    global ln
    global y_axes
    global sr_ds
    global subplt
    x = sld_samp.val
    ln.set_ydata(y_axes[(int)(x*sr_ds):(int)((x+5)*sr_ds)])
    #ln.set_xdata(xrange((int)(x*sr_ds),(int)((x+5)*sr_ds)))
    #subplt.autoscale_view(True,True,True)
def usage():
    print "Usage: rdsamp.py -r record"

if __name__ == "__main__":
    main(sys.argv[1:])
