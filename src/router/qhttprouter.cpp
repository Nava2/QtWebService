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

#include "qhttprouter.h"

#include <QDebug>

#include "qhttpresponse.h"
#include "qhttprequest.h"
#include "qhttproute.h"
#include "qhttproutedrequest.h"

const QString QHttpRouter::DEFAULT_404_MSG = 
    "<html> " 
    "<head>" 
    "   <title>404 Unknown Page</title>" 
    "</head>" 
    "<body>"
    "   <h1>404 Page Not Found</h1>"
    "   <p><b>Requested:</b> ${page}</p>"
    "</body>"
    "</html>\n\n";
    
const QRegExp QHttpRouter::DEFAULT_404_PATH_REPL("\\$\\{page\\}");
    
const QHttpRouter::RouteFunction QHttpRouter::DEFAULT_404 = [](QSharedPointer<QHttpRoutedRequest> req,
                                                               QHttpResponse* resp) {
        resp->writeHead(QHttpResponse::STATUS_NOT_FOUND);
        QString msg = QHttpRouter::DEFAULT_404_MSG;
        resp->writeText(msg.replace(QHttpRouter::DEFAULT_404_PATH_REPL,
                                            req->path()));
        
        resp->end();
    };

QHttpRouter::QHttpRouter(const QHash<QHttpServer::HttpMethod, RoutePairList> routes,
                         const RouteFunction fourohfour,
                         QObject* parent)
    : QObject(parent), 
    m_routes(routes),
    m_404(fourohfour) {
    
}

QHttpRouter::~QHttpRouter()
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

void QHttpRouter::handleRoute(QHttpRequest* request, QHttpResponse* resp)
{
    qDebug() << request->methodString() << ":" << request->url();
   
    const QString route = request->path();
    request->storeBody();
    
    QHash<QString, QString> postParams;
    if (request->url().hasQuery()) {
        parsePOSTVars(request->url().query(), postParams);
    }

    if (request->method() == QHttpServer::HTTP_POST) {
        QString bodyVals = QString(request->body());

        parsePOSTVars(bodyVals, postParams);
    }

    // we recieved a request:
    const QStringList sepPath = route.split('/', QString::SkipEmptyParts);

    QSharedPointer<QHttpRoute::ParsedRoute> routeResponse;
    QHttpRouter::RouteFunction func;
    
    QHttpServer::HttpMethod method = request->method();
    RoutePairList routes = m_routes[method];
  
    // search for the first matching path
    for (RoutePair pair : routes) {
        QSharedPointer<QHttpRoute::ParsedRoute> resp(pair.first->checkPath(route, sepPath));
        if (resp) {
            func = pair.second;
            routeResponse = resp;

            break;
        }
    }

    auto reqPtr = QHttpRoutedRequest::create(request, postParams,
                                          routeResponse->urlParams(),
                                          routeResponse->splat(),
                                          this);

    connect(request, &QHttpRequest::end, [this, func, reqPtr, resp]() {
        if (func) {
            // we found a proper route:
            func(reqPtr, resp);
        } else {
            // 404:
            m_404(reqPtr, resp);
        }
    });
}

