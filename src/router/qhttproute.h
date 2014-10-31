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
#ifndef QHTTPROUTE_H
#define QHTTPROUTE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QSharedPointer>
#include <QList>
#include <QHash>
#include <QMutex>

#include "../qhttpserverapi.h"

class QHTTPSERVER_API QHttpRoute {

public:

    //!< typedef for Shared Pointer
    typedef QSharedPointer<QHttpRoute> Ptr;

    /**
     * @brief The ParsedRoute class is a resultant type from
     * %QHttpRoute::checkPath
     */
    class ParsedRoute {
    public:

        //!< Typedef for convenience
        typedef QSharedPointer<ParsedRoute> Ptr;

        /**
         * @brief Return a %QHash of variables and values that are pulled from
         * a route after parsing.
         *
         * Returns a hash of all **named** variables in a route.
         *
         * @return const reference to the variables
         */
        inline
        const QHash<QString, QString> &urlParams() {
            return m_urlParams;
        }

        /**
         * @brief splat Any unnamed value, or `*` present
         * @return Non-empty list of parameters, all values are kept in order
         *      of occurance then declaration
         */
        inline
        const QStringList &splat() {
            return m_splat;
        }

        /**
         * @brief groups All captures in a route.
         * @return
         */
        inline
        const QStringList &groups() {
            return m_groupVals;
        }

    public:

        ParsedRoute(const QHash<QString, QString> &params, const QStringList &splat, const QStringList &groupVals)
            : m_urlParams(params), m_splat(splat), m_groupVals(groupVals) {

        }

        ParsedRoute(const ParsedRoute &other)
            : m_urlParams(other.m_urlParams), m_splat(other.m_splat),
              m_groupVals(other.m_groupVals) {

        }

        ~ParsedRoute() { /* no op */ }

    private:

        const QHash<QString, QString> m_urlParams;
        const QStringList m_splat;
        const QStringList m_groupVals;
    };

    /// No-op
    virtual
    ~QHttpRoute() { }

    /*!
     * Full route expression, i.e. `"/profile/:userId/:type/"`
     */
    inline
    const QString route() const {
        return _route;
    }

    /*!
     * Return a \ref QList of variables that are pulled from a route after 
     * parsing. The order is **not** guarenteed.
     * \return const reference to the variables
     */
    virtual
    const QStringList variables() const = 0;
    
    /*!
     * \brief checkPath Check that path values (not including root) for 
     * whether the path matches all underlying \ref QHttpRoutePath instances
     * and if variables are present, load them into the `vars` \ref QHash.
     * \param paths Individual paths to try and match
     * \returns Valid pointer if found, caller must delete pointer
     */
    virtual
    ParsedRoute::Ptr const checkPath(const QString path, const QStringList &sepPath) = 0;
    
protected:

    explicit QHttpRoute(const QString route) :
        _route(route) {
    }

    const QString _route;
    
};

/**
 * @brief The QHttpRouteFactory class creates new instances of QHttpRoute
 */
class QHTTPSERVER_API QHttpRouteFactory {

public:

    enum CreationError {
        NO_ERROR = 0,
        PATH_PART_ERROR,
        ROOT_MISSING,
        INVALID_REGEX_PRODUCED
    };

    QHttpRouteFactory()
        : m_errorMutex(),
          m_lastError(CreationError::NO_ERROR),
          m_lastErrorMsg() {
        // no-op
    }

    ~QHttpRouteFactory() {

    }

    QHttpRoute::Ptr create(const QString &route) const;

    QHttpRoute::Ptr create(const QRegularExpression &regex) const;

    QHttpRoute::Ptr createRegex(const QString &comp) const;

    const CreationError lastError() const{
        return m_lastError;
    }

    const QString lastErrorMessage() const{
        return m_lastErrorMsg;
    }

private:

    inline
    void setError(CreationError err, const QString message) const {
        m_errorMutex.lock();

        m_lastError = err;
        m_lastErrorMsg = message;

        m_errorMutex.unlock();
    }

    inline
    void clearError() const {
        m_errorMutex.lock();

        m_lastError = CreationError::NO_ERROR;
        m_lastErrorMsg.clear();

        m_errorMutex.unlock();
    }

    mutable QMutex m_errorMutex;
    mutable CreationError m_lastError;
    mutable QString m_lastErrorMsg;

};

#endif // QHTTPROUTE_H
