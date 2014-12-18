
IF (NOT QtWebService_LIB_NAME )
    message(fatal_error "Can not configure from this directory")
ENDIF()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)


find_package(QHttpServer REQUIRED)

SET( QtWebService_testsrcs
    QWebRouteTest.cpp
    QWebServiceTest.cpp
    QWebRouterTest.cpp
    
    catch/catch.hpp
)

include_directories(${INCLUDE_OUTPUT_DIR})
include_directories("../")

add_executable(qwebservice-test
    main.cpp
    
    ${QtWebService_testsrcs})
    
add_dependencies(qwebservice-test
        QtWebService
    )

target_link_libraries(qwebservice-test
        Qt5::Network 
        Qt5::Core 
        
        ${QHTTPSERVER_LIBRARIES}
        QtWebService
)
