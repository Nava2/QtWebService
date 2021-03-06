
project(QtWebService)

cmake_minimum_required(VERSION 2.8.11)

SET(QtWebService_LIB_NAME "QtWebService")
SET(QtWebService_LIB_VERSION "0.0.1")

IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "DEBUG")
    message(STATUS "Setting CMAKE_BUILD_TYPE to: DEBUG")
ENDIF()

IF( ${CMAKE_BUILD_TYPE} MATCHES "^DEBUG$" )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wuninitialized" )
ENDIF()

IF(MINGW)
    SET(CMAKE_CXX_FLAGS "-std=gnu++0x ${CMAKE_CXX_FLAGS}")
    SET(BUILD_CHAIN "mingw")
ELSEIF(UNIX)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # using Clang
        set ( CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
        SET(BUILD_CHAIN "clang")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # using GCC
        SET( CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
        SET(BUILD_CHAIN "gcc")
    endif()

    IF(APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
        message(STATUS "Apple Machine detected, Using libc++")
    ENDIF()
ENDIF()

message(STATUS "Module path: ${CMAKE_MODULE_PATH}")
message(STATUS "Found build chain: ${BUILD_CHAIN}")

SET(INCLUDE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set( CMAKE_AUTOMOC ON )
include_directories( ${CMAKE_BINARY_DIR} )

# Qt5:
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Xml REQUIRED)

message(STATUS "Found Qt version = ${Qt5Core_VERSION_STRING}")

find_package(QHttpServer REQUIRED)

message(STATUS "Found QHttpServer at = ${QHTTPSERVER_INCLUDE_DIRS}, LIB = ${QHTTPSERVER_LIBRARIES}")

include_directories(${QHTTPSERVER_INCLUDE_DIRS})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQTWEBSERVICE_EXPORT")

SET(QtWebService_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/")
SET(QtWebService_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# QtWebService:

SET( QtWebService_SRCS
    #general:
    lib/QWebService.cpp
    lib/QWebServiceConfig.cpp

    lib/router/QWebRequest.cpp
    lib/router/QWebResponse.cpp

    #router:
    lib/router/QWebRouter.cpp
    lib/router/QWebRoute.cpp
)

SET( QtWebService_PUBLIC_HEADER
    include/QWebService.h
    include/QWebServiceConfig.h

    include/router/QWebRequest.h
    include/router/QWebResponse.h

    include/router/QWebRouter.h
    include/router/QWebRoute.h

    include/test/TestUtils.h
)

SET( QtWebService_PRIVATE_HEADER

#    include/private/qtwebserviceapi.h
    include/private/qtwebservicefwd.h
)

SET( QtWebService_CMAKE_CONFIG
    "${QtWebService_LIB_NAME}Config.cmake"
)

SET( QtWebService_PC_CONFIG
    "lib${QtWebService_LIB_NAME}.pc"
    )

INCLUDE_DIRECTORIES(${QtWebService_SRC_DIR} ${QtWebService_INCLUDE_DIR})

add_library(QtWebService SHARED
    ${QtWebService_SRCS}
    ${QtWebService_PUBLIC_HEADER}
    ${QtWebService_PRIVATE_HEADER}
)

target_link_libraries(QtWebService
        #installed libs:
        Qt5::Network
        Qt5::Core
        Qt5::Xml

        ${QHTTPSERVER_LIBRARIES}
    )

# configure:
SET(BINARY_SHARE_PATH "${CMAKE_CURRENT_BINARY_DIR}/share")
SET(INSTALL_SHARE_PATH "share/${QtWebService_LIB_NAME}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/share/CMakeConfig.cmake"
    "${BINARY_SHARE_PATH}/${QtWebService_CMAKE_CONFIG}" @ONLY )

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/share/libpkgconfig.pc"
    "${BINARY_SHARE_PATH}/${QtWebService_PC_CONFIG}" @ONLY )

install(
    FILES
        "${BINARY_SHARE_PATH}/${QtWebService_PC_CONFIG}"
        "${BINARY_SHARE_PATH}/${QtWebService_CMAKE_CONFIG}"
    DESTINATION ${INSTALL_SHARE_PATH} )

# INSTALL components:

install(TARGETS QtWebService
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION "include/${QtWebService_LIB_NAME}"
        FILES_MATCHING REGEX ".*\\.h(pp)?$" )

add_subdirectory(test)
