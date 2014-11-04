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

#include "router/QWebResponse.h"

#include <QPair>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QDebug>

QWebResponse::QWebResponse()
    : m_outFunc(nullptr), m_status(StatusCode::STATUS_OK)
{

}

QSharedPointer<QWebResponse> QWebResponse::create() {
    return QSharedPointer<QWebResponse>(new QWebResponse);
}

QWebResponse::~QWebResponse() {
    // no-op
}

void QWebResponse::setHeader(const QString key, const QString value) {
    if (!key.isEmpty()) {
        m_headers[key] = value;
    } else {
        qDebug() << "Tried to set empty Key to value:" << value;
    }
}

void QWebResponse::setStatusCode(StatusCode code) {
    m_status = code;
}

bool QWebResponse::isValidResponse() {
    return (bool)m_outFunc;
}

bool QWebResponse::writeFile(QFile file) {
    const QFileInfo info(file);

    m_outFunc = [info](ResponseError *error) -> QByteArray {
        // check if the file exists
        if (!info.exists()) {
            *error = FILE_DOES_NOT_EXIST;
            return QByteArray();
        }

        QFile file(info.path());

        // open it for reading
        if (!file.open(QIODevice::ReadOnly)) {
            *error = FILE_COULD_NOT_OPEN;

            return QByteArray();
        }

        // read the file!
        *error = SUCCESS;
        QByteArray out = file.readAll();
        file.close();

        return out;
    };

    return true;
}

bool QWebResponse::writeText(const QString text, const QString contentType) {

    m_outFunc = [text](ResponseError *error) -> QByteArray {
        QByteArray out;
        out.reserve(text.length());
        out.append(text);

        return out;
    };

    if (!m_headers.contains("Content-Type")) {
         m_headers["Content-Type"] = contentType.isEmpty() ? "text/plain" : contentType;
    }

    return true;
}

QWebResponse::ResponseError QWebResponse::writeToResponse(QSharedPointer<QWebRequest> req,
                                                          QHttpResponse *httpResponse) {
    if (!isValidResponse()) {
        return NO_DATA_SET;
    }

    ResponseError error = SUCCESS;
    QByteArray buff = m_outFunc(&error);

    if (error != SUCCESS) {
        return error;
    }

    QSharedPointer<QByteArray> outPtr;
    emit responseDataPrepared(req, buff, outPtr);

    const QByteArray &out = outPtr ? *outPtr : buff;

    httpResponse->setHeader("Content-Length", QString::number(buff.length()));
    for (QString key : m_headers.keys()) {
        httpResponse->setHeader(key, m_headers[key]);
    }

    httpResponse->writeHead(m_status);
    httpResponse->write(out);
    httpResponse->end();

    return SUCCESS;
}
