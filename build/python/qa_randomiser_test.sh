#!/bin/sh
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/marina/Documents/sdr1/gr-ccsds/python
export PATH=/home/marina/Documents/sdr1/gr-ccsds/build/python:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export DYLD_LIBRARY_PATH=$LD_LIBRARY_PATH:$DYLD_LIBRARY_PATH
export PYTHONPATH=/home/marina/Documents/sdr1/gr-ccsds/build/swig:$PYTHONPATH
/usr/bin/python2 /home/marina/Documents/sdr1/gr-ccsds/python/qa_randomiser.py 
