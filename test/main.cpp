#include <QTest>

#include "qhttproutetest.h"
//#include "qhttproutepathtest.h"
    
#define TEST( type ) { \
    type test; \
    QTest::qExec( &test ); \
    }
    
int main(int argc, char *argv[])
{
    TEST(QHttpRouteTest)
//    TEST(QHttpRoutePathTest)
        
    return 0;
}

#undef TEST
