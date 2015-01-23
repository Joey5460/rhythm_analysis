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
#global ln
#global sld_samp
#global y_axes 
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
    sr_ds = 50
    # Loop over each sample and print the signal values.
    while wfdb.getvec(v) > 0:
        for i in range(0,1):
            lf[n] = v[i]
        n = n + 1
    bf_math.filtering(lf,siarray[0].nsamp,sr)
    _nsamp = siarray[0].nsamp
    
    x_axes = xrange(_nsamp*sr_ds/sr)
    print "x_axes:", _nsamp*sr_ds/sr
    y_axes = []
    for i in xrange(_nsamp):
         b, val = bf_math.down_sample(lf[i], sr, sr_ds)
         if (b):
             y_axes.append(val)
    print "y_axes:", len(y_axes)
    plt.figure(1)
    subplt = plt.subplot(111)
    ln, = plt.plot(xrange(0,5*sr_ds), y_axes[0:5*sr_ds] )
    #plt.axis([-100, 100])
    plt.ylim(-300,300)
    #subplt.autoscale_view(True,True,True)
    axcolor = 'lightgoldenrodyellow'
    axsamp = plt.axes([0.25, 0.15, 0.65, 0.03], axisbg=axcolor)
    sld_samp = Slider(axsamp, 'samp', 0.1, 33*60, valinit=1)

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
