#include <QTest>

#include "QWebRouteTest.h"

//#include "qhttproutepathtest.h"
    
#define TEST( type ) { \
    type test; \
    QTest::qExec( &test ); \
    }
    
int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    TEST(QWebRouteTest)
//    TEST(QHttpRoutePathTest)
        
    return 0;
}

#undef TEST
