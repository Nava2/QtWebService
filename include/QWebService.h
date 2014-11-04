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
#ifndef QWEBSERVICE_H
#define QWEBSERVICE_H

#include <QObject>
#include <QSharedPointer>
#include <QHttpServer/qhttpserver.h>
#include <QHttpServer/qhttpresponse.h>
#include <QHttpServer/qhttprequest.h>

#include "private/qtwebserviceapi.h"
#include "private/qtwebservicefwd.h"

class QTWEBSERVICE_API QWebService : public QObject {
    Q_OBJECT

    /// @cond nodoc
    friend class QWebServiceConfig;
    /// @endcond

public:
    /// Defines the signature for a routing function
    typedef std::function<void(const QSharedPointer<QWebRoutedRequest>, const QSharedPointer<QHttpResponse>)> RouteFunction;

    typedef QHttpRequest::HttpMethod HttpMethod;

    virtual
    ~QWebService();

private:
    QWebService(QHttpServer *server, QWebRouter *router, QObject *parent);

    QHttpServer * const m_server;
    QWebRouter * const m_router;

};

#endif // QWEBSERVICE_H
