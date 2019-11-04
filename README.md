# GNURadio out-of-tree module for CCSDS/ECSS compatible reception/transmission

## Quick Install instructions
Requirements:
- GNURadio (http://git.gnuradio.org/git/gnuradio.git contains git submodules)

Dependencies:
- libfec (http://ka9q.net/code/fec/ see notes below)
- libldpc (https://gitlab.lrz.de/sdr/ldpc)

The dependencies can be automatically downloaded and build alongside gr-ccsds by providing the cmake option `-DCCSDS_BUILD_DEPS=On`

Build:
- cd gr-ccsds
- mkdir build && cd build
- cmake -DCCSDS_BUILD_DEPS=On ../
- make
- sudo make install

## Install instructions
The gr-ccsds module requires a GNURadio install, as well as the fec library from Phil Karn, KA9Q and the LDPC library.

By default only the gr-ccsds module is build. Its dependencies are expected to be installed to a system path already.

CMake can be configured to download and install all dependencies (or a few of them) along with gr-ccsds. The flag `CCSDS_DEPS_<DEPENDENCY>=On/Off` enabled or disables this behavior. The default value for all dependencies can be set by the option `CCSDS_BUILD_DEPS`, which in turn defaults to Off (i.e. no dependencies are build). If `CCSDS_BUILD_DEPS` is set to On individual dependencies can still be used from the system paths by explicitly disabling this component with `CCSDS_DEPS_<DEPENDENCY>=Off`.

### fec install instructions
Whether or not the fec library should be downloaded and installed along gr-ccsds can be controlled by the cmake option `CCSDS_DEPS_FEC=On/Off`.

The instructions on how to install the library by hand is described [here](ka9q_fec.md).

### ldpc install instructions
Whether or not the ldpc library should be downloaded and installed along gr-ccsds can be controlled by the cmake option `CCSDS_DEPS_LDPC=On/Off`.

The instructions on how to install the library by hand is described in the library repository https://gitlab.lrz.de/sdr/ldpc

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
