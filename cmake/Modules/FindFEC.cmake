# - Try to find FEC (libfec)
# Once done this will define
#  LIBFEC_FOUND - System has FEC
#  LIBFEC_INCLUDE_DIRS - The FEC include directories
#  LIBFEC_LIBRARIES - The libraries needed to use FEC
#  LIBFEC_DEFINITIONS - Compiler switches required for using FEC

find_path(LIBFEC_INCLUDE_DIR fec.h
          PATH_SUFFIXES libfec )

find_library(LIBFEC_LIBRARY NAMES fec libfec )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBFEC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibFEC  DEFAULT_MSG
                                  LIBFEC_LIBRARY LIBFEC_INCLUDE_DIR)

mark_as_advanced(LIBFEC_INCLUDE_DIR LIBFEC_LIBRARY )

set(LIBFEC_LIBRARIES ${LIBFEC_LIBRARY} )
set(LIBFEC_INCLUDE_DIRS ${LIBFEC_INCLUDE_DIR} )
