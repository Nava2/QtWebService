#include "qhttproutedrequest.h"


QHttpRoutedRequest::QHttpRoutedRequest(QHttpRequest *httpReq,
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

QSharedPointer<QHttpRoutedRequest> QHttpRoutedRequest::create(QHttpRequest *httpReq,
                                                              const QHash<QString, QString> &postParams,
                                                              const QHash<QString, QString> &urlParams,
                                                              const QStringList &splat,
                                                              QObject *parent) {
    QHttpRoutedRequest *ptr = new QHttpRoutedRequest(httpReq, postParams, urlParams, splat, parent);

    if (parent != nullptr) {
        return QSharedPointer<QHttpRoutedRequest>(ptr, &QObject::deleteLater);
    }

    return QSharedPointer<QHttpRoutedRequest>(ptr);
}
