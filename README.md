# rhythm_analysis
How to use
----
- Download wfdb, then complie and install(ref: https://www.physionet.org/physiotools/wfdb-linux-quick-start.shtml)

	**NB**: ./configure --prefix=/somewhere/else
    Typing: setenv LD_LIBRARY_PATH /usr/local/lib
    Otherwise, do this by typing:
      LD_LIBRARY_PATH='/usr/local/lib'
      export LD_LIBRARY_PATH
  
- Install swig if it is not installed
- Enter lib_swig, type python setup.py install
- Enter vt_detect, type make, 
- When making cmd gets successful, run db_test.sh.

  **NB**: When using setwfdb in db_test.sh, absolute path should be used. 
- Enter the plot_ecg, run sen_spe.py
