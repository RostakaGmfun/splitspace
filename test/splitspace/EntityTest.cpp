#include <catch/catch.hpp>
#include <splitspace/Entity.hpp>
#include <splitspace/Engine.hpp>

TEST_CASE( "Entity test", "[Entity]") {
    splitspace::Engine *engine = new splitspace::Engine();
    splitspace::EntityManifest *entityManifest = new splitspace::EntityManifest();

    SECTION( "Specify parent in constructor" ) {
        splitspace::Entity *root = new splitspace::Entity(engine, entityManifest, nullptr);
        splitspace::Entity *child = new splitspace::Entity(engine, entityManifest, root);

        REQUIRE( child->getParent() == root );
        REQUIRE( root->getChildren().size() == 1 );
        REQUIRE( root->getChildren()[0] == child );
    }

    SECTION( "Add and remove child dynamically" ) {
        splitspace::Entity *root = new splitspace::Entity(engine, entityManifest, nullptr);
        splitspace::Entity *child = new splitspace::Entity(engine, entityManifest, nullptr);

        REQUIRE( root->addChild(child) == true );
        REQUIRE( root->getChildren().size() == 1 );
        REQUIRE( root->getChildren()[0] == child );
        REQUIRE( child->getParent() == root );

        REQUIRE( root->removeChild(child) == true );
        REQUIRE( root->getChildren().size() == 0 );
        REQUIRE( child->getParent() == nullptr );
    }

}
