#include "statemachine.h"

#ifndef UNIT_TEST
#include <Arduino.h>
#else
extern "C" uint32_t millis();
#endif

uint16_t State::m_oCounter = 0;

State::State(TValueFunction p_run) :
    m_id(oCounter()),
    m_run(p_run) {
}

uint16_t State::id() const {
    return m_id;
}

uint8_t State::run(const uint32_t currentMillis) const {
    return m_run();
}


StateTimed::StateTimed(uint32_t p_forTime, TValueFunction p_run) :
    State(p_run),
    m_forTime(p_forTime),
    m_startTime(0) {
}

void StateTimed::transitionStart(const uint32_t p_currentTime) const {
    m_startTime = p_currentTime;
}

uint8_t StateTimed::run(const uint32_t p_currentTime) const {
    if (p_currentTime - m_startTime > m_forTime) {
        // We reset the time in case we re-run this state again
        m_startTime = p_currentTime;
        return State::run(p_currentTime);
    }

    return UINT8_MAX;
}



StateMachine::StateMachine(const std::vector<State*> p_states) :
    m_pos(0),
    m_states(p_states) {
}

StateMachine::~StateMachine() {
    for (State* s : m_states) {
        delete s;
    }
}


void StateMachine::start() const {
    m_states[0]->transitionStart(millis());
}

bool StateMachine::current(const State* state) const {
    return m_states[m_pos] == state;
}

void StateMachine::handle() {
    const uint32_t currentMillis = millis();
    uint8_t newState = m_states[m_pos]->run(currentMillis);

    // Test if we need to change state
    if (m_pos != newState && newState>=0 && newState<m_states.size()) {
        // When state isfound end the previous state and start the new state
        m_states[m_pos]->transitionEnd(currentMillis);
        m_pos = newState;
        m_states[m_pos]->transitionStart(currentMillis);
    }
}

