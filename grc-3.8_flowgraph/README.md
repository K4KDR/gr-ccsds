
Good working flowgraph for GNU Radio 3.8

Important note ref. MOVE-II download packets... only every 6th packet is BPSK.   So, do NOT expect a decode on every packet.   However, the other 5 CW (or similar) packets are very helpful for you to CENTER the demodulated packet in the flowgraph.   

Example:  if your recording is a 20 kHz USB window, with careful doppler tracking the center of your packet should be at the 10k point.   You get the idea.

NOTE!   File path MUST be updated in the 'LDPC Decoder (CCSDS)' block!!



![Screenshot from 2023-12-30 04-32-16](https://github.com/K4KDR/gr-ccsds/assets/19886368/10bed5b8-3585-4875-bd67-5551fcf0baf1)

