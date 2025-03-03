#ifndef FSM_STATES_H
#define FSM_STATES_H

#pragma once

#include <unordered_map>
#include <string>

enum class FSMStates {
    IDLE,
    INIT,
    ARMED,
    MANUAL_FLIGHT,
    AUTOMATIC_FLIGHT,
    ABORT
};

extern std::unordered_map<FSMStates, std::string> FSMStatesToString; // extern ensures that FSMStatesToString is defined only once

#endif // FSM_STATES_H