# - Find libmetis
# Find the native libmetis includes and library
#
# LIBMETIS_INCLUDES - where to find METIS.h
# LIBMETIS_LIBRARIES - List of libraries when using LIBMETIS.
# LIBMETIS_FOUND - True if libmetis found.

set(LIBMETIS_ROOT "" CACHE STRING "libmetis root directory")

find_path(LIBMETIS_INCLUDE_DIR metis.h 
    HINTS "${LIBMETIS_ROOT}/include")

find_library(LIBMETIS_LIBRARY
   metis
   HINTS "${LIBMETIS_ROOT}/lib")

set(LIBMETIS_LIBRARIES ${LIBMETIS_LIBRARY})
set(LIBMETIS_INCLUDE_DIRS ${LIBMETIS_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set LIBMETIS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBMETIS DEFAULT_MSG
                                  LIBMETIS_LIBRARY LIBMETIS_INCLUDE_DIR)

mark_as_advanced(LIBMETIS_INCLUDE_DIR LIBMETIS_LIBRARY)

