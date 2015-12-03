# Install script for directory: /home/marina/Documents/sdr1/gr-ccsds/grc

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/gnuradio/grc/blocks" TYPE FILE FILES
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_randomiser.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_reversebits.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_msg_null_src.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_ticp_frame_source.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_ticp_frame_sink.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_blob_msg_source_b.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_blob_msg_sink_b.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_conv_decode27.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_mpsk_ambiguity_resolver_f.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_rs_encode.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_rs_decode.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_pll_cc.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_dll_cc.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_add_asm.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_trunk_tail.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_softbits_msg_to_bytes_b.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_mpsk_detector_soft_cf.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_mpsk_mod_bc.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_conv_encode27_bb.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_mpsk_preamble_cc.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_softbittobit.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_simple_bpsk_SNR_qf.xml"
    "/home/marina/Documents/sdr1/gr-ccsds/grc/ccsds_discriminator.xml"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

