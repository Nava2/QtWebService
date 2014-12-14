
#include "catch/catch.hpp"

#include "router/QWebRoute.h"

#include <string>

SCENARIO( "Create and match Regex paths", "[QWebRoute]" ) {
    
    typedef QWebRoute::ParsedRoute::Ptr ResultPtr;
    
    const QWebRouteFactory factory;
    
    WHEN( "Invalid regex path ending '/a/'" ) {
        QWebRoute::Ptr ptr = factory.createRegex("/a/");
        REQUIRE_FALSE(ptr);
        REQUIRE(factory.lastError() == QWebRouteFactory::QWebRouteFactory::SLASH_TERMINATOR);
    }
    
    GIVEN( "Named group and unamed group '/(?<foo>foo\\d+)/(bar|foo)'" ) {
        QWebRoute::Ptr ptr = factory.createRegex("/(?<foo>foo\\d+)/(bar|foo)");
        REQUIRE(ptr);
        
        WHEN("Matched against '/foo2/bar'") {
            ResultPtr result = ptr->checkPath("/foo2/bar"); 
            REQUIRE( result ); 
            
            REQUIRE(result->urlParams().size() == 1);
            REQUIRE(result->urlParams()["foo"] == "foo2");
            REQUIRE(result->splat().size() == 1);

            const QStringList groups = {"/foo2/bar", "foo2", "bar"};
            REQUIRE(result->groups() == groups);
        }
        
    }
    
    GIVEN( "Two-level, single OR specification '/test/(foo|bar|wat)'" ) {
        QWebRoute::Ptr ptr = factory.createRegex("/test/(foo|bar|wat)");
        REQUIRE(ptr);
        
        WHEN( "Matched against '/test/bar'" ) {
            ResultPtr result = ptr->checkPath("/test/bar");
            REQUIRE(result);
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->groups().size() == 2);
            REQUIRE(result->splat().size() == 1);

            const QStringList groups = {"/test/bar", "bar"};
            REQUIRE(result->groups() == groups);
            REQUIRE(result->splat()[0] == groups[1]);
        }
        
        THEN( "Matched against '/test/foo'" ) {
            ResultPtr result = ptr->checkPath("/test/foo");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->groups().size() == 2);
            REQUIRE(result->splat().size() == 1);

            const QStringList groups = {"/test/foo", "foo"};
            REQUIRE(result->groups() == groups);
            REQUIRE(result->splat()[0] == groups[1]);
        }
        
        THEN( "Matched against '/test/test' (invalid)" ) {
            ResultPtr result = ptr->checkPath("/test/test");
            REQUIRE_FALSE(result);
        }
        
        THEN( "Matched against '/test' (invalid)" ) {
            ResultPtr result = ptr->checkPath("/test");
            REQUIRE_FALSE(result);
        }
    }
    
}

SCENARIO( "Create and match DSL paths", "[QWebRoute]" ) {
    
    typedef QWebRoute::ParsedRoute::Ptr ResultPtr;
    
    const QWebRouteFactory factory;
    GIVEN( "Invalid syntax" ) {
        
        WHEN( "Invalid path ending '/a/'" ) {
            QWebRoute::Ptr ptr = factory.create("/a/");
            REQUIRE_FALSE(ptr);
            REQUIRE(factory.lastError() == QWebRouteFactory::SLASH_TERMINATOR);
        }
        
        WHEN("Extra leading |, '/|one|two") {
            auto ptr = factory.create("/|one|two");
            REQUIRE_FALSE(ptr);
            REQUIRE(factory.lastError() == QWebRouteFactory::PATH_PART_ERROR);
        }
        
        WHEN("Missing name, '/$bad") {
            auto ptr = factory.create("/$bad");
            REQUIRE_FALSE(ptr);
            REQUIRE(factory.lastError() == QWebRouteFactory::PATH_PART_ERROR);
        }
        
        WHEN("Missing OR term, '/one|") {
            auto ptr = factory.create("/one|");
            REQUIRE_FALSE(ptr);
            REQUIRE(factory.lastError() == QWebRouteFactory::PATH_PART_ERROR);
        }
        
        WHEN("Missing colon on name token, '/_no_colon$one|two'") {
            auto ptr = factory.create("/_no_colon$one|two");
            REQUIRE_FALSE(ptr);
            REQUIRE(factory.lastError() == QWebRouteFactory::PATH_PART_ERROR);
        }
        
        WHEN("Colon on specification token, '/:name$:bad_colon'") {
            auto ptr = factory.create("/:name$:bad_colon");
            REQUIRE_FALSE(ptr);
            REQUIRE(factory.lastError() == QWebRouteFactory::PATH_PART_ERROR);
        }
        
        WHEN("Missing ROOT, ':name/a'") {
            auto ptr = factory.create(":name/a");
            REQUIRE_FALSE(ptr);
            REQUIRE(factory.lastError() == QWebRouteFactory::ROOT_MISSING);
        }
    }
        
    GIVEN( "Multi-level, direct match '/one/two/three'" ) {
        QWebRoute::Ptr ptr = factory.createRegex("/one/two/three");
        REQUIRE(ptr);
        
        WHEN( "Matched against /one/two/three" ) {
            ResultPtr result = ptr->checkPath("/one/two/three");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat().size() == 0);
            REQUIRE(result->groups() == QStringList({"/one/two/three"}));
        }
        
        THEN( "Matched against /one/two (invalid)" ) {
            ResultPtr result = ptr->checkPath("/one/two");
            REQUIRE_FALSE(result);
        }
        
        THEN( "Matched against /two/one/three (invalid)" ) {
            ResultPtr result = ptr->checkPath("/two/one/three");
            REQUIRE_FALSE(result);
        }
    }
    
    GIVEN( "Single level, direct match with name '/:name$one'" ) {
        QWebRoute::Ptr ptr = factory.create("/:name$one");
        REQUIRE(ptr);
        
        WHEN( "Matched against /one" ) {
            ResultPtr result = ptr->checkPath("/one");
            REQUIRE(result);
            
            auto params = QHash<QString, QString>({{"name", "one"}});
            REQUIRE(result->urlParams() == params);
            REQUIRE(result->groups() == QStringList({"/one", params["name"]}));
        }
        
        AND_THEN( "Matched against /none" ) {
            ResultPtr result = ptr->checkPath("/none");
            REQUIRE_FALSE(result);
        }
    }
    
    GIVEN( "Single level, multiple match with name '/:name$one|two'" ) {
        QWebRoute::Ptr ptr = factory.create("/:name$one|two");
        REQUIRE(ptr);
        
        WHEN( "Matched against /one" ) {
            ResultPtr result = ptr->checkPath("/one");
            REQUIRE(result);
            
            auto params = QHash<QString, QString>({{"name", "one"}});
            REQUIRE(result->urlParams() == params);
            REQUIRE(result->groups() == QStringList({"/one", params["name"]}));
        }
        
        THEN( "Matched against /two" ) {
            ResultPtr result = ptr->checkPath("/two");
            REQUIRE(result);
            
            auto params = QHash<QString, QString>({{"name", "two"}});
            REQUIRE(result->urlParams() == params);
            REQUIRE(result->groups() == QStringList({"/two", params["name"]}));
        }
        
        THEN( "Matched against /one|two" ) {
            ResultPtr result = ptr->checkPath("/one|two");
            REQUIRE_FALSE(result);
        }
        
        THEN( "Matched against /none" ) {
            ResultPtr result = ptr->checkPath("/none");
            REQUIRE_FALSE(result);
        }
    }
    
    GIVEN( "Single level, no name, multiple values /one|two" ) {
        QWebRoute::Ptr ptr = factory.create("/one|two");
        REQUIRE(ptr);
        
        WHEN( "Matched against /one" ) {
            ResultPtr result = ptr->checkPath("/one");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat().size() == 0);
            REQUIRE(result->groups() == QStringList({"/one"}));
        }
        
        THEN( "Matched against /two" ) {
            ResultPtr result = ptr->checkPath("/two");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat().size() == 0);
            REQUIRE(result->groups() == QStringList({"/two"}));
        }
        
        THEN( "Matched against /" ) {
            ResultPtr result = ptr->checkPath("/");
            REQUIRE_FALSE(result);
        }
    }
    
    GIVEN( "Single level, named unspecified var /:name" ) {
        QWebRoute::Ptr ptr = factory.create("/:name");
        REQUIRE(ptr);
        
        WHEN( "Matched against /foo" ) {
            ResultPtr result = ptr->checkPath("/foo");
            REQUIRE(result);
            
            auto params = QHash<QString, QString>({{"name", "foo"}});
            REQUIRE(result->urlParams() == params);
            REQUIRE(result->splat().size() == 0);
            REQUIRE(result->groups() == QStringList({"/foo", params["name"]}));
        }
        
        THEN( "Matched against /" ) {
            ResultPtr result = ptr->checkPath("/");
            REQUIRE_FALSE(result); 
        }
    }
    
    GIVEN( "Single level, named as wildcard /:name$+" ) {
        QWebRoute::Ptr ptr = factory.create("/:name$+");
        REQUIRE(ptr);
        
        WHEN( "Matched against /foo" ) {
            ResultPtr result = ptr->checkPath("/foo");
            REQUIRE(result);
            
            auto params = QHash<QString, QString>({{"name", "foo"}});
            REQUIRE(result->urlParams() == params);
            REQUIRE(result->splat().size() == 0);
            REQUIRE(result->groups() == QStringList({"/foo", params["name"]}));
        }
        
        THEN( "Matched against /" ) {
            ResultPtr result = ptr->checkPath("/");
            REQUIRE_FALSE(result); 
        }
    }
    
    GIVEN( "Single level, named as partial wildcard /:name$foo*" ) {
        QWebRoute::Ptr ptr = factory.create("/:name$foo*");
        REQUIRE(ptr);
        
        WHEN( "Matched against /foobar" ) {
            ResultPtr result = ptr->checkPath("/foobar");
            REQUIRE(result);
            
            auto params = QHash<QString, QString>({{"name", "foobar"}});
            REQUIRE(result->urlParams() == params);
            REQUIRE(result->splat().size() == 0);
            REQUIRE(result->groups() == QStringList({"/foobar", params["name"]}));
        }
        
        THEN( "Matched against /" ) {
            ResultPtr result = ptr->checkPath("/");
            REQUIRE_FALSE(result); 
        }
    }
    
    GIVEN( "Single level, unnamed wildcard /+" ) {
        QWebRoute::Ptr ptr = factory.create("/+");
        REQUIRE(ptr);
        
        WHEN( "Matched against /foo" ) {
            ResultPtr result = ptr->checkPath("/foo");
            REQUIRE(result);

            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat() == QStringList({"foo"}));
            REQUIRE(result->groups() == QStringList({"/foo", "foo"}));
        }
        
        THEN( "Matched against /+" ) {
            ResultPtr result = ptr->checkPath("/+");
            REQUIRE_FALSE(result); 
        }
    }
    
    GIVEN( "Single level, unnamed partial wildcard /foo*" ) {
        QWebRoute::Ptr ptr = factory.create("/foo*");
        REQUIRE(ptr);
        
        WHEN( "Matched against /foo" ) {
            ResultPtr result = ptr->checkPath("/foo");
            REQUIRE(result);

            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat() == QStringList({"foo"}));
            REQUIRE(result->groups() == QStringList({"/foo", "foo"}));
        }
        
        THEN( "Matched against /foobar" ) {
            ResultPtr result = ptr->checkPath("/foobar");
            REQUIRE(result);

            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat() == QStringList({"foobar"}));
            REQUIRE(result->groups() == QStringList({"/foobar", "foobar"}));
        }
        
        THEN( "Matched against /" ) {
            ResultPtr result = ptr->checkPath("/");
            REQUIRE_FALSE(result); 
        }
    }
    
    GIVEN( "Two level, multiple values, no capture /one|two/foo" ) {
        QWebRoute::Ptr ptr = factory.create("/one|two/foo");
        REQUIRE(ptr);
        
        WHEN( "Matched against /one/foo" ) {
            ResultPtr result = ptr->checkPath("/one/foo");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat().size() == 0);
            REQUIRE(result->groups() == QStringList({"/one/foo"}));
        }
        
        THEN( "Matched against /two/foo" ) {
            ResultPtr result = ptr->checkPath("/two/foo");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat().size() == 0);
            REQUIRE(result->groups() == QStringList({"/two/foo"}));
        }
        
        THEN( "Matched against /foo/two" ) {
            ResultPtr result = ptr->checkPath("/foo/two");
            REQUIRE_FALSE(result);
        }
    }
    
    GIVEN( "Single level, multiple values with wildcards /one*|two+" ) {
        QWebRoute::Ptr ptr = factory.create("/one*|two+");
        REQUIRE(ptr);
        
        WHEN( "Matched against /one" ) {
            ResultPtr result = ptr->checkPath("/one");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat() == QStringList({"one"}));
            REQUIRE(result->groups() == QStringList({"/one", "one"}));
        }
        
        THEN( "Matched against /oneA" ) {
            ResultPtr result = ptr->checkPath("/oneA");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat() == QStringList({"oneA"}));
            REQUIRE(result->groups() == QStringList({"/oneA", "oneA"}));
        }
        
        THEN( "Matched against /twoA" ) {
            ResultPtr result = ptr->checkPath("/twoA");
            REQUIRE(result);
            
            REQUIRE(result->urlParams().size() == 0);
            REQUIRE(result->splat() == QStringList({"twoA"}));
            REQUIRE(result->groups() == QStringList({"/twoA", "twoA"}));
        }
        
        THEN( "Matched against /two" ) {
            ResultPtr result = ptr->checkPath("/two");
            REQUIRE_FALSE(result);
        }
    }

}
