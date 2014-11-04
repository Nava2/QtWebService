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

#pragma once
#ifndef QHTTPROUTER_H
#define QHTTPROUTER_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QPair>
#include <QHttpServer/qhttpserver.h>

#include <qwebserviceconfig.h>

#include "private/qtwebserviceapi.h"
#include "private/qtwebservicefwd.h"

class QTWEBSERVICE_API QWebRouter : public QObject
{
    Q_OBJECT
    
    /// @cond nodoc
    friend class QWebService;
    /// @endcond
    
public:
    
    //!< Re-typedef %QHttpServer::RouteFunction
    typedef QWebService::RouteFunction RouteFunction;
    
    typedef QPair<QSharedPointer<QWebRoute>, RouteFunction> RoutePair;
    typedef QList<RoutePair> RoutePairList;
    
    //!< Immediately returns HTTP status code 404
    static const RouteFunction DEFAULT_404; 

    static const QString DEFAULT_404_MSG;
    static const QRegExp DEFAULT_404_PATH_REPL;
       
    ~QWebRouter();
    
private slots:
    
    /*!
     * Figures out the proper RouteHandler if installed, otherwise it will 
     * trigger a 404 for the `resp` instance.
     * 
     * \param request The HTTP request including all HTTP info
     * \param resp Resultant HTTP QHttpResponse
     */
    void handleRoute(QHttpRequest *request, QHttpResponse *resp);
    

private:
    
    explicit QWebRouter(const QHash<QWebService::HttpMethod, RoutePairList> routes,
                         const RouteFunction fourohfour,
                         QObject* parent = nullptr);

    const QHash<QWebService::HttpMethod, RoutePairList> m_routes;

    const RouteFunction m_404;
    
};

#undef HTTP_METHOD

#endif // ROUTER_H
