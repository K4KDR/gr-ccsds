# GNURadio out-of-tree module for CCSDS/ECSS compatible reception/transmission

## Quick Install instructions
Requirements:
- GNURadio
- libfec
- libldpc

Build:
- cmake
- make
- make install

## Install instructions
The gr-ccsds module requires a GNURadio install, as well as the fec library from Phil Karn, KA9Q and the LDPC library from https://gitlab.lrz.de/sdr/ldpc.

### libfec install instructions
You can download the base library from Phil Karns website: http://ka9q.net/code/fec
Or directly download the library from the command line with
````
wget --quiet http://ka9q.net/code/fec/fec-3.0.1.tar.bz2;
````

In order to use libfec with GNURadio on modern systems two patches are needed.

The first one is from http://lodge.glasgownet.com/2011/11/07/making-opendab-fec-and-the-psion-wavefinder-work-on-x64/
and it will disable all attempts to detect and run assembly code (which is for 32bit processors and thus incompatible)

It can be downloaded from the command line with
````
wget --quiet http://lodge.glasgownet.com/bitsnbobs/kg_fec-3.0.1.patch
````

The second patch if from us and will insert the `extern "C" { ... }` block around the header file to be able to use this C library from within C++.
Save the following code into a file called `extern_c_header.patch`
````
diff -Naur fec-3.0.1-origpatch/fec.h fec-3.0.1/fec.h
--- fec-3.0.1-origpatch/fec.h	2013-02-28 10:30:42.175998400 +0100
+++ fec-3.0.1/fec.h	2013-02-28 10:32:48.987994820 +0100
@@ -6,6 +6,11 @@
 #ifndef _FEC_H_
 #define _FEC_H_
 
+#ifdef __cplusplus
+extern "C"
+{
+#endif
+
 /* r=1/2 k=7 convolutional encoder polynomials
  * The NASA-DSN convention is to use V27POLYA inverted, then V27POLYB
  * The CCSDS/NASA-GSFC convention is to use V27POLYB, then V27POLYA inverted
@@ -341,6 +346,10 @@
 
 int cpu_features(void);
 
+#ifdef __cplusplus
+} /* END extern "C" */
+#endif
+
 #endif /* _FEC_H_ */
 
 

````

Make sure you have the fec archive and the two patches in the cwd. Their md5sums should be
````
52846b9eac4080e68ade7d2bd13eb504  fec-3.0.1.tar.bz2
78eee9c211fc2a8ca4d11c515f7fe194  kg_fec-3.0.1.patch
7cb15410de10125f2ba69593a295029e  extern_c_header.patch
````

Extract libfec
````
tar -xjf fec-3.0.1.tar.bz2
cd fec-3.0.1
````

Apply patches
````
patch < ../kg_fec-3.0.1.patch
patch < ../extern_c_header.patch
````

Install
````
./configure --prefix=/usr/local
make
sudo make install
````

### gr-ccsds install instructions
The build process is controlled by Cmake.

A typical build process looks like this:
````
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ../
make
make test
sudo make install
````

Afterwards the environment variables `PYTHONPATH` should contain the parth `/usr/local/lib/python2.7/dist-package` and `LD_LIBRARY_PATH` should contain `/usr/local/lib`. Changes to the latter might require a run of `sudo ldconfig` to take effect.