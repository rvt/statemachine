#include <catch2/catch.hpp>

#include <statemachine.h>
#include <iostream>
#include "arduinostubs.hpp"

TEST_CASE( "Should correctly get current value with simple state machine", "[statemachine]" ) {
    State* thirdState;
    State* secondState;
    State* firstState;
    thirdState = new State([](){std::cerr << "thirdState\n";return 2;});
    secondState = new State([](){std::cerr << "secondState\n";return 2;});
    firstState = new State([](){std::cerr << "firstState\n";return 1;});

    StateMachine machine({firstState, secondState, thirdState} );
    machine.start();

    // Should initially be at the first state
    REQUIRE(machine.current(firstState) == true);

    // then second state
    machine.handle();
    REQUIRE(machine.current(secondState) == true);

    // then third state
    machine.handle();
    REQUIRE(machine.current(thirdState) == true);

    // well advance and should always end in last state
    machine.handle();
    REQUIRE(machine.current(thirdState) == true);
    REQUIRE(machine.current(secondState) == false);
}

TEST_CASE( "Should handle timed states", "[statemachine]" ) {
    State* thirdState;
    State* secondState;
    State* firstState;
    thirdState = new State([](){std::cerr << "thirdState\n";return 2;});
    secondState = new StateTimed(10, [](){std::cerr << "secondState\n";return 2;});
    firstState = new State([](){std::cerr << "firstState\n";return 1;});

    StateMachine machine({firstState, secondState, thirdState} );
    machine.start();
    // Should initially be at the first state
    REQUIRE(machine.current(firstState) == true);

    // then second state
    machine.handle();
    REQUIRE(machine.current(secondState) == true);

    // Still at second state
    machine.handle();
    REQUIRE(machine.current(secondState) == true);

    // should have been advanced to third state
    millisStubbed = 11;
    machine.handle();
    REQUIRE(machine.current(thirdState) == true);
}
