#ifndef FSM_STATES_H
#define FSM_STATES_H

#pragma once

#include <unordered_map>
#include <string>
#include "Payload.h"

enum class FSMStates {
    INIT,
    IDLE,
    ARMED,
    MANUAL_FLIGHT,
    AUTOMATIC_FLIGHT,
    FORCED_LANDING,
    ABORT
};

std::unordered_map<FSMStates, std::string> FSMStatesToString = 
{    
    {FSMStates::IDLE, "IDLE"},
    {FSMStates::INIT, "INIT"},
    {FSMStates::ARMED, "ARMED"},
    {FSMStates::MANUAL_FLIGHT, "MANUAL_FLIGHT"},
    {FSMStates::AUTOMATIC_FLIGHT, "AUTOMATIC_FLIGHT"},
    {FSMStates::FORCED_LANDING, "FORCED_LANDING"},
    {FSMStates::ABORT, "ABORT"}
};

void serializeFSMState(FSMStates state, Payload &payload) {
    int state_int = static_cast<int>(state);
    payload.write(state_int);
}

void deserializeFSMState(FSMStates &state, Payload &payload) {
    int state_int;
    payload.read(state_int);
    state = static_cast<FSMStates>(state_int);
}

#endif // FSM_STATES_H