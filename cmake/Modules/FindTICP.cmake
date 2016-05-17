# - Try to find TICP (libticp)
# Once done this will define
#  LIBTICP_FOUND - System has TICP
#  LIBTICP_INCLUDE_DIRS - The TICP include directories
#  LIBTICP_LIBRARIES - The libraries needed to use TICP
#  LIBTICP_DEFINITIONS - Compiler switches required for using TICP

find_path(LIBTICP_INCLUDE_DIR ticp/Ticp.hpp
          PATH_SUFFIXES libticp )

find_library(LIBTICP_LIBRARY NAMES ticp libticp )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBTICP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibTICP  DEFAULT_MSG
                                  LIBTICP_LIBRARY LIBTICP_INCLUDE_DIR)

mark_as_advanced(LIBTICP_INCLUDE_DIR LIBTICP_LIBRARY )

set(LIBTICP_LIBRARIES ${LIBTICP_LIBRARY} )
set(LIBTICP_INCLUDE_DIRS ${LIBTICP_INCLUDE_DIR} )
