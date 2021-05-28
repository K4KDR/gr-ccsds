== MOVE-II postprocessing flowgraph ==

This flowgraph processes a recorded and dopler corrected basebandfile for MOVE-II (a/b). It outputs the decoded blocks with additional metadata (in a detatched header file).

=== Instructions ===

1. (Only the first time) Copy the LDPC parity matrix file `AR4JA_r12_k1024n.a` to `/usr/opt/GnuRadio/`
2. Download a baseband recording from the groundstation, or recordings page (the file should have the extension `.dat` and a suffix of `wo_doppler` or `-dopplercorrected`. Do not change the filename.
3. Run the script `prepare_recording_metadata.py` with the baseband recording as parameter. This script will parse the filename and write the parsed information into a GNURadio metadata file with the same filename as the baseband recording, but an `.hdr`
4. (Only the first time) Convert the flowgraph `move2_postprocessing.grc` to a python script by either running `grcc move2_postprocessing.grc` or opening it in `gnuradio-companion` and hi the generate button.
5. Run the script `move2_postprocessing.py -i <baseband recording> -r 25e3 -o <output filename>`. Once the script is done it will have generated a `output filename` file with the decoded bytes and a `<output filename>.hdr` file with the GNURadio metadata. These files can now be processed with the `beaconparser_meta` program.
