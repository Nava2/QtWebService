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
#ifndef Q_WEB_MIDDLE_WARE_H
#define Q_WEB_MIDDLE_WARE_H


#include "private/qtwebservicefwd.h"

#include "QWebMiddleWareRegistrar.h"

#include <QObject>

/**
 * @brief The MiddleWare class is a purely virtual class which is used as a base for installing MiddleWare into a
 * QtWebService.
 *
 * MiddleWare software is anything which will operate on a %QWebRequest before it reaches a routing function, or on a
 * %QWebResponse after a routing function completes. There are a series of signals emitted by the QtWebService
 * framework that can be connected when %MiddleWare::configure is called.
 */
class QWebMiddleWare : public QObject {

    Q_OBJECT

public:

    //!< Convenience typedef
    typedef QWebMiddleWareRegistrar::Entry RegistrarEntry;

    /**
     * @brief ~QWebMiddleWare No-operation destructor specified for inheritance concerns.
     */
    virtual
    ~QWebMiddleWare() { }

    virtual
    void configure(RegistrarEntry * const entry) = 0;

protected:
    /**
     * @brief QWebMiddleWare This ctor is only used by subclasses
     * @param parent
     */
    QWebMiddleWare(QObject *parent) : QObject(parent) { }
};


#endif // Q_WEB_MIDDLE_WARE_H
