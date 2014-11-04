#include "QWebService.h"

QWebService::QWebService(QHttpServer *server, QWebRouter *router, QObject *parent) :
    QObject(parent),
    m_server(server),
    m_router(router),
    m_registrar()
{
}

QWebService::~QWebService() {

}
