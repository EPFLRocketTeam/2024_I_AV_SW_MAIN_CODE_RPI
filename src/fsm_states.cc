#include "fsm_states.h"

std::unordered_map<FSMStates, std::string> FSMStatesToString = 
{    
    {FSMStates::IDLE, "IDLE"},
    {FSMStates::INIT, "INIT"},
    {FSMStates::ARMED, "ARMED"},
    {FSMStates::MANUAL_FLIGHT, "MANUAL_FLIGHT"},
    {FSMStates::AUTOMATIC_FLIGHT, "AUTOMATIC_FLIGHT"},
    {FSMStates::ABORT, "ABORT"}
};