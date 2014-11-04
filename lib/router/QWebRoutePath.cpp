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

#include "qhttproutepath.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDebug>

// js reg: ^(?::((?:[\w\d-]|%\d{2})+)\$?)?(?:(?:((?:[\w\d-]|%\d{2})+)\|?)*|\*)$
const QRegExp QHttpRoutePath::VALID_EXP("(?::((?:[\\w\\d-]|\%\\d{2})+)\\$?)?(?:(?:((?:[\\w\\d-]|\%\\d{2})+)\\|?)*|\\*)",
    Qt::CaseInsensitive);

// static factory
QHttpRoutePath *QHttpRoutePath::create(const QString comp, QObject *parent) {
    if (comp.isEmpty()) {
        return nullptr;
    }

    if (comp.length() == 1) {
        if (comp[0] == '/') {
            // special case:

            return new QHttpRoutePath(comp, "", QList<QString>(),
                                      QHttpRoutePath::SPEC_EXACT, parent);
        }
        if (comp[0] == '*') {
            // it's a single wildcard, return quickly:

            return new QHttpRoutePath(comp, "", QList<QString>(),
                                      QHttpRoutePath::SPEC_WILDCARD, parent);
        }

        // It is not a special case
    }
    
    if (!VALID_EXP.exactMatch(comp)) {
        qDebug() << "Bad regex match for:" << comp;
        return nullptr;
    }

    QString varName;
    QStringList varVals;

    int posSpecTok = comp.indexOf('$');

    if (comp[0] == ':') {
        // have a variable
        if (posSpecTok != -1) {
            varName = comp.mid(1, posSpecTok - 1);
        } else {
            varName = comp.mid(1, comp.length() - 1);

            return new QHttpRoutePath(comp, varName, QList<QString>(),
                                      QHttpRoutePath::SPEC_WILDCARD, parent);
        }
    }

    //if (posSpecTok != -1) { // always true
    // we have some form of designation:
    varVals = comp.mid(posSpecTok+1, comp.length() - posSpecTok - 1)
            .split('|', QString::SkipEmptyParts);
    varVals.sort();
    //}

    return new QHttpRoutePath(comp, varName, varVals,
                              varVals.size() > 1 ? QHttpRoutePath::SPEC_EXACT_OR : QHttpRoutePath::SPEC_EXACT,
                              parent);
}

QHttpRoutePath::QHttpRoutePath(const QString comp,
                               const QString variableName,
                               const QList<QString> validValues,
                               const QHttpRoutePath::PathSpecificity spec,
                               QObject *parent)
    : QObject(parent),
      _expr(comp),
      _variableName(variableName),
      _isVariable(!variableName.isEmpty()),
      _validValues(validValues),
      _specificity(spec){

}

QHttpRoutePath::~QHttpRoutePath()
{

}

bool QHttpRoutePath::operator==(const QHttpRoutePath& other) const
{
    return (this == &other) ||
        (this->_isVariable == other._isVariable && 
         this->_specificity == other._specificity && 
         this->_expr == other._expr &&
         this->_variableName == other._variableName && 
         this->_validValues == other._validValues);  
}


bool QHttpRoutePath::checkPath(const QString path) const
{
    if (path.isEmpty()) {
        return false;
    }

    // based on the specificity, change how we handle it
    // this may expand as smarter/more intensive features happen
    bool valid = false;
    switch (_specificity) {
    case SPEC_EXACT: {
        valid = path == _validValues[0];
    } break;
    case SPEC_EXACT_OR: {
        valid = _validValues.contains(path);
    } break;
    case SPEC_WILDCARD: {
        valid = true;
    } break;

    }

    return valid;
}
