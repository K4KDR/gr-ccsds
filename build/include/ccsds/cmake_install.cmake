# Install script for directory: /home/marina/Documents/sdr1/gr-ccsds/include/ccsds

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ccsds" TYPE FILE FILES
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/api.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/randomiser.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/reversebits.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/msg_null_src.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/add_asm.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/ticp_frame_source.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/ticp_frame_sink.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/trunk_tail.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/pll_cc.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/dll_cc.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/blob_msg_sink_b.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/blob_msg_source_b.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/softbits_msg_to_bytes_b.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/rs_encode.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/rs_decode.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/mpsk_ambiguity_resolver_f.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/conv_decode27.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/conv_encode27_bb.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/conv_puncturing27.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/mpsk_mod_bc.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/mpsk_detector_soft_cf.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/mpsk_preamble_cc.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/softbittobit.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/simple_bpsk_SNR_qf.h"
    "/home/marina/Documents/sdr1/gr-ccsds/include/ccsds/discriminator.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

