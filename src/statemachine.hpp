#pragma once
#include <stdint.h>
#include <functional>
#include <vector>

#ifndef UNIT_TEST
#include <Arduino.h>
#else
extern "C" uint32_t millis();
#endif

/**
 * Simpel state that gets run each time the StateMachine reaches this state
 */
class State {
    friend class StateMachine;
public:
    typedef std::function<State* ()> TRunFunction;

private:
    TRunFunction m_run;

private:
    virtual void transitionStart(const uint32_t p_currentTime) const {}
    virtual void transitionEnd(const uint32_t p_currentTime) const {}

protected:
    virtual State* run(const uint32_t currentMillis) const;

public:
    State(const TRunFunction& p_run);
    State();
    virtual ~State() {
    }
    void setRunnable(const TRunFunction& p_run) {
        m_run = p_run;
    }
};

/**
 * State that will run after given time has passed
 */
class StateTimed : public State {
    const uint32_t m_forTime;
    mutable uint32_t m_startTime;

public:
    StateTimed(const uint32_t p_forTime, const TRunFunction& p_run);
    StateTimed(const uint32_t p_forTime);

private:
    virtual void transitionStart(const uint32_t p_currentTime) const;
    virtual State* run(const uint32_t p_currentTime) const;
};


/**
 * StateMachine itself that will run through all states
 */

class StateMachine {
private:
    State* m_currentState;
public:
    StateMachine(State* p_first);

    virtual ~StateMachine();

    // Call start once after you created the state machine but as aclose as possible
    // to your loop function
    void start() const;

    // Evaluates if the current state is the given state
    bool current(const State* state_id) const;

    // Call in your loop function regularly
    void handle();

};

class DeletingStateMachine :public StateMachine{
private:
    const std::vector<State*> m_states;
public:
    DeletingStateMachine(State* p_first, const std::vector<State*>& m_states);

    virtual ~DeletingStateMachine();
};
