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

#include <assert.h>

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
    "   <p><b>Requested:</b> <pre>${page}</pre></p>"
    "</body>"
    "</html>\n\n";
    
const QRegExp QWebRouter::DEFAULT_404_PATH_REPL("\\$\\{page\\}");
    
const QWebRouter::RouteFunction QWebRouter::DEFAULT_404 = [](QSharedPointer<QWebRequest> req,
                                                             QSharedPointer<QWebResponse> resp) {
        QString msg = QWebRouter::DEFAULT_404_MSG;
        resp->setStatusCode(QWebResponse::StatusCode::STATUS_NOT_FOUND);
        resp->writeText(msg.replace(QWebRouter::DEFAULT_404_PATH_REPL,
                                            req->path()), "text/html");
    };
    
const QWebRouter::ApplyPredicate QWebRouter::ALWAYS = [](QSharedPointer<QWebRequest> re, QSharedPointer<QWebResponse> rs) -> bool { return true; };
const QWebRouter::ApplyPredicate QWebRouter::NEVER = [](QSharedPointer<QWebRequest> re, QSharedPointer<QWebResponse> rs) -> bool { return false; };

QWebRouter::QWebRouter(const QHash< QWebService::HttpMethod, QWebRouter::RoutePairList >& routes, 
                       const QHash< QWebRouter::RouteStage, QVector< QPair< QWebRouter::ApplyPredicate, QWebRouter::RouteFunction > > >& middleware, 
                       const QWebRouter::RouteFunction fourohfour,
                       QObject* parent)
    : QObject(parent),
      m_routes(routes),
      m_middleware(middleware),
      m_404(fourohfour),
      m_service(nullptr) {
    
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

const QVector< QWebRouter::RouteFunction > QWebRouter::getMiddleware(const QWebRouter::RouteStage stage, const QWebRequest::Ptr req, const QWebResponse::Ptr resp)
{
    assert(m_middleware.contains(stage));
    
    const QVector<QPair<ApplyPredicate, RouteFunction> > pairs = m_middleware[stage];
    
    QVector<RouteFunction > out;
    out.reserve(pairs.size());
    
    for (QPair<ApplyPredicate, RouteFunction> pair : pairs) {
        if (pair.first(req, resp)) {
            // predicate was true:
            out += pair.second;
        }
    }
    
    // we could squeeze the memory, but it's not needed and the over head is probably more than the overhead saved. 
    // - Kevin
    // out.squeeze();
    
    return out;
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
    QWebRoute::ParsedRoute::Ptr routeResponse;
    QWebRouter::RouteFunction func;
    
    QWebService::HttpMethod method = request->method();
    RoutePairList routes = m_routes[method];
  
    // search for the first matching path
    for (RoutePair pair : routes) {
        QSharedPointer<QWebRoute::ParsedRoute> resp = pair.first->checkPath(route);
        if (resp) {
            func = pair.second;
            routeResponse = resp;

            break;
        }
    }

    QSharedPointer<QWebRequest> reqPtr;

    if (routeResponse) {
        reqPtr = QWebRequest::create(request, postParams,
                                     routeResponse->urlParams(),
                                     routeResponse->splat());
    } else {
        reqPtr = QWebRequest::create(request, postParams, QHash<QString, QString>(), QStringList());
    }

    QSharedPointer<QWebResponse> webRespPtr = QWebResponse::create();
    
    auto preMW = getMiddleware(RouteStage::PRE_HANDLER, reqPtr, webRespPtr);
    
    connect(request, &QHttpRequest::end, [this, func, reqPtr, resp, webRespPtr, preMW]() {
        // apply the pre-emptive middle ware
        for (auto mw : preMW) {
            mw(reqPtr, webRespPtr);
        }
        
        if (func) {
            // we found a proper route:
            func(reqPtr, webRespPtr);
        } else {
            // 404:
            m_404(reqPtr, webRespPtr);
        }

        auto postMW = this->getMiddleware(RouteStage::POST_HANDLER, reqPtr, webRespPtr);
        for (auto mw : postMW) {
            mw(reqPtr, webRespPtr);
        }
        
        webRespPtr->writeToResponse(reqPtr, resp);
    });
}

