#ifndef QWEBSERVICE_H
#define QWEBSERVICE_H

#include <QObject>
#include <QSharedPointer>
#include <QHttpServer/qhttpserver.h>
#include <QHttpServer/qhttpresponse.h>
#include <QHttpServer/qhttprequest.h>

#include "private/qtwebserviceapi.h"
#include "private/qtwebservicefwd.h"

class QTWEBSERVICE_API QWebService : public QObject {
    Q_OBJECT

public:
    /// Defines the signature for a routing function
    typedef std::function<void(const QSharedPointer<QWebRoutedRequest>, const QSharedPointer<QHttpResponse>)> RouteFunction;

    typedef QHttpRequest::HttpMethod HttpMethod;

    virtual
    ~QWebService();

private:
    QWebService(QWebServiceConfig *config, QObject *parent);

    QHttpServer *_server;

};

#endif // QWEBSERVICE_H
