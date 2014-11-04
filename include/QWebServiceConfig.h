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
#ifndef QWEBSERVICECONFIG_H
#define QWEBSERVICECONFIG_H

#include <QRegularExpression>
#include <QString>
#include <QSharedPointer>
#include <QHttpServer/qhttpserver.h>

#include <functional>

#include "private/qtwebserviceapi.h"
#include "private/qtwebservicefwd.h"

#include "QWebService.h"

/// Used to specify a QWebService's configuration
class QTWEBSERVICE_API QWebServiceConfig {

public:

    /// Redefines typedef
    typedef QWebService::RouteFunction RouteFunction;

    /// Key used for storing routing information while configuring, this should
    /// not be used outside of %QWebServiceConfig
    class Key {
    public:
        //!< Simple typedef to ease code
        typedef QSharedPointer<Key> Ptr;

        //!< String based path, may have variables
        const QString path;

        //!< Regex based path
        const QRegularExpression reg;

        //!< String representation (same as path or reg.pattern).
        const QString strRep;

        //!< If true, there is a path stored, if false, RegExp
        const bool isPath;

        //!< Routing fuction pointer
        const RouteFunction func;

    public:
        /**
         * @brief create New shared pointer (saves lifetime concerns)
         * @param reg Regex
         * @param func Routing function
         * @return Shared Pointer to new %Key instance
         */
        static Ptr create(const QRegularExpression &reg, QWebService::RouteFunction func) {
            return Ptr(new Key(reg, func));
        }

        /**
         * @brief create New shared pointer (saves lifetime concerns)
         * @param path QString based path
         * @param func Routing function
         * @return Shared Pointer to new %Key instance
         */
        static Ptr create(const QString &path, QWebService::RouteFunction func) {
            return Ptr(new Key(path, func));
        }

    private:
        explicit
        Key(const QString &path, QWebService::RouteFunction func)
            : path(path), reg(), strRep(path),
              isPath(false), func(func) {

        }

        explicit
        Key(const QRegularExpression &reg, QWebService::RouteFunction func)
            : path(), reg(reg), strRep(reg.pattern()),
              isPath(true), func(func) {
        }
    };

    /**
     * Default ctor, intialize buffers.
     */
    QWebServiceConfig();

    /**
     * Clean-up resources
     */
    ~QWebServiceConfig();

    /**
     * @brief validHttpMethods return all implemented %QHttpServer::HttpMethod
     *      values
     * @return %QSet of %QHttpServer::HttpMethod
     */
    static
    const QSet<QWebService::HttpMethod> validHttpMethods() {
        return { QWebService::HttpMethod::HTTP_GET, QWebService::HttpMethod::HTTP_DELETE,
                    QWebService::HttpMethod::HTTP_PUT, QWebService::HttpMethod::HTTP_POST };
    }

/**
 * \define HTTP_METHOD convienience C Macro for defining the different HTTP
 * methods accepted.
 */
#define HTTP_METHOD( name ) \
    template <typename T> \
    inline \
    QWebServiceConfig & name (const QString path, T * handler) { \
        return QWebServiceConfig:: name (Key::create(path, bindHandler(handler))); \
    } \
    \
    template <typename T> \
    inline \
    QWebServiceConfig & name (const QRegularExpression &regRoute, T * handler) { \
        return QWebServiceConfig:: name (Key::create(regRoute, bindHandler(handler))); \
    } \
    \
    QWebServiceConfig & name (const QString path, QWebService::RouteFunction func) { \
        return QWebServiceConfig:: name (Key::create(path, func)); \
    } \
    \
    QWebServiceConfig & name (const QRegularExpression &regRoute, QWebService::RouteFunction func) { \
        return QWebServiceConfig:: name (Key::create(regRoute, func)); \
    } \
    \
    QWebServiceConfig & name ( const Key::Ptr key )

    /*!
     * Installs a new route for all methods in %Config::validHttpMethods().
     *
     * \param route The http route to read, this will eventually be smart,
     *      currently only uses static string matching
     * \param handler A \ref QHttpRouteHandler instance which will be bound via
     *      \ref QObject::setParent to `this` instance.
     * \return reference to `*this`.
     */
    HTTP_METHOD( route ) {
        for (QWebService::HttpMethod method : validHttpMethods()) {
            addHandler(method, key);
        }

        return *this;
    }

        /*!
        * Installs a new route with a handler for GET requests.
        *
        * \param route The http route to read, this will eventually be smart,
        *      currently only uses static string matching
        * \param handler A \ref QHttpRouteHandler instance which will be bound via
        *      \ref QObject::setParent to `this` instance.
        * \return reference to `*this`.
        */
    HTTP_METHOD( get ) {
        addHandler(QWebService::HttpMethod::HTTP_GET, key);

        return *this;
    }

    /*!
        * Installs a new route with a handler for POST requests.
        *
        * \param route The http route to read, this will eventually be smart,
        *      currently only uses static string matching
        * \param handler A \ref QHttpRouteHandler instance which will be bound via
        *      \ref QObject::setParent to `this` instance.
        * \return reference to `*this`.
        */
    HTTP_METHOD( post ) {
        addHandler(QWebService::HttpMethod::HTTP_POST, key);

        return *this;
    }

    /*!
        * Installs a new route with a handler for PUT requests.
        *
        * \param route The http route to read, this will eventually be smart,
        *      currently only uses static string matching
        * \param handler A \ref QHttpRouteHandler instance which will be bound via
        *      \ref QObject::setParent to `this` instance.
        * \return reference to `*this`.
        */
    HTTP_METHOD( put ) {
        addHandler(QWebService::HttpMethod::HTTP_PUT, key);

        return *this;
    }

    /*!
        * Installs a new route with a handler for DELETE requests.
        *
        * \param route The http route to read, this will eventually be smart,
        *      currently only uses static string matching
        * \param handler A \ref QHttpRouteHandler instance which will be bound via
        *      \ref QObject::setParent to `this` instance.
        * \return reference to `*this`.
        */
    HTTP_METHOD( del ) {
        addHandler(QWebService::HttpMethod::HTTP_DELETE, key);

        return *this;
    }

#undef HTTP_METHOD

    /*!
        * Installs a 404 handler, the default behaviour is to redirect the
        * user to root.
        * \param func a function that matches \ref RouteFunction specification.
        * \return reference to `*this`.
        */
    QWebServiceConfig &fourohfour(RouteFunction func);

    /*!
        * Installs a 404 handler, the default behaviour is to redirect the
        * user to root.
        * \param handler A \ref QHttpRouteHandler instance which will be bound via
        *      \ref QObject::setParent to `this` instance.
        * \return reference to `*this`.
        */
    template <class T>
    inline
    QWebServiceConfig &fourohfour(T * handler) {
        return QWebServiceConfig::fourohfour(bindHandler(handler));
    }


    /**
     * Create a new instance of %QHttpServer, configuring it.
     * @param parent Parent of new Builder
     * @return new %QHttpServer with parent `parent`.
     */
    QWebService *build(QObject *parent = nullptr) const;

private:



    /**
     * Binds the handleRoute to a handler which is then passed through std::bind
     * creating a %std::function.
     */
    template <class T> inline
    RouteFunction bindHandler(T *handler) {
        // force it, this should be compiled away anyway in Opt.
        const QObject *tmp = static_cast<const T *>(nullptr);
        Q_UNUSED(tmp);

        using namespace std::placeholders;

        m_specialHandlers.insert(handler);

        return std::bind(&T::operator (), handler, _1, _2);
    }

    /**
     * Adds a handler for @c method.
     * @param method Type of HttpMethod
     * @param route Path for routing
     * @param handler action
     */
    inline
    void addHandler(const QWebService::HttpMethod method, const Key::Ptr key) {
        m_handlers[method] += key;
    }

    QHash<QWebService::HttpMethod, QSet<Key::Ptr> > m_handlers;

    QSet<QObject *> m_specialHandlers;

    QWebService::RouteFunction m_404;

    // needs to be a pointer because of forward declaration
    const QWebRouteFactory * const m_factory;

};

inline
uint qHash(const QWebServiceConfig::Key &key) {
    uint out = 17 * qHash(&key.func);
    out = out * 31 + qHash(key.isPath);
    out = out * 31 + qHash(key.strRep);

    return out;
}

inline
uint qHash(const QWebServiceConfig::Key::Ptr &key) {
    return qHash(*key);
}


#endif // QWEBSERVICECONFIG_H
