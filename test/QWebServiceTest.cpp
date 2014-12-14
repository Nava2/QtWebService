#include "catch/catch.hpp"

#include "QWebService.h"
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonObject>
#include <QUrl>
#include <string>
#include <QWebServiceConfig.h>
#include <QTimer>
#include "router/QWebRequest.h"
#include "router/QWebResponse.h"

template <class T, typename F>
void spin(T * obj, F callback, const int msec = 400)
{
    //Spin until we get the reply or timeout
    QEventLoop eLoop;

    QTimer timer;
    bool timeOut = false;
    QObject::connect(&timer, &QTimer::timeout, [&](){ timeOut = true; eLoop.quit(); } );

    timer.start(msec);
    QObject::connect(obj, callback, &eLoop, &QEventLoop::quit);
    eLoop.exec();

    REQUIRE(!timeOut);
}

SCENARIO( "A simple service is configured and used", "[QWebService]" ) {

    GIVEN( "A network manager" )
    {
        QNetworkAccessManager manager;

        auto response = [](QSharedPointer<QWebRequest> req, QSharedPointer<QWebResponse> resp)
        {
            REQUIRE(req);
            REQUIRE(resp);

            QJsonObject result;
            result.insert(QString("hello"),QString("world"));
            resp->setStatusCode(QWebResponse::StatusCode::STATUS_OK);
            resp->writeJson(result);
        };

        QSharedPointer<QWebService> service = QSharedPointer<QWebService> (QWebServiceConfig()
                .get("/hello", response)
                .build());

        REQUIRE(service);

        service->startService(QHostAddress::LocalHost, 8080);

        WHEN( "We try to call the path hello" )
        {
            //Set up the networking request
            QNetworkRequest request(QUrl("http://localhost:8080/hello"));

            //Post the request
            QNetworkReply* reply = manager.get(request);

            spin(&manager, &QNetworkAccessManager::finished, 400);

            REQUIRE(reply);
        }
    }
}
