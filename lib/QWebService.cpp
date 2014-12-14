#include "QWebService.h"

QWebService::QWebService(QHttpServer *server, QWebRouter *router, QObject *parent) :
    QObject(parent),
    m_server(server),
    m_router(router),
    m_registrar()
{
}

QWebService::~QWebService() {
    m_server->close();
}

bool QWebService::startService(const QHostAddress &address, quint16 port) {
    bool out = m_server->listen(address, port);

    if (out) { // it started successfully, emit the signal
        qDebug() << "Started Web Service listening for:" << address << " on port" << port;
        emit start(address, port);
    }

    return out;
}

void QWebService::stopService() {
    m_server->close();
    emit stop();
}
