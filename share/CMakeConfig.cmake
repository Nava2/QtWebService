# - Try to find qhttpserver
# Once done this will define
#  QHTTPSERVER_FOUND - System has qhttpserver
#  QHTTPSERVER_INCLUDE_DIRS - The qhttpserver include directories
#  QHTTPSERVER_LIBRARIES - The libraries needed to use qhttpserver
#  QHTTPSERVER_DEFINITIONS - Compiler switches required for using qhttpserver

find_package(PkgConfig)

pkg_check_modules(PC_QHTTPSERVER QUIET @qhttpserver_LIB_NAME@-@qhttpserver_LIB_VERSION@)

set(QHTTPSERVER_DEFINITIONS ${PC_QHTTPSERVER_CFLAGS_OTHER})

find_path(QHTTPSERVER_INCLUDE_DIR @qhttpserver_LIB_NAME@/qhttpserver.h
          HINTS ${PC_QHTTPSERVER_INCLUDEDIR} ${PC_QHTTPSERVER_INCLUDE_DIRS}
          PATH_SUFFIXES qhttpserver )

find_library(QHTTPSERVER_LIBRARY NAMES @qhttpserver_LIB_NAME@ lib@qhttpserver_LIB_NAME@
             HINTS ${PC_QHTTPSERVER_LIBDIR} ${PC_QHTTPSERVER_LIBRARY_DIRS} )

set(QHTTPSERVER_LIBRARIES ${QHTTPSERVER_LIBRARY} )
set(QHTTPSERVER_INCLUDE_DIRS ${QHTTPSERVER_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set QHTTPSERVER_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(qhttpserver  DEFAULT_MSG
                                  QHTTPSERVER_LIBRARY QHTTPSERVER_INCLUDE_DIR)

mark_as_advanced(QHTTPSERVER_INCLUDE_DIR QHTTPSERVER_LIBRARY )