#ifndef FSM_STATES_H
#define FSM_STATES_H

enum class FSMStates {
    IDLE,
    INIT,
    ARMED,
    MANUAL_FLIGHT,
    AUTOMATIC_FLIGHT,
    ABORT
};

#endif // FSM_STATES_H