#include "QWebService.h"

QWebService::QWebService(QHttpServer *server, QWebRouter *router, QSet<QWebMiddleWare *> wares, QObject *parent) :
    QObject(parent),
    m_server(server),
    m_router(router),
    m_wares(wares)
{
}

QWebService::~QWebService() {

}
