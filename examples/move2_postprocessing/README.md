# MOVE-II postprocessing flowgraph

This flowgraph processes a recorded and dopler corrected basebandfile for MOVE-II (a/b). It outputs the decoded blocks with additional metadata (in a detatched header file).

## Decoding Instructions

#### First time preparations
If you run the postprocessing for the first time, or after updating the gr-ccsds module, run the following steps:

0. Install GNURadio 3.8
1. Compile and install this `gr-ccsds` module (make sure you have the correct branch) according to the [main README.md](../../README.md).
2. Copy the LDPC parity matrix file `AR4JA_r12_k1024n.a` to `/usr/opt/GnuRadio/`
3. Convert the flowgraph `move2_postprocessing.grc` to a python script by either running `grcc move2_postprocessing.grc` or opening it in `gnuradio-companion` and hi the generate button.

### Decoding
1. Download a baseband recording from the groundstation, or the [recordings page](https://recordings.move.lrt.ed.tum.de) (the file should have the extension `.dat` and a suffix of `-wo_doppler` or `-dopplercorrected`. **Do not change the filename**.
2. Run the script `prepare_recording_metadata.py` with the baseband recording as parameter. This script will parse the filename and write the parsed information into a GNURadio metadata file. The metadata filename is the same, as the original recording with an added `.hdr` extension.
3. Run the script `move2_postprocessing.py -i <baseband recording> -r 25e3 -o <output filename>`. The script will run for quite a while without output (a few minutes on a fast PC). Wait for the program to complete. The script will generate two output files. `<output filename>` contains the decoded block bytes (The as the Decoded file on the recordings page) and `<output filename>.hdr` contains metadata for each decoded block in GNURadio metadata format. These files can now be processed with the `beaconparser_meta` program (available in [this](https://gitlab.lrz.de/mondbaron/move-ii-beaconparser) repository in the `beaconparser-metadata` branch).
