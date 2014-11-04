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

#ifndef QHTTPROUTEPATH_H
#define QHTTPROUTEPATH_H

#include <QObject>
#include <QString>
#include <QStringList>

#include "qhttpserverapi.h"

class QHTTPSERVER_API QHttpRoutePath {
        
public:

    virtual ~QHttpRoutePath();
    
    /*!
     * Return true if has a variable component
     */
    inline
    bool isVariable() const {
        return _isVariable;
    }
    
    /*!
     * Return the name of the variable searched for, or empty string if
     * none are specified.
     * \return Name of variable or empty if not specified.
     */
    inline
    const QString variable() const {
        return _variableName;
    }
    
    /*!
     * Return the valid values associated with a path, if empty, this implies
     * it is has a \ref QHttpRoutePath::specificity of
     * \ref QHttpRoutePath::SPEC_WILDCARD.
     * \return valid values, or empty.
     */
    inline
    const QStringList validValues() const {
        return _validValues;
    }
    
    /*!
     * Get the full expression from the path
     */
    inline
    const QString expr() const {
        return _expr;
    }
    
    /*!
     * Check if a \ref QStringRef matches the Path.
     * \param path Path to parse against
     * \return Shared pointer instance, if no variable was present, the result is null
     */
    bool checkPath(const QString path) const = 0;

    const QStringList captures() const = 0;
    
    bool operator==(const QHttpRoutePath &other) const;

protected:
    QHttpRoutePath(const QString comp,
                            const QString variableName,
                            const QList<QString> validValues,
                            QObject *parent = nullptr);

private:

    const QString _expr;
    const bool _isVariable;
    const QString _variableName;
    const QList<QString> _validValues;
    
    static const QRegExp VALID_EXP;
};



#endif
