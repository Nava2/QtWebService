#include "router/QWebRequest.h"


QWebRequest::QWebRequest(QHttpRequest *httpReq,
                                       const QHash<QString, QString> &postParams,
                                       const QHash<QString, QString> &urlParams,
                                       const QStringList &splat,
                                       QObject *parent) :
    QObject(parent),
    m_req(httpReq),
    m_urlParams(urlParams),
    m_postParams(postParams),
    m_splat(splat)
{

}

QWebRequest::~QWebRequest() {

}

QSharedPointer<QWebRequest> QWebRequest::create(QHttpRequest *httpReq,
                                                              const QHash<QString, QString> &postParams,
                                                              const QHash<QString, QString> &urlParams,
                                                              const QStringList &splat,
                                                              QObject *parent) {
    QWebRequest *ptr = new QWebRequest(httpReq, postParams, urlParams, splat, parent);

    if (parent != nullptr) {
        return QSharedPointer<QWebRequest>(ptr, &QObject::deleteLater);
    }

    return QSharedPointer<QWebRequest>(ptr);
}
