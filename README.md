QHttpServer
===========

A Qt HTTP Server - because hard-core programmers write web-apps in C++ :)

[![Build Status](https://travis-ci.org/Nava2/qhttpserver.svg)](https://travis-ci.org/Nava2/qhttpserver)

It uses Joyent's [HTTP Parser](http://github.com/joyent/http-parser) and is asynchronous and does not require any inheritance.

QHttpServer is available under the MIT License.

**NOTE: QHttpServer is NOT fully HTTP compliant right now! DO NOT use it for
anything complex**

Installation
------------

Requires Qt 4 or Qt 5. (Untested on Qt 4)

    mkdir build/ && cd build/ && cmake .. && make -j2 && su -c "make install"

To link to your projects put this in your project's qmake project file

    LIBS += -lqhttpserver

By default, the installation prefix is /usr/local. To change that to /usr,
for example, run:

    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr

Usage
-----

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

This project is a fork of the original, by Nikhil Marathe 
(https://github.com/nikhilm/qhttpserver) and all of his original copyright 
notices are maintained. This project aims to add features on top of the 
original reference implementation to allow for easier application development. 

As with Nikhil, I welcome contributions. Bug fixes and pull requests are ideal.

If you're going to implement a large feature, please open an issue with the details before spending time. 

Cheers,

Kevin

Everybody who has ever contributed shows up in [Contributors](https://github.com/Nava2/qhttpserver/graphs/contributors).


