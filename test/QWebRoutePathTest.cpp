#include "qhttproutepathtest.h"

#include <QTest>
#include <qhttproutepath.h>

void QHttpRoutePathTest::initTestCase()
{
    // Called before the first testfunction is executed
}

void QHttpRoutePathTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void QHttpRoutePathTest::init()
{
    // Called before each testfunction is executed
}

void QHttpRoutePathTest::cleanup()
{
    // Called after every testfunction
}

void QHttpRoutePathTest::checkRoutePath(QHttpRoutePath* path, QString route,
                                        bool isVar, QString varName, 
                                        QStringList validValues, 
                                        QHttpRoutePath::PathSpecificity spec)
{
    QVERIFY2(path->expr() == route, "path->expr() != route");
    
    QCOMPARE(path->isVariable(), isVar);
    QCOMPARE(path->variable(), varName);
    QCOMPARE(path->specificity(), spec);
    QCOMPARE(path->validValues(), validValues);
}


void QHttpRoutePathTest::create()
{  
    // test some invalids:
    {
        auto invalid = QHttpRoutePath::create(" ", this);
        QVERIFY2(invalid == nullptr, "Created valid from \" \" route.");
    } {
        auto invalid = QHttpRoutePath::create("/a", this);
        QVERIFY2(invalid == nullptr, "Created valid from \"/a\" route.");
    } {
        auto invalid = QHttpRoutePath::create("**", this);
        QVERIFY2(invalid == nullptr, "Created valid from \"**\" route.");
    }

    auto withVariable = QHttpRoutePath::create(":userId", this);
    checkRoutePath(withVariable, ":userId",
        true, "userId",
        QStringList(), QHttpRoutePath::SPEC_WILDCARD
    );

    auto withVariableAndSpec = QHttpRoutePath::create(":userType$user|admin", this);
    checkRoutePath(withVariableAndSpec, ":userType$user|admin",
        true, "userType",
        QStringList({"admin", "user"}), QHttpRoutePath::SPEC_EXACT_OR
    );
    
    auto onlyWild = QHttpRoutePath::create("*", this);
    checkRoutePath(onlyWild, "*", 
                   false, "", 
                   QStringList(), QHttpRoutePath::SPEC_WILDCARD);
}

void QHttpRoutePathTest::matching()
{
    // Exact spec:
    auto exact = QHttpRoutePath::create("user", this);
    QVERIFY2(exact->checkPath("user"), "Exact match not found");
    QVERIFY2(!exact->checkPath("invalid"), "Exact match passed on erroneous value."); 

    // OR
    auto multi = QHttpRoutePath::create("dog|cat|ham", this);
    QVERIFY(multi->checkPath("dog"));
    QVERIFY(multi->checkPath("cat"));
    QVERIFY(multi->checkPath("ham"));
    QVERIFY(!multi->checkPath("bourbon"));
    
    // WILDCARD
    auto wildcard = QHttpRoutePath::create("*", this);
    QVERIFY(wildcard->checkPath("anything"));
    QVERIFY(wildcard->checkPath("%20"));   
}


