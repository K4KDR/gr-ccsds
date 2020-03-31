%template(frame_sync_config_sptr) boost::shared_ptr<gr::ccsds::frame_sync_config>;
%pythoncode %{
frame_sync_config_sptr.__repr__ = lambda self: "<CCSDS Fame sychronization config>"
frame_sync_config = frame_sync_config.make;
%}

%template(frame_generation_config_sptr) boost::shared_ptr<gr::ccsds::frame_generation_config>;
%pythoncode %{
frame_generation_config_sptr.__repr__ = lambda self: "<CCSDS Fame generation config>"
frame_generation_config = frame_generation_config.make;
%}