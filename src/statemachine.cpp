#include "statemachine.h"

#ifndef UNIT_TEST
#include <Arduino.h>
#else
extern "C" uint32_t millis();
#endif

uint16_t State::m_oCounter = 0;

State::State(const TValueFunction& p_run) :
    m_id(oCounter()),
    m_run(p_run) {
}
State::State() : State(nullptr) {
}

uint16_t State::id() const {
    return m_id;
}

uint16_t State::run(const uint32_t currentMillis) const {
    return m_run();
}

StateTimed::StateTimed(uint32_t p_forTime, const TValueFunction& p_run) :
    State(p_run),
    m_forTime(p_forTime),
    m_startTime(0) {
}

StateTimed::StateTimed(uint32_t p_forTime) : StateTimed(p_forTime, nullptr) {
}

void StateTimed::transitionStart(const uint32_t p_currentTime) const {
    m_startTime = p_currentTime;
}

uint16_t StateTimed::run(const uint32_t p_currentTime) const {
    if (p_currentTime - m_startTime > m_forTime) {
        // We reset the time in case we re-run this state again
        m_startTime = p_currentTime;
        return State::run(p_currentTime);
    }

    return UINT16_MAX;
}
