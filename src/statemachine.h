#pragma once
#include <stdint.h>
#include <functional>
#include <array>

#ifndef UNIT_TEST
#include <Arduino.h>
#else
extern "C" uint32_t millis();
#endif

template<std::size_t desiredCapacity> class StateMachine;

/**
 * Simpel state that gets run each time the StateMachine reaches this state
 */
class State {
     template<std::size_t desiredCapacity> friend class StateMachine;
public:
    typedef std::function<uint16_t (void)> TValueFunction;

private:
    static uint16_t m_oCounter;
    uint16_t oCounter() {
        return m_oCounter++;
    }
    const uint16_t m_id;
    TValueFunction m_run;

private:
    virtual void transitionStart(const uint32_t p_currentTime) const {}
    virtual void transitionEnd(const uint32_t p_currentTime) const {}

protected:
    virtual uint16_t run(const uint32_t currentMillis) const;

public:
    State(const TValueFunction& p_run);
    State();
    virtual ~State() {
    }
    uint16_t id() const;
    void setRunnable(const TValueFunction& p_run) {
        m_run = p_run;
    }
    inline bool operator==(const State& other) const {
        return other.m_id == m_id;
    }
    inline bool operator!=(const State& other) const {
        return !(*this == other);
    }
};

/**
 * State that will run after given time has passed
 */
class StateTimed : public State {
    const uint32_t m_forTime;
    mutable uint32_t m_startTime;

public:
    StateTimed(const uint32_t p_forTime, const TValueFunction& p_run);
    StateTimed(const uint32_t p_forTime);

private:
    virtual void transitionStart(const uint32_t p_currentTime) const;
    virtual uint16_t run(const uint32_t p_currentTime) const;
};


/**
 * StateMachine itself that will run through all states
 */

template<std::size_t desiredCapacity>
class StateMachine {
private:
    size_t m_currentArrayPos;
    const std::array<State*, desiredCapacity> m_states;
public:
    StateMachine(const std::array<State*, desiredCapacity>& p_states) :
        m_currentArrayPos(0),
        m_states(p_states) {
    }

    virtual ~StateMachine() {
        for (State* s : m_states) {
            delete s;
        }
    }

    // Call start once after you created the state machine but as aclose as possible
    // to your loop function
    void start() const {
        m_states[0]->transitionStart(millis());
    }

    // Evaluates if the current state is the given state
    bool current(const uint16_t state_id) const {
        return m_states[m_currentArrayPos]->id() == state_id;
    }

    uint8_t getStatePosForId(uint16_t id) const {
        for (uint8_t i = 0; i < desiredCapacity; i++) {
            if (m_states[i]->id() == id) {
                return i;
            }
        }

        return 0;
    }

    // Call in your loop function regularly
    void handle() {
        const uint32_t currentMillis = millis();
        uint16_t newStateId = m_states[m_currentArrayPos]->run(currentMillis);

        // when UINT16_MAX is returned, we stay on the same state
        if (newStateId == UINT16_MAX) {
            return;
        }

        uint8_t newArrayPos = getStatePosForId(newStateId);

        // Test if we need to change state
        if (m_currentArrayPos != newArrayPos) {
            // When state isfound end the previous state and start the new state
            m_states[m_currentArrayPos]->transitionEnd(currentMillis);
            m_currentArrayPos = newArrayPos;
            m_states[m_currentArrayPos]->transitionStart(currentMillis);
        }
    }

};

