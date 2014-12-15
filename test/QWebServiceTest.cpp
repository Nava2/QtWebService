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

#include "test/TestUtils.h"

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

            bool timeout = testUtils::spinUntil(&manager, &QNetworkAccessManager::finished, 400);

            REQUIRE(!timeout);
            REQUIRE(reply);
        }
    }
}
