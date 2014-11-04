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

#include "router/QWebRouter.h"

#include "router/QWebRoute.h"
#include "router/QWebRequest.h"
#include "router/QWebResponse.h"

#include <QDebug>
#include <QSharedPointer>

#include <QHttpServer/qhttpresponse.h>
#include <QHttpServer/qhttprequest.h>

const QString QWebRouter::DEFAULT_404_MSG =
    "<html> " 
    "<head>" 
    "   <title>404 Unknown Page</title>" 
    "</head>" 
    "<body>"
    "   <h1>404 Page Not Found</h1>"
    "   <p><b>Requested:</b> ${page}</p>"
    "</body>"
    "</html>\n\n";
    
const QRegExp QWebRouter::DEFAULT_404_PATH_REPL("\\$\\{page\\}");
    
const QWebRouter::RouteFunction QWebRouter::DEFAULT_404 = [](QSharedPointer<QWebRequest> req,
                                                               QSharedPointer<QWebResponse> resp) {
        QString msg = QWebRouter::DEFAULT_404_MSG;
        resp->writeText(msg.replace(QWebRouter::DEFAULT_404_PATH_REPL,
                                            req->path()));
    };

QWebRouter::QWebRouter(const QHash<QWebService::HttpMethod, RoutePairList> routes,
                         const RouteFunction fourohfour,
                         QObject* parent)
    : QObject(parent), 
    m_routes(routes),
    m_404(fourohfour) {
    
}

QWebRouter::~QWebRouter()
{

}

inline
void parsePOSTVars(QString query, QHash<QString, QString> &out) {
    const QStringList pairs = query.split('&', QString::SkipEmptyParts);

    for (QString pair : pairs) {
        const QStringList keyVals = pair.split('=', QString::KeepEmptyParts);

        out[keyVals[0]] = keyVals[1];
    }
}

void QWebRouter::handleRoute(QHttpRequest* request, QHttpResponse* resp)
{
    qDebug() << request->methodString() << ":" << request->url();
   
    const QString route = request->path();
    request->storeBody();
    
    QHash<QString, QString> postParams;
    if (request->url().hasQuery()) {
        parsePOSTVars(request->url().query(), postParams);
    }

    if (request->method() == QWebService::HttpMethod::HTTP_POST) {
        QString bodyVals = QString(request->body());

        parsePOSTVars(bodyVals, postParams);
    }

    // we recieved a request:
    const QStringList sepPath = route.split('/', QString::SkipEmptyParts);

    QSharedPointer<QWebRoute::ParsedRoute> routeResponse;
    QWebRouter::RouteFunction func;
    
    QWebService::HttpMethod method = request->method();
    RoutePairList routes = m_routes[method];
  
    // search for the first matching path
    for (RoutePair pair : routes) {
        QSharedPointer<QWebRoute::ParsedRoute> resp(pair.first->checkPath(route, sepPath));
        if (resp) {
            func = pair.second;
            routeResponse = resp;

            break;
        }
    }

    auto reqPtr = QWebRequest::create(request, postParams,
                                          routeResponse->urlParams(),
                                          routeResponse->splat());
    auto respPtr = QWebResponse::create(resp);

    connect(request, &QHttpRequest::end, [this, func, reqPtr, respPtr]() {
        if (func) {
            // we found a proper route:
            func(reqPtr, respPtr);
        } else {
            // 404:
            m_404(reqPtr, respPtr);
        }
    });
}

