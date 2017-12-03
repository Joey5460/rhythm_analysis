# Rhythm Analysis
This is an algorithm for detecting VF and VT used by AED. 

Requirements
-----------
sudo dnf install swig
sudo dnf install redhat-rpm-config
sudo dnf install python2-devel
sudo dnf install python3-devel
sudo dnf install python2-tkinter

pip install --user numpy
pip install --user matplotlib

How to use
----------
- Download wfdb, then complie and install(ref: https://www.physionet.org/physiotools/wfdb-linux-quick-start.shtml)

	**NB**: ./configure --prefix=/somewhere/else
    Typing: setenv LD_LIBRARY_PATH /somewhere/else/lib
    Otherwise, do this by typing:
      LD_LIBRARY_PATH='/somewhere/else/lib'
      export LD_LIBRARY_PATH
  
- Install swig if it is not installed
- Enter lib_swig, type python setup.py build_ext --inplace

- Enter vt_detect, type make.
- Download mitdb from https://physionet.org/physiobank/database/mitdb/ to project's folder.
- When making cmd gets successful, run db_test.sh.

  **NB**: When using setwfdb in db_test.sh, absolute path should be used. 
- Enter the plot_ecg, run sen_spe.py

Or readme.sh may do that stuff automatically
