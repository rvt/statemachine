#include "statemachine.h"

#ifndef UNIT_TEST
#include <Arduino.h>
#else
extern "C" uint32_t millis();
#endif

State::State(const TRunFunction& p_run) :
    m_run(p_run) {
}
State::State() : State(nullptr) {
}

State* State::run(const uint32_t currentMillis) const {
    return m_run();
}

StateTimed::StateTimed(uint32_t p_forTime, const TRunFunction& p_run) :
    State(p_run),
    m_forTime(p_forTime),
    m_startTime(0) {
}

StateTimed::StateTimed(uint32_t p_forTime) : StateTimed(p_forTime, nullptr) {
}

void StateTimed::transitionStart(const uint32_t p_currentTime) const {
    m_startTime = p_currentTime;
}

State* StateTimed::run(const uint32_t p_currentTime) const {
    if (p_currentTime - m_startTime > m_forTime) {
        // We reset the time in case we re-run this state again
        m_startTime = p_currentTime;
        return State::run(p_currentTime);
    }

    return (State*)this;
}

StateMachine::StateMachine(State* p_first) :
    m_currentState(p_first) {
}

StateMachine::~StateMachine() {
}

// Call start once after you created the state machine but as aclose as possible
// to your loop function
void StateMachine::start() const {
    m_currentState->transitionStart(millis());
}

// Evaluates if the current state is the given state
bool StateMachine::current(const State* state_id) const {
    return m_currentState == state_id;
}

// Call in your loop function regularly
void StateMachine::handle() {
    const uint32_t currentMillis = millis();
    State* newState = m_currentState->run(currentMillis);

    // Test if we need to change state
    if (m_currentState != newState) {
        // When state isfound end the previous state and start the new state
        m_currentState->transitionEnd(currentMillis);
        m_currentState = newState;
        m_currentState->transitionStart(currentMillis);
    }
}

DeletingStateMachine::DeletingStateMachine(State* p_first, const std::vector<State*>& p_states) : 
  StateMachine(p_first),
  m_states(p_states) {
    
}

DeletingStateMachine::~DeletingStateMachine() {
    for (State* s : m_states) {
        delete s;
    }
}