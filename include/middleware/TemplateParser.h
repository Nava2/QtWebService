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

#ifndef TEMPLATEPARSER_H
#define TEMPLATEPARSER_H

#include "private/qtwebserviceapi.h"
#include "private/qtwebservicefwd.h"

#include "QWebMiddleWare.h"
#include "QWebService.h"
#include "router/QWebRequest.h"

class TemplateParser : public QWebMiddleWare
{
public:
    TemplateParser(QObject *parent = nullptr);

    /**
     * @brief ~QWebMiddleWare No-operation destructor specified for inheritance concerns.
     */
    virtual
    ~TemplateParser() { }

    virtual
    void configure(RegistrarEntry * const entry) {
        connect(entry, &QWebMiddleWareRegistrar::Entry::webResponseDataPrepared,
                this, &TemplateParser::myFunc);
    }

    bool myFunc(QSharedPointer<QWebRequest> req, QByteArray in, QSharedPointer<QByteArray> &out) {
        return false;
    }
};

#endif // TEMPLATEPARSER_H
