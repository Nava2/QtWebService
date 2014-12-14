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
#include "router/QWebRequest.h"
#include "router/QWebResponse.h"

void spin(const QNetworkAccessManager& manager)
{
    //Spin until we get the reply
    QEventLoop eLoop;
    QObject::connect(&manager, SIGNAL( finished( QNetworkReply * ) ), &eLoop, SLOT(quit()));
    eLoop.exec();
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
            resp->writeJson(QJsonDocument(result));
        };

        QSharedPointer<QWebService> service = QSharedPointer<QWebService> (QWebServiceConfig()
                .get ("/hello", response)
                .build());

        service->startService(QHostAddress::LocalHost, 8080);

        REQUIRE(service);

        WHEN( "We try to call the path hello" )
        {
            //Set up the networking request
            QNetworkRequest request(QUrl("http://localhost:8080/hello"));

            //Post the request
            QNetworkReply* reply = manager.get(request);

            spin(manager);

            REQUIRE(reply);
        }
    }
}
