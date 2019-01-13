[![Build Status](https://api.travis-ci.org/rvt/statemachine.svg?branch=master)](https://www.travis-ci.org/rvt/statemachine)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)


# Small library to create a simple state machine in c++

This library provides a way to create a simple state machine

## Example

```
TEST_CASE( "Should correctly get current value with simple state machine", "[statemachine]" ) {
    State* thirdState;
    State* secondState;
    State* firstState;
    thirdState = new State([&thirdState](){std::cerr << "thirdState\n";return thirdState;});
    secondState = new State([&thirdState](){std::cerr << "secondState\n";return thirdState;});
    firstState = new State([&secondState](){std::cerr << "firstState\n";return secondState;});

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
```

An example can be find here [bbq-controller/src/main.cpp](https://github.com/rvt/bbq-controller/blob/master/src/main.cpp) where
it´s used to connect to wifi and Mosquitto. It will detected if the connection drops and re-connects


## License

This code is released under the MIT License.