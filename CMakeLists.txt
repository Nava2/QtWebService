
project(qwebapplication)

cmake_minimum_required(VERSION 2.8.11)

SET(QHTTPSERVER_MAIN true)

SET(qwebapplication_LIB_NAME "QtWebApp")
SET(qwebapplication_LIB_VERSION "0.0.1")


IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE "DEBUG")
    message(STATUS "Setting CMAKE_BUILD_TYPE to: DEBUG")
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

message(STATUS "Found build chain: ${BUILD_CHAIN}")

SET(INCLUDE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/include")
SET(INCLUDE_OUTPUT_UTIL_DIR "${INCLUDE_OUTPUT_DIR}/util")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_AUTOMOC ON)

# Qt5:
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(QtWebServer REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DQWEBAPPLICATION_EXPORT")

SET(qwebapplication_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/")

# qwebapplication:

SET( qwebapplication_SRCS
    #router:
    src/router/qhttproute.cpp
    src/router/qhttprouter.cpp
    src/router/qhttproutedrequest.cpp

)

SET( qwebapplication_PUBLIC_HEADER
    src/router/qhttprouter.h
    src/router/qhttproute.h
    src/router/qhttproutedrequest.h
)

SET( qwebapplication_CMAKE_CONFIG
    "${qwebapplication_LIB_NAME}Config.cmake"
)

SET( qwebapplication_PC_CONFIG
    "lib${qwebapplication_LIB_NAME}.pc"
    )

add_custom_target(qwebapplication_headers)
INCLUDE_DIRECTORIES(${INCLUDE_OUTPUT_DIR})

IF("${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE")
    MESSAGE(STATUS "Copying Headers for release")

    foreach(header ${qwebapplication_PUBLIC_HEADER})
        add_custom_command(
            TARGET qwebapplication_headers
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${header} ${INCLUDE_OUTPUT_DIR}/${header}
            MAIN_DEPENDENCY ${header})
    endforeach()
else()
    INCLUDE_DIRECTORIES( ${qwebapplication_SRC_DIR})
endif()

include_directories(${INCLUDE_OUTPUT_DIR})

add_library(qwebapplication SHARED
    ${qwebapplication_SRCS}
    ${qwebapplication_PUBLIC_HEADER}
    ${http-parser_SRCS}
)

add_dependencies(qwebapplication
    qwebapplication_headers
)

target_link_libraries(qwebapplication
        #installed libs:
        Qt5::Network
        Qt5::Core
    )

# configure:
SET(BINARY_SHARE_PATH "${CMAKE_CURRENT_BINARY_DIR}/share")
SET(INSTALL_SHARE_PATH "share/${qwebapplication_LIB_NAME}")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/share/CMakeConfig.cmake"
    "${BINARY_SHARE_PATH}/${qwebapplication_CMAKE_CONFIG}" )

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/share/libpkgconfig.pc"
    "${BINARY_SHARE_PATH}/${qwebapplication_PC_CONFIG}" )

install(
    FILES
        "${BINARY_SHARE_PATH}/${qwebapplication_PC_CONFIG}"
        "${BINARY_SHARE_PATH}/${qwebapplication_CMAKE_CONFIG}"
    DESTINATION ${INSTALL_SHARE_PATH} )

# INSTALL components:

install(TARGETS qwebapplication
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)

install(DIRECTORY src/
        DESTINATION "include/${qwebapplication_LIB_NAME}"
        FILES_MATCHING PATTERN "*.h" )

add_subdirectory(test)
