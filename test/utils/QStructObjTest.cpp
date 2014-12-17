/*
 * Copyright 2014 Kevin Brightwell <kevin.brightwell2@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <catch/catch.hpp>

#include "utils/QStructObjBuilder.h"

SCENARIO( "Creating simple Structured Objects via QStructObj::Builder", "[QWebService] [utils] [QStructObj]" ) {

    GIVEN( "A QStructObj::Builder for an Object" )
    {
        QStructObj::Builder builder = QStructObj::Builder::create();
        
        WHEN( "Using invalid names, an logic_error is thrown" ) {
        
            REQUIRE_THROWS_AS( builder.boolean("", true), std::logic_error);
            REQUIRE_THROWS_AS( builder.string("", "foo"), std::logic_error);
            REQUIRE_THROWS_AS( builder.number("", 2.3), std::logic_error);
            REQUIRE_THROWS_AS( builder.array(""), std::logic_error);
            REQUIRE_THROWS_AS( builder.object(""), std::logic_error);
        }
        
        WHEN( "Using array methods on an object builder, logic_error is thrown" ) {
            
            REQUIRE_THROWS_AS( builder.boolean(true), std::logic_error);
            REQUIRE_THROWS_AS( builder.string("foo"), std::logic_error);
            REQUIRE_THROWS_AS( builder.number(2.3f), std::logic_error);
            REQUIRE_THROWS_AS( builder.array(), std::logic_error);
            REQUIRE_THROWS_AS( builder.object(), std::logic_error);
            
        }
        
        WHEN( "Inserting values, no errors are thrown." ) {
            builder.boolean("a", true)
                   .string("b", "foo")
                   .number("c", 20l)
                   .array("d").close()
                   .object("e").close();
        }
    }
    
    GIVEN( "A QStructObj::Builder for an Array" )
    {
        QStructObj::Builder builder = QStructObj::Builder::create().array("foo");
        
        WHEN( "Using object methods, an logic_error is thrown" ) {
        
            REQUIRE_THROWS_AS( builder.boolean("bar", true), std::logic_error);
            REQUIRE_THROWS_AS( builder.string("bar", "foo"), std::logic_error);
            REQUIRE_THROWS_AS( builder.number("bar", 2.3), std::logic_error);
            REQUIRE_THROWS_AS( builder.array("bar"), std::logic_error);
            REQUIRE_THROWS_AS( builder.object("bar"), std::logic_error);
        }
    }
}
