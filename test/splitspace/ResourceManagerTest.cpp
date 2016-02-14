#include <catch/catch.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Resource.hpp>
#include <splitspace/Entity.hpp>

TEST_CASE( "ResourceManager test", "[ResourceManager]") {
   
    using namespace splitspace;
    Engine *engine = new Engine();
    engine->logManager = new LogManager();

    SECTION( "Empty Scene and Material lists" ) {
        ResourceManager *manager = new ResourceManager(engine);
        
        std::vector<std::string> emptyStringVector;

        REQUIRE( manager->loadMaterialLib(emptyStringVector) == true );
        REQUIRE( manager->createSceneManifests(emptyStringVector) == true );
    }

    SECTION( "Invalid Scene and Material names" ) {
        ResourceManager *manager = new ResourceManager(engine);

        REQUIRE( manager->loadMaterialLib("fake") == false );
        REQUIRE( manager->createScene("fake") == false );
    }

    SECTION( "Empty resource names" ) {
        ResourceManager *manager = new ResourceManager(engine);
        ResourceManifest *manifest = new ResourceManifest(RES_UNKNOWN);

        REQUIRE( manager->loadMaterialLib("") == false );
        REQUIRE( manager->createScene("") == false );
        REQUIRE( manager->addManifest(manifest) == false );
        REQUIRE( manager->loadResource("") == nullptr );
        REQUIRE( manager->unloadResource("") == false );
    }
    
    SECTION( "Invalid Scene and Material names in list" ) {
        ResourceManager *manager = new ResourceManager(engine);
        
        std::vector<std::string> stringVector;
        stringVector.push_back("fake");

        REQUIRE( manager->loadMaterialLib(stringVector) == false );
        REQUIRE( manager->createSceneManifests(stringVector) == false );
    }

    SECTION( "Manifest doubling" ) {
        ResourceManager *manager = new ResourceManager(engine);
        ResourceManifest *manifest = new ResourceManifest(RES_UNKNOWN);
        manifest->name = "TestResource";

        REQUIRE( manager->addManifest(manifest) == true );
        REQUIRE( manager->addManifest(manifest) == false );
    }

    SECTION ( "Loading nonexistent Resource" ) {
        ResourceManager *manager = new ResourceManager(engine);

        REQUIRE( manager->loadResource("fake") == nullptr );
    }

    SECTION ( "Loading valid Resource") {
        ResourceManager *manager = new ResourceManager(engine);
        EntityManifest *manifest = new EntityManifest();
        Resource *res = nullptr;
        manifest->name = "TestEntity";
        manager->addManifest(manifest);

        REQUIRE( (res = manager->loadResource(manifest->name)) != nullptr );
        REQUIRE( manager->loadResource(manifest->name) == res );
    }

    SECTION( "Unloading Resource which is not loaded" ) {
        ResourceManager *manager = new ResourceManager(engine);
        ResourceManifest *manifest = new ResourceManifest(RES_UNKNOWN);
        manifest->name = "TestResource";
        manager->addManifest(manifest);

        REQUIRE( manager->unloadResource(manifest->name) == false );
    }
    
    SECTION( "Unloading Resource which does not exist" ) {
        ResourceManager *manager = new ResourceManager(engine);

        REQUIRE( manager->unloadResource("fake") == false );
    }

    SECTION( "Garbage collection" ) {
        ResourceManager *manager = new ResourceManager(engine);

        for( int i = 0;i<10;i++) {
            EntityManifest *manifest = new EntityManifest();
            manifest->name = "TestEntity"+std::to_string(i);
            manager->addManifest(manifest);
            manager->loadResource(manifest->name);
        }

        for( int i = 3; i<8; i++) {
            manager->unloadResource("TestEntity"+std::to_string(i));
        }

        REQUIRE( manager->collectGarbage() == 5 );
    }

}
