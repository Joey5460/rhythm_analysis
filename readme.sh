#!/usr/bin/bash
ls|grep wfdb
if ! [ $? -eq 0 ] ; then
    wget -c https://www.physionet.org/physiotools/wfdb.tar.gz
    tar xfvz wfdb.tar.gz
fi
dwfdb=$(ls|grep wfdb-)
pwd_wfdb=$(pwd)
#pwd_wfdb=${PWD} # for enviromental variable
pushd $dwfdb
./configure --prefix=$pwd_wfdb
make install
make check
popd

yum_cmd=""
if [ -x "$(command -v apt)" ]; then
   echo "ubuntu-like"
   yum_cmd="apt-get"
elif [ -x "$(command -v dnf)" ]; then
   echo "fedora-like"
   yum_cmd="dnf"
fi

if ! [ -x "$(command -v swig)" ]; then
    sudo $yum_cmd install swig 
fi
sudo $yum_cmd install redhat-rpm-config
sudo $yum_cmd install python2-devel
sudo $yum_cnd install python3-devel
sudo $yum_cmd install python2-tkinter

pip install numpy
pip install matplotlib

pushd lib_swig
python setup.py build_ext --inplace
popd
pushd  vt_detect
make
popd

