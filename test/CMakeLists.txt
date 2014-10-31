
IF (NOT QHTTPSERVER_MAIN )
    message(fatal_error "Can not configure from this directory")
ENDIF()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

find_package(Qt5Test REQUIRED)

SET( qhttpserver_testsrcs 
#    qhttproutepathtest.cpp
    qhttproutetest.cpp
)

include_directories(${INCLUDE_OUTPUT_DIR})
include_directories("../")

add_executable(qhttpserver-test 
    main.cpp
    
    ${qhttpserver_testsrcs})
    
add_dependencies(qhttpserver-test
        qhttpserver
    )

LINK_LIBRARIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

target_link_libraries(qhttpserver-test 
        qhttpserver 
        
        #installed libs:
        Qt5::Network 
        Qt5::Core 
        Qt5::Test)
