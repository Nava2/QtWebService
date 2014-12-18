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

#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <QTimer>
#include <QEventLoop>
#include <QObject>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

#include <QUrlQuery>

#endif

#include "../router/QWebRouter.h"

#include <type_traits>
#include <assert.h>

/**
 * @file Useful test harnesses and utilities, this file should include all others.
 */

namespace QtWS {

namespace test {

/**
 * Spins until either `msec` milliseconds pass or the `callback` signal fires, if it times out, the test will fail.
 * 
 * @param 
 * @return true if signal is triggered, false if there is a timeout fail
 */
template <class T, class F>
bool spinUntil(T * obj, F callback, const int msec = 400)
{
    static_assert(std::is_base_of<QObject, T>::value, "T must derive from QObject");
    static_assert(std::is_function<F>::value || std::is_member_function_pointer<F>::value,
                  "callback must be function or member function of subclass of T");
    
    //Spin until we get the reply or timeout
    QEventLoop eLoop;

    QTimer timer;
    bool timeOutFail = false;
    QObject::connect(&timer, &QTimer::timeout, [&](){ timeOutFail = true; eLoop.quit(); });

    timer.start(msec);
    QObject::connect(obj, callback, &eLoop, &QEventLoop::quit);
    eLoop.exec();

    // Return true if the signal was triggered
    return !timeOutFail;
}

QWebRouter::RouteFunction composeRouteFunction(QWebRouter::RouteFunction f, 
                                               QWebRouter::RouteFunction g) {
    return [f, g](QWebRequest::Ptr req, QWebResponse::Ptr resp) {
            f(req, resp);
            g(req, resp);
        };
}


template<typename... Args>
QWebRouter::RouteFunction composeRouteFunction(QWebRouter::RouteFunction f, 
                                               QWebRouter::RouteFunction g, 
                                               Args... args) {
    auto comp = [f, g](QWebRequest::Ptr req, QWebResponse::Ptr resp) {
            f(req, resp);
            g(req, resp);
        };
    
    return composeRouteFunction(comp, args...);
}

/**
 * Quick method declared to have an `/echo` path. 
 */
void ECHO_REPLY(QWebRequest::Ptr req, QWebResponse::Ptr resp) {
    resp->setStatusCode(QWebResponse::StatusCode::STATUS_OK);
    resp->writeText(req->body());
}

/**
 * @class NetworkFactory creates network queries and responses quickly and easily for testing.
 */
class NetworkFactory {
    
public:
 
    typedef std::function<void(QNetworkReply *)> SuccessCallback;
    typedef std::function<void(QNetworkReply *, QNetworkReply::NetworkError)> ErrorCallback;
    
    NetworkFactory(const QUrl &url, 
                   ErrorCallback error = nullptr,
                   SuccessCallback success = nullptr)
        : m_url(url), m_mgr(),
          m_defaultSuccess(success), 
          m_defaultError(error) {
        
    }
    
    void setDefaultSuccessCallback(SuccessCallback success) {
        m_defaultSuccess = success;
    }
    
    void setDefaultErrorCallback(ErrorCallback error) {   
        m_defaultError = error;
    }
    
    
    QNetworkReply *get(const QString &path, 
                                      const QHash<QString, QString> &queryParams = QHash<QString, QString>(), 
                                      ErrorCallback error = nullptr, 
                                      SuccessCallback success = nullptr) {
        loadDefaultCallbacksOnNull(&success, &error);
        
        QNetworkRequest req = request(path, queryParams); 
        
        QNetworkReply *reply = m_mgr.get(req);
        
        QObject::connect(reply, &QNetworkReply::finished, 
                         [reply, error, success]() {
                            if (error && reply->error() != QNetworkReply::NoError) {
                                error(reply, reply->error());
                            } else if (success) {
                                success(reply);
                            }
                        });
        
        return reply;
    }
    
    QNetworkReply *post(const QString &path, 
                        const QByteArray &postData = QByteArray(),
                        const QHash<QString, QString> &queryParams = QHash<QString, QString>(), 
                        ErrorCallback error = nullptr, 
                        SuccessCallback success = nullptr) {
        loadDefaultCallbacksOnNull(&success, &error);
        
        QNetworkRequest req = request(path, queryParams); 
        
        QNetworkReply *reply = m_mgr.post(req, postData);
        
        QObject::connect(reply, &QNetworkReply::finished, 
                         [reply, error, success]() {
                            if (error && reply->error() != QNetworkReply::NoError) {
                                error(reply, reply->error());
                            } else if (success) {
                                success(reply);
                            }
                        });
        
        return reply;
    }
    
    
    
private:
    
    const QUrl m_url;
    
    QNetworkAccessManager m_mgr;
    
    SuccessCallback m_defaultSuccess;
    ErrorCallback m_defaultError;
    
    void loadDefaultCallbacksOnNull(SuccessCallback *success, ErrorCallback *error) {
        if (!*success) {
            *success = m_defaultSuccess;
        }
        
        if (!*error) {
            *error = m_defaultError;
        }
    }
        
    

    QNetworkRequest request(const QString &path, const QHash<QString, QString> &queryParams) {      
        
        QUrl reqUrl(m_url);
        reqUrl.setPath(path);
       
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        // if on Qt 5+ have to use QUrlQuery:
        QUrlQuery query;
        for (QString key : queryParams.keys()) { 
            query.addQueryItem(key, queryParams[key]);
        }
        reqUrl.setQuery(query);
        
#else
        // it's in QURL before
        for (QString key : queryParams.keys()) { 
            reqUrl.addQueryItem(key, queryParams[key]);
        }
#endif   
        
        return QNetworkRequest(reqUrl);       
    }
    
};


} // end namespace test

} // end namespace QtWS

#endif // TESTUTILS_H
