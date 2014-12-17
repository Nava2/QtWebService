/*
 * Copyright 2014 Kevin Brightwell <kevin.brightwell2@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

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

#include "test/TestUtils.h"

#include <catch/catch.hpp>

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

            bool noTimeout = testUtils::spinUntil(&manager, &QNetworkAccessManager::finished, 400);

            REQUIRE(noTimeout);
            REQUIRE(reply);
        }
    }
}
