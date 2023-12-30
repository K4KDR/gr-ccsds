
Suggested syntax to process .bin file created by GNU Radio flowgraph:

... if .bin file was saved as

/tmp/move-ii.bin

... run this command:

./beaconparser -7 -8 -r -i80 /tmp/move-ii.bin | more

(will output non-perfect decodes - 80% or better - so that you can know if you have some decodes even if not 100% clean)
