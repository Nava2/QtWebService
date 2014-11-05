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

#ifndef QMIDDLEWAREREGISTRAR_H
#define QMIDDLEWAREREGISTRAR_H


#include "private/qtwebservicefwd.h"

#include "QWebMiddleWare.h"

#include <QObject>
#include <QSharedPointer>
#include <QByteArray>
#include <QList>

// forward

class QWebMiddleWareRegistrarEntry : public QObject {

    Q_OBJECT

public:

    ~QWebMiddleWareRegistrarEntry() { /* no-op */ }

signals:

    /**
     * @brief webResponseDataPrepared
     * @param req
     * @param in
     * @param out
     * @return true if event handled.
     */
    bool webResponseDataPrepared(QSharedPointer<QWebRequest> req, QByteArray in, QSharedPointer<QByteArray> &out);

private:

    /// @cond nodoc
    friend class QWebMiddleWareRegistrar;
    /// @endcond

    QWebMiddleWareRegistrarEntry(QObject *parent) : QObject(parent) { }
};


/**
 * @brief The QWebMiddleWareRegistrar class is a collection of MiddleWares that are kept and iterated over when attempting
 * to use a signal.
 */
class QWebMiddleWareRegistrar : public QObject {
    Q_OBJECT

public:
    typedef QWebMiddleWareRegistrarEntry Entry;

    explicit QWebMiddleWareRegistrar(QObject *parent = 0);

    /**
     * @brief Destructor
     */
    ~QWebMiddleWareRegistrar();

    QWebMiddleWareRegistrar &registerMiddleWare(QWebMiddleWare * const ware);


public slots:

    void webResponseDataPrepared(QSharedPointer<QWebRequest> req, QByteArray in, QSharedPointer<QByteArray> &out);

private:
    QList<Entry *> m_entries;
};

#endif // QMIDDLEWAREREGISTRAR_H
