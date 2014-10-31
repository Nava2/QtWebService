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
#ifndef QHTTPROUTEDREQUEST_H
#define QHTTPROUTEDREQUEST_H

#include <QObject>
#include <QHash>
#include <QList>
#include <QString>
#include <QSharedPointer>

#include "../qhttprequest.h"

class QHttpRoutedRequest : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief create Create a new instance of %QHttpRoutedRequest
     * @param httpReq %QHttpRequest as the base
     * @param urlParams Any parameters that were stored in the url
     * @param splat If a wildcard was used in the URL, this will be matches
     *      from the wildcard.
     * @param parent QObject parent
     * @return Shared pointer
     */
    static QSharedPointer<QHttpRoutedRequest> create(QHttpRequest *httpReq,
                                      const QHash<QString, QString> &postParams,
                                      const QHash<QString, QString> &urlParams,
                                      const QStringList &splat,
                                      QObject *parent = 0);

    /**
     * @brief urlParams All parameters passed in as "router" variables
     * @return %QHash of all variables `<key, value>`
     */
    inline
    const QHash<QString, QString> &urlParams() {
        return m_urlParams;
    }

    /**
     * @brief urlSplat Values, if any, that were captured by a wildcard
     * @return %QStringList, which may be empty if no wildcard values are
     *      present
     */
    inline
    const QStringList &urlSplat() {
        return m_splat;
    }

    /**
     * @brief queryParams Parameters found in the query string of a URL and
     * the body of a POST/PUT request
     * @return %QHash, possibly empty, with key-value pairs.
     */
    inline
    const QHash<QString, QString> &queryParams() {
        return m_urlParams;
    }

    /**
     * @brief path Calls %QHttpRequest::path directly.
     * @return
     */
    inline
    const QString path() {
        return m_req->path();
    }

    /**
     * @brief path Calls %QHttpRequest::path directly.
     * @return
     */
    inline
    const QUrl &url() {
        return m_req->url();
    }

    inline
    QHttpRequest * const httpRequest() {
        return m_req;
    }

signals:

public slots:

private:
    explicit QHttpRoutedRequest(QHttpRequest *httpReq,
                                const QHash<QString, QString> &postParams,
                                const QHash<QString, QString> &urlParams,
                                const QStringList &splat,
                                QObject *parent = 0);

    QHttpRequest * const m_req;
    const QHash<QString, QString> m_urlParams;
    const QHash<QString, QString> m_postParams;
    const QStringList m_splat;

};

#endif // QHTTPROUTEDREQUEST_H
