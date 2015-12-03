#!/bin/sh
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/marina/Documents/sdr1/gr-ccsds/lib
export PATH=/home/marina/Documents/sdr1/gr-ccsds/build/lib:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
qa_ccsds_conv_encode27_aux 
