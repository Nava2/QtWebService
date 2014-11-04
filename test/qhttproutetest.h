#pragma once
#ifndef Q_HTTP_ROUTE_TEST_H
#define Q_HTTP_ROUTE_TEST_H

#include <QObject>

#include <router/QWebRoute.h>

class QWebRouteTest : public QObject
{
    Q_OBJECT
    
public: 
    QWebRouteTest() :
        QObject(),
        m_factory(new QWebRouteFactory) {

    }
    
    virtual ~QWebRouteTest() {
        delete m_factory;
    }
    
private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();

    void checkRegexPath();

    void checkDSLPath_Valid();
    void checkDSLPath_Invalid();
    
private:

    const QWebRouteFactory *m_factory;
};

#endif // QWebRouteTEST_H
