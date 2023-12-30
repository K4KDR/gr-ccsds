# GNURadio out-of-tree module for CCSDS/ECSS compatible reception/transmission

## Quick Install instructions
Requirements:
- GNURadio (http://git.gnuradio.org/git/gnuradio.git contains git submodules)

Dependencies:
- libfec (http://ka9q.net/code/fec/ see notes below)
- libldpc (https://gitlab.lrz.de/sdr/ldpc)

The dependencies are automatically downloaded and build alongside gr-ccsds. This can be disabled by providing the cmake option `-DCCSDS_BUILD_DEPS=Off`

***
*** K4KDR Notes specific to this fork ***
--- please note that all dependency repositories have been copied here as well.   So, following the BUILD instructions below, I'm hoping that the dependencies will be sourced from the locations listed in the .gitmodules file.   If not, then the next best option would be to install them manually ---
***
***

Build:
```
cd gr-ccsds
mkdir build && cd build
cmake -DCCSDS_BUILD_DEPS=On ../
make
sudo make install
```

## Install instructions
The gr-ccsds module requires a GNURadio install, as well as the fec library from Phil Karn, KA9Q and the LDPC library.

By default the libfec and libldpc dependencies of gr-ccsds are build as well build. If this is disabled by setting `CCSDS_BUILD_DEPS=Off` the dependencies must be installed to a system path before running cmake.

CMake can be configured to download and install all dependencies (or a few of them) along with gr-ccsds. The flag `CCSDS_DEPS_<DEPENDENCY>=On/Off` enables or disables this behavior. The default value for all dependencies can be set by the option `CCSDS_BUILD_DEPS`, which in turn defaults to On (i.e. all dependencies are build). If `CCSDS_BUILD_DEPS` is set to On individual dependencies can still be used from the system paths by explicitly disabling this component with `CCSDS_DEPS_<DEPENDENCY>=Off`.

### fec install instructions
Whether or not the fec library should be downloaded and installed along gr-ccsds can be controlled by the cmake option `CCSDS_DEPS_FEC=On/Off`.

The instructions on how to install the library by hand is described [here](ka9q_fec.md).

### ldpc install instructions
Whether or not the ldpc library should be downloaded and installed along gr-ccsds can be controlled by the cmake option `CCSDS_DEPS_LDPC=On/Off`.

The instructions on how to install the library by hand is described in the library repository https://gitlab.lrz.de/sdr/ldpc

### gr-ccsds install instructions
The build process is controlled by Cmake.

A typical build process looks like this:
```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCCSDS_BUILD_DEPS=On ../
make
make test
sudo make install
```

Afterwards the environment variables `PYTHONPATH` should contain the path `/usr/local/lib/python2.7/dist-package` and `LD_LIBRARY_PATH` should contain `/usr/local/lib`. Changes to the latter might require a run of `sudo ldconfig` to take effect.

If there are issues with swig, or missing libraries, the library search path needs to be updated with `ldconfig` (as root)
