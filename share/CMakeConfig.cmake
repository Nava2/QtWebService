# - Try to find QtWebService
# Once done this will define
#  QTWEBSERVICE_FOUND - System has qhttpserver
#  QTWEBSERVICE_INCLUDE_DIRS - The qhttpserver include directories
#  QTWEBSERVICE_LIBRARIES - The libraries needed to use qhttpserver
#  QTWEBSERVICE_DEFINITIONS - Compiler switches required for using qhttpserver

find_package(PkgConfig)

pkg_check_modules(PC_QTWEBSERVICE QUIET @QtWebService_LIB_NAME@-@QtWebService_LIB_VERSION@)

set(QTWEBSERVICE_DEFINITIONS ${PC_QTWEBSERVICE_CFLAGS_OTHER})

find_path(QTWEBSERVICE_INCLUDE_DIR @QtWebService_LIB_NAME@/QWebService.h
          HINTS ${PC_QTWEBSERVICE_INCLUDEDIR} ${PC_QTWEBSERVICE_INCLUDE_DIRS}
          PATH_SUFFIXES QtWebService )

find_library(QTWEBSERVICE_LIBRARY NAMES @QtWebService_LIB_NAME@ lib@QtWebService_LIB_NAME@
             HINTS ${PC_QTWEBSERVICE_LIBDIR} ${PC_QTWEBSERVICE_LIBRARY_DIRS} )

set(QTWEBSERVICE_LIBRARIES ${QTWEBSERVICE_LIBRARY} )
set(QTWEBSERVICE_INCLUDE_DIRS ${QTWEBSERVICE_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set QTWEBSERVICE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(QtWebService  DEFAULT_MSG
                                  QTWEBSERVICE_LIBRARY QTWEBSERVICE_INCLUDE_DIR)

mark_as_advanced(QTWEBSERVICE_INCLUDE_DIR QTWEBSERVICE_LIBRARY )