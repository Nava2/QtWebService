#pragma once
#ifndef Q_HTTP_ROUTE_TEST_H
#define Q_HTTP_ROUTE_TEST_H

#include <QObject>

#include <router/qhttproute.h>

class QHttpRouteTest : public QObject
{
    Q_OBJECT
    
public: 
    QHttpRouteTest() : 
        QObject(),
        m_factory(new QHttpRouteFactory) {

    }
    
    virtual ~QHttpRouteTest() {
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

    const QHttpRouteFactory *m_factory;
};

#endif // QHTTPROUTETEST_H
