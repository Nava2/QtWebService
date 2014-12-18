/*
 * Copyright 2014 Kevin Brightwell <kevin.brightwell2@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "catch/catch.hpp"

#include <router/QWebRequest.h>
#include <router/QWebResponse.h>
#include <router/QWebRouter.h>
#include <QWebService.h>
#include <QWebServiceConfig.h>
#include <test/TestUtils.h>

#include "CatchUtils.h"

#include <sstream>
#include <string>

QWebRouter::RouteFunction CATCH_queryParams(const QHash<QString, QString> &expected) {
    return [&expected](QWebRequest::Ptr req, QWebResponse::Ptr resp) {
            REQUIRE(expected == req->urlParams()); 
        };
}

QWebRouter::RouteFunction CATCH_urlSplat(const QStringList &expected) {
    return [&expected](QWebRequest::Ptr req, QWebResponse::Ptr resp) {
            REQUIRE(expected == req->urlSplat()); 
        };
}

QWebRouter::RouteFunction CATCH_urlParams(const QHash<QString, QString> &expected) {
    return [&expected](QWebRequest::Ptr req, QWebResponse::Ptr resp) {
            REQUIRE(expected == req->urlParams()); 
        };
}

QWebRouter::RouteFunction CATCH_path(const QString &expected) {
    return [&expected](QWebRequest::Ptr req, QWebResponse::Ptr resp) {
            REQUIRE(expected == req->path()); 
        };
}

QWebRouter::RouteFunction CATCH_url(const QUrl &expected) {
    return [&expected](QWebRequest::Ptr req, QWebResponse::Ptr resp) {
            REQUIRE(expected == req->url()); 
        };
}

/**
 * @struct PRE_POST_Check Used as Middleware to confirm triggering.
 */
struct PRE_POST_Check {
    
    PRE_POST_Check() : pretrig(0), posttrig(0) { }

    void PRE(QWebRequest::Ptr req, QWebResponse::Ptr resp) {
        pretrig++;
    }
    
    void POST(QWebRequest::Ptr req, QWebResponse::Ptr resp) {
        posttrig++;
    }
    
    int pretrig, posttrig;
};

SCENARIO( "Install PRE and POST Middleware, verify execution" ) {
    
    PRE_POST_Check checker;
    
    using namespace std::placeholders;
    
    QSharedPointer<QWebService> service = QSharedPointer<QWebService>(QWebServiceConfig()
        .installMiddleware(QWebRouter::PRE_HANDLER, QWebRouter::ALWAYS, 
                           std::bind(&PRE_POST_Check::PRE, checker, _1, _2))
        .installMiddleware(QWebRouter::PRE_HANDLER, QWebRouter::ALWAYS, 
                           std::bind(&PRE_POST_Check::POST, checker, _1, _2))
        .get("/echo", &QtWS::test::ECHO_REPLY)
        .build());
        
    REQUIRE(service);
    
    QtWS::test::NetworkFactory netfactory(QUrl("http://localhost:8080"), [](QNetworkReply *reply, QNetworkReply::NetworkError error) {
            std::stringstream ss;
            ss << "Reply failed: [" << error << "] " << reply->errorString().toStdString();
            INFO(ss.str());
            
            REQUIRE(false);
        });
    
    service->startService(QHostAddress::LocalHost, 8080);
    
    WHEN( "Requesting a single URL" ) {
        QNetworkReply* reply = netfactory.get("/echo");
        
        REQUIRE(QtWS::test::spinUntil(reply, &QNetworkReply::finished, 400));
        
        REQUIRE(checker.pretrig == checker.posttrig);
        REQUIRE(checker.pretrig == 1);
    }
    
}


SCENARIO( "Query paths and verify parameters are parsed", "[QWebRoute]" ) {
    
    
        
    WHEN( "POST string params are used" ) {
        
    }

}
