QtWebService
===========

A Qt Web Service framework for easily exposing C++ code to the Web via HTTP. 

[![Build Status](https://travis-ci.org/Nava2/QtWebService.svg?branch=master)](https://travis-ci.org/Nava2/QtWebService)

This server leverages the [QHttpServer project](https://github.com/nikhilm/qhttpserver) as the back end. 

QtWebService is available under the MIT License.

Installation
------------

Requires Qt 4 or Qt 5. (Untested on Qt 4)

    mkdir build/ && cd build/ && cmake .. && make -j2 && su -c "make install"

To link to your projects put this in your project's qmake project file

    LIBS += -lQtWebService

By default, the installation prefix is /usr/local. To change that to /usr,
for example, run:

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr

Usage
-----

IGNORE THIS SECTION

Include the headers

    #include <qhttpserver.h>
    #include <qhttprequest.h>
    #include <qhttpresponse.h>

Create a server, and create a lambda handler:

    QHttpServer *server = QHttpServer::Config()
    		.route("/", [](QHttpRequest* req, QHash<QString, QString> *vars, QHttpResponse* resp) {
    		    resp->writeHead(QHttpResponse::STATUS_OK);
    		    resp->writeText(QString("<html><body><h1>Hello World!</h1></body></html>"));
    		    resp->end();
    		})
    		.build();
        
    // let's go!
    server->listen(QHostAddress::LocalHost, 8080);

In the handler, you are expected to respond to messages or add custom routing. 
To bind to single HTTP request type, `QHttpServer::Config::get()` and similar
are supported. Otherwise, `QHttpServer::Config::route()` will allow one to 
handle all HTTP request types. 

The server and request/response objects emit various signals
and have guarantees about memory management. See the API documentation for
these.

Contribute
----------

I welcome contributions. Bug fixes and pull requests are ideal.

If you're going to implement a large feature, please open an issue with the details before spending time. 

Cheers,

Kevin

Everybody who has ever contributed shows up in [Contributors](https://github.com/Nava2/QtWebService/graphs/contributors).


