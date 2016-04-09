#include <catch/catch.hpp>
#include <splitspace/EventManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/LogManager.hpp>

using namespace splitspace;

class TestListener: public EventListener {
public:
    TestListener(): EventListener(EVM_GAME|EVM_INPUT|EVM_WINDOW, "TestListener" )
    {}

    virtual void onEvent(splitspace::Event *ev) {
        static_cast<void>(ev);
    }
};

TEST_CASE( "EventManager test", "[EventManager]") {
    Engine *engine = new Engine();
    engine->logManager = new LogManager();

    SECTION( "Add and remove valid EventListener" ) {
        EventManager *evMgr = new EventManager(engine->logManager);
        EventListener *l = new TestListener();

        REQUIRE( evMgr->addListener(l) == true );
        REQUIRE( evMgr->getListeners().size() == 1 );
        REQUIRE( evMgr->getListeners()[0] == l );

        REQUIRE( evMgr->removeListener(l) == true );
        REQUIRE( evMgr->getListeners().size() == 0 );
    }
    
    SECTION( "Add and remove invalid EventListener" ) {
        EventManager *evMgr = new EventManager(engine->logManager);
        EventListener *l1 = new TestListener();
        EventListener *l2 = new TestListener();

        REQUIRE( evMgr->addListener(nullptr) == false );
        REQUIRE( evMgr->addListener(l1) == true );

        REQUIRE( evMgr->removeListener(l2) == false );
        REQUIRE( evMgr->getListeners().size() == 1 );
        REQUIRE( evMgr->getListeners()[0] == l1 );


    }

}
