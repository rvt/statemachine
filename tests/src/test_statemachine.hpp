#include <catch2/catch.hpp>

#include <statemachine.h>
#include <iostream>
#include "arduinostubs.hpp"

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&& ... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

TEST_CASE("Should correctly get current value with simple state machine", "[statemachine]") {
    auto firstState = new State;
    auto unusedState0 = new State;
    auto secondState = new State;
    auto unusedState1 = new State;
    auto thirdState = new State;

    auto run1 = [secondState]() {
        std::cerr << "firstState\n";
        return secondState->id();
    };
    auto run2 = [thirdState]() {
        std::cerr << "secondState\n";
        return thirdState->id();
    };
    auto run3 = [thirdState]() {
        std::cerr << "thirdState\n";
        return thirdState->id();
    };

    firstState->setRunnable(run1);
    secondState->setRunnable(run2);
    thirdState->setRunnable(run3);

    StateMachine<3> machine {{firstState, secondState, thirdState} };
    machine.start();

    // Should initially be at the first state
    REQUIRE(machine.current(firstState->id()) == true);

    // then second state
    machine.handle();
    REQUIRE(machine.current(secondState->id()) == true);

    // then third state
    machine.handle();
    REQUIRE(machine.current(thirdState->id()) == true);

    // well advance and should always end in last state
    machine.handle();
    REQUIRE(machine.current(thirdState->id()) == true);
    REQUIRE(machine.current(secondState->id()) == false);
    // Third state still active
    machine.handle();
    REQUIRE(machine.current(thirdState->id()) == true);
}

TEST_CASE("Should handle timed states", "[statemachine]") {
    auto firstState = new State;
    auto secondState = new StateTimed{10};
    auto thirdState = new State;

    auto firstStateId = firstState->id();
    auto secondStateId = secondState->id();
    auto thirdStateId = thirdState->id();
    auto run1 = [secondStateId]() {
        std::cerr << "firstState\n";
        return secondStateId;
    };
    auto run2 = [thirdStateId]() {
        std::cerr << "secondState\n";
        return thirdStateId;
    };
    auto run3 = [thirdStateId]() {
        std::cerr << "thirdState\n";
        return thirdStateId;
    };

    firstState->setRunnable(run1);
    secondState->setRunnable(run2);
    thirdState->setRunnable(run3);

    StateMachine<3> machine {{firstState, secondState, thirdState} };
    machine.start();
    // Should initially be at the first state
    REQUIRE(machine.current(firstState->id()) == true);

    // then second state
    machine.handle();
    REQUIRE(machine.current(secondState->id()) == true);

    // Still at second state
    machine.handle();
    REQUIRE(machine.current(secondState->id()) == true);

    // should have been advanced to third state
    millisStubbed = 11;
    machine.handle();
    REQUIRE(machine.current(thirdState->id()) == true);
}
