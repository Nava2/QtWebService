#pragma once
<<<<<<< HEAD
<<<<<<< HEAD
#ifndef Q_HTTP_ROUTE_PATH_TEST_H
#define Q_HTTP_ROUTE_PATH_TEST_H
=======
#define QHTTPROUTEPATHTEST_H
>>>>>>> 838be6b... Fixes most compiling issues, adds qt-maybe, might remove it lator (it's not as useful as I thought..). Need to reimplement proper normal routing, regex should work.
=======
#ifndef Q_HTTP_ROUTE_PATH_TEST_H
#define Q_HTTP_ROUTE_PATH_TEST_H
>>>>>>> f4f4ce7... Fixes all compiling issues.. qt-maybe taken off build chain, it doesn't play nicely with mingw apparently. Commented out lots of tests. :(

#include <QObject>
#include <QStringList>
#include <router/qhttproutepath.h>

class QHttpRoutePathTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();
    void cleanup();
    
    void create();
    
    void matching();
    
private: 
    
    void checkRoutePath(QHttpRoutePath *path, 
                        QString route,
                        bool isVar, QString varName, 
                        QStringList validValues, 
                        QHttpRoutePath::PathSpecificity spec);

};

#endif // QHTTPROUTEPATHTEST_H
