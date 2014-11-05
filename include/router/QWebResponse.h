/*
 * Copyright 2014 kevin
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

#ifndef QWEBRESPONSE_H
#define QWEBRESPONSE_H


#include "../private/qtwebservicefwd.h"

#include <functional>

#include <QObject>
#include <QSharedPointer>
#include <QHash>
#include <QStringList>
#include <QFile>
#include <QJsonDocument>
#include <QDomDocument>

#include <QHttpServer/qhttpresponse.h>

class QTWEBSERVICE_API QWebResponse : public QObject
{
    Q_OBJECT

public:

    enum ResponseError {
        SUCCESS = 0,

        //!< Tried to write response, no data was set
        NO_DATA_SET,

        //!< QFile was attempted to be read, and was non-existant
        FILE_DOES_NOT_EXIST = 10,
        FILE_COULD_NOT_OPEN,

        NULLPTR = 100
    };

    typedef QHttpResponse::StatusCode StatusCode;

    /**
     * @brief create Create a new instance of %QWebRequest
     * @return Shared pointer
     */
    static QSharedPointer<QWebResponse> create();

    /**
     * @brief setHeader Sets a header parameter based on the type passed
     * @param key
     * @param value
     */
    void setHeader(const QString key, const QString value);

    /**
     * @brief setStatusCode Set the response to use the status code.
     * @param code
     */
    void setStatusCode(StatusCode code);

    /**
     * @brief writeFile Enqueues the %QFile to be written to the output stream as a byte array. This is done by reading
     * the file to a bytestream, and writing it after.
     * @param file File that will be read, this will emit the signal to middleware hooks
     * @return
     */
    bool writeFile( QFile file);

    bool writeText(const QString text, const QString contentType = "text/plain");

//    bool writeText(const QByteArray &text);

//    bool writeJSON(const QJsonDocument &doc);

//    bool writeXML(const QDomDocument &doc);

    /**
     * @brief isValidResponse returns true if a response was queued to be written
     * @return
     */
    bool isValidResponse();

    /**
     * @brief writeToResponse writes the stored data to the QHttpResponse instance
     * @param httpResponse Response to write data to
     * @return
     */
    ResponseError writeToResponse(QSharedPointer<QWebRequest> req, QHttpResponse *httpResponse);


    virtual
    ~QWebResponse();

signals:

    /**
     * @brief modifyOutputData emitted when data is about to be written to a QHttpResponse
     * @param in
     * @param out use QSharedPointer::reset to set this shared pointer to a new value for use
     */
    void responseDataPrepared(QSharedPointer<QWebRequest> req, QByteArray in, QSharedPointer<QByteArray> &out);

private:
    QWebResponse();

    std::function<QByteArray(ResponseError *)> m_outFunc;
    QHash<QString, QString> m_headers;
    StatusCode m_status;
};

#endif // QWEBRESPONSE_H
