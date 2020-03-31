%template(constellation_ccsds_sptr) boost::shared_ptr<gr::digital::constellation>;

%template(constellation_ccsds_bpsk_sptr) boost::shared_ptr<gr::ccsds::constellation_ccsds_bpsk>;
%pythoncode %{
constellation_ccsds_bpsk_sptr.__repr__ = lambda self: "<CCSDS BPSK constellation>"
constellation_ccsds_bpsk = constellation_ccsds_bpsk.make;
%}

%template(constellation_ccsds_qpsk_sptr) boost::shared_ptr<gr::ccsds::constellation_ccsds_qpsk>;
%pythoncode %{
constellation_ccsds_qpsk_sptr.__repr__ = lambda self: "<CCSDS QPSK constellation>"
constellation_ccsds_qpsk = constellation_ccsds_qpsk.make;
%}