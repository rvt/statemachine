[![Build Status](https://api.travis-ci.org/rvt/statemachine.svg?branch=master)](https://www.travis-ci.org/rvt/statemachine)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

# Small library to create a simple state machine in c++

This library provides a way to create a simple state machine

## Example

```cpp
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
```

An example can be find here [bbq-controller/src/main.cpp](https://github.com/rvt/bbq-controller/blob/master/src/main.cpp) where
it´s used to connect to wifi and Mosquitto. It will detected if the connection drops and re-connects

## License

This code is released under the MIT License.

## V 1.0.1
* Each state should return a state number instead of this

## V 1.0.2
* Allow to reference state numbers instead of order added
* Allow to create the state prior to the runnable code