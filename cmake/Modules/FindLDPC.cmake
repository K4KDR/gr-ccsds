# - Try to find LDPC (libldpc)
# Once done this will define
#  LIBLDPC_FOUND - System has LDPC
#  LIBLDPC_INCLUDE_DIRS - The LDPC include directories
#  LIBLDPC_LIBRARIES - The libraries needed to use LDPC
#  LIBLDPC_DEFINITIONS - Compiler switches required for using LDPC

find_path(LIBLDPC_INCLUDE_DIR ldpc/ldpc.h
          PATH_SUFFIXES libldpc )

find_library(LIBLDPC_LIBRARY NAMES ldpc libldpc )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBLDPC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LibLDPC  DEFAULT_MSG
                                  LIBLDPC_LIBRARY LIBLDPC_INCLUDE_DIR)

mark_as_advanced(LIBLDPC_INCLUDE_DIR LIBLDPC_LIBRARY )

set(LIBLDPC_LIBRARIES ${LIBLDPC_LIBRARY} )
set(LIBLDPC_INCLUDE_DIRS ${LIBLDPC_INCLUDE_DIR} )
