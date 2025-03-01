#ifndef FMS_STATES_H
#define FMS_STATES_H

// Protect the enum definition with a guard to ensure it is only defined once
#ifndef FMSState
#define FMSState

enum FMSState {
    FMS_STATE_IDLE = 0,
    FMS_STATE_INIT = 1,
    FMS_STATE_ARMED = 2,
    FMS_STATE_MANUAL_FLIGHT = 3,
    FMS_STATE_AUTOMATIC_FLIGHT = 4,
    FMS_STATE_ABORT = 5
};

#endif // FMSState

#endif // FMS_STATES_H