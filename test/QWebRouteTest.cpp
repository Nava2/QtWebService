#include "QWebRouteTest.h"

#include <QtTest>

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QHash>

void QWebRouteTest::initTestCase()
{
    // Called before the first testfunction is executed
}

void QWebRouteTest::cleanupTestCase()
{
    // Called after the last testfunction was executed
}

void QWebRouteTest::init()
{
    // Called before each testfunction is executed
}

void QWebRouteTest::cleanup()
{
    // Called after every testfunction
}

inline 
QStringList spl(QString path) {
    return path.split('/', QString::SkipEmptyParts);
}

#define CHECK_PATH(ptr, var, path) \
    ResultPtr var ; \
    { \
        QString qstr( path ); \
        var = ( ptr )->checkPath(qstr, spl(qstr)); \
    }

void QWebRouteTest::checkRegexPath()
{
    typedef QWebRoute::ParsedRoute::Ptr ResultPtr;
    {
        auto ptr = m_factory->createRegex("/(?<foo>foo\\d+)/(bar|foo)/");
        CHECK_PATH(ptr, result, "/foo2/bar/");

        QVERIFY(result);
        QCOMPARE(result->urlParams().size(), 1);
        QCOMPARE(result->urlParams()["foo"], QString("foo2"));
        QCOMPARE(result->splat().size(), 1);

        const QStringList groups = {"/foo2/bar/", "foo2", "bar"};
        QCOMPARE(result->groups(), groups);

    } {
        auto ptr = m_factory->createRegex("/test/(foo|bar|wat)/");
        CHECK_PATH(ptr, result, "/test/bar/");

        QVERIFY(result);
        QCOMPARE(result->urlParams().size(), 0);
        QCOMPARE(result->groups().size(), 2);
        QCOMPARE(result->splat().size(), 1);

        const QStringList groups = {"/test/bar/", "bar"};
        QCOMPARE(result->groups(), groups);
        QCOMPARE(result->splat()[0], groups[1]);
    } {
        // bad path
        auto ptr = m_factory->createRegex("/(foo\\d+)/(bar|wat)/");
        CHECK_PATH(ptr, result, "/foo2");

        QVERIFY(!result);
    }
}

/*
 * :: GOOD ::
 *
/:name$one
/:nam$one|two
/one|two
/one|two/foo
 */
void QWebRouteTest::checkDSLPath_Valid()
{

    typedef QWebRoute::ParsedRoute::Ptr ResultPtr;

    {   // direct match
        auto ptr = m_factory->create("/one/two/three");
        CHECK_PATH(ptr, result, "/one/two/three");

        QVERIFY(result);
        QCOMPARE(result->urlParams().size(), 0);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/one/two/three"}));
    } {
        // single name and accepted value
        auto ptr = m_factory->create("/:name$one");
        CHECK_PATH(ptr, result, "/one/");

        QVERIFY(result);
        auto params = QHash<QString, QString>({{"name", "one"}});
        QCOMPARE(result->urlParams(), params);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/one", params["name"]}));
    } {
        // single name, multiple values
        auto ptr = m_factory->create("/:name$one|two");
        CHECK_PATH(ptr, result, "/one/");

        QVERIFY(result);
        auto params = QHash<QString, QString>({{"name", "one"}});
        QCOMPARE(result->urlParams(), params);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/one", params["name"]}));
    } {
        // no name, multiple values
        auto ptr = m_factory->create("/one|two");
        CHECK_PATH(ptr, result, "/one/");

        QVERIFY(result);
        QCOMPARE(result->urlParams().size(), 0);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/one"}));
    } {
        // two level path, bi-options, no capture
        auto ptr = m_factory->create("/one|two/foo");
        CHECK_PATH(ptr, result, "/one/foo");

        QVERIFY(result);
        QCOMPARE(result->urlParams().size(), 0);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/one/foo"}));
    } {
        // wildcard, with name
        auto ptr = m_factory->create("/:name$*.html");
        CHECK_PATH(ptr, result, "/index.html");

        QVERIFY(result);
        auto params = QHash<QString, QString>({{"name", "index.html"}});
        QCOMPARE(result->urlParams(), params);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/index.html", "index.html"}));
    } {
        // name no spec, second name with wildcard
        auto ptr = m_factory->create("/:user/:file$*.html");
        CHECK_PATH(ptr, result, "/foo/index.html");

        QVERIFY(result);
        auto params = QHash<QString, QString>({{"user", "foo"}, {"file", "index.html"}});
        QCOMPARE(result->urlParams(), params);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/foo/index.html", "foo", "index.html"}));
    } {
        // name wild spec, second name with wildcard
        auto ptr = m_factory->create("/:user$*/:file$*.html");
        CHECK_PATH(ptr, result, "/foo/index.html");

        QVERIFY(result);
        auto params = QHash<QString, QString>({{"user", "foo"}, {"file", "index.html"}});
        QCOMPARE(result->urlParams(), params);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/foo/index.html", "foo", "index.html"}));
    } {
        // bi-optional, no capture
        auto ptr = m_factory->create("/one|two/foo");
        CHECK_PATH(ptr, result, "/one/foo");

        QVERIFY(result);
        QCOMPARE(result->urlParams().size(), 0);
        QCOMPARE(result->splat().size(), 0);
        QCOMPARE(result->groups(), QStringList({"/one/foo"}));
    }

}

/* :: BAD ::
 *
/$bad
/|wat
/wat|
/_no_colon$one|two|three
:no_slash$confusion
/:name$:bad_colon
 */
void QWebRouteTest::checkDSLPath_Invalid() {

    typedef QWebRoute::ParsedRoute::Ptr ResultPtr;

    // FAILURES:
    {
        for (QString path: {"/|one|two", "/$bad",
             "/wat|", "/_no_colon$one|two|three",
             "/:name$:bad_colon"}) {
            auto ptr = m_factory->create(path);
            QVERIFY2(!ptr, QTest::toString("Pointer created from: " % path));
            QVERIFY2(m_factory->lastError() == QWebRouteFactory::PATH_PART_ERROR,
                     QTest::toString("No Error on: " % path));
        }
    } {
        auto ptr = m_factory->create(":validName$validOption");
        QVERIFY(!ptr);
        QCOMPARE(m_factory->lastError(), QWebRouteFactory::ROOT_MISSING);
    }
}

#undef CHECK_PATH
