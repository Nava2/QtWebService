
IF (NOT QtWebService_LIB_NAME )
    message(fatal_error "Can not configure from this directory")
ENDIF()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

find_package(Qt5Test REQUIRED)

SET( QtWebService_testsrcs
#    qhttproutepathtest.cpp
    QWebRouteTest.cpp
)

include_directories(${INCLUDE_OUTPUT_DIR})
include_directories("../")

add_executable(qwebservice-test
    main.cpp
    
    ${QtWebService_testsrcs})
    
add_dependencies(qwebservice-test
        QtWebService
    )

LINK_LIBRARIES(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

target_link_libraries(qwebservice-test
        
        #installed libs:
        Qt5::Network 
        Qt5::Core 
        Qt5::Test
        ${QHTTPSERVER_LIBRARIES}
        QtWebService
)
