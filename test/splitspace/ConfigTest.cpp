#include <catch/catch.hpp>
#include <splitspace/Config.hpp>

TEST_CASE( "Configuration test", "[Config]") {

    SECTION( "Empty configuration" ) {
        splitspace::Config config;

        REQUIRE( config.parse("test_data/empty.json") == true );

        REQUIRE( config.window.width == 640 );
        REQUIRE( config.window.height == 480 );
        REQUIRE( config.window.fullscreen == false );
        REQUIRE( config.window.caption.empty() == true );

        REQUIRE( config.log.logFile.empty() == true );
        REQUIRE( config.log.level == splitspace::LOG_WARN );

        REQUIRE( config.scenes.empty() == true );
        REQUIRE( config.matLibs.empty() == true );
        
    }
}
