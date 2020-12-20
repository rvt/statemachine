#include <catch2/catch.hpp>

#include <statemachine.hpp>
#include <iostream>
#include "arduinostubs.hpp"

template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&& ... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

TEST_CASE("Should correctly get current value with simple state machine", "[statemachine]") {
    auto firstState = new State;
    auto secondState = new State;
    auto thirdState = new State;

    auto run1 = [secondState]() {
        std::cerr << "firstState\n";
        return secondState;
    };
    auto run2 = [thirdState]() {
        std::cerr << "secondState\n";
        return thirdState;
    };
    auto run3 = [thirdState]() {
        std::cerr << "thirdState\n";
        return thirdState;
    };

    firstState->setRunnable(run1);
    secondState->setRunnable(run2);
    thirdState->setRunnable(run3);

    StateMachine machine {firstState };
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
    // Third state still active
    machine.handle();
    REQUIRE(machine.current(thirdState) == true);
}

TEST_CASE("Should handle timed states", "[statemachine]") {
    auto firstState = new State;
    auto secondState = new StateTimed{10};
    auto thirdState = new State;

    auto run1 = [secondState]() {
        std::cerr << "firstState\n";
        return secondState;
    };
    auto run2 = [thirdState]() {
        std::cerr << "secondState\n";
        return thirdState;
    };
    auto run3 = [thirdState]() {
        std::cerr << "thirdState\n";
        return thirdState;
    };

    firstState->setRunnable(run1);
    secondState->setRunnable(run2);
    thirdState->setRunnable(run3);

    StateMachine machine {firstState };
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
