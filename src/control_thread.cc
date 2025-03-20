#include "control_thread.h"
#include "DroneController.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>

using cactus_rt::CyclicThread;
#define TUNING_FILE_NAME "drone_config.json"

ControlThread::ControlThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output)
    : CyclicThread("ControlThread", MakeConfig()), control_input(control_input), control_output(control_output)
{
    controller = std::make_unique<Controller>(ControllerFromFile(TUNING_FILE_NAME));
    controller->reset();
}

CyclicThread::LoopControl ControlThread::Loop(int64_t elapsed_ns) noexcept
{
    ControlInput input = control_input->Read();
    ControlOutput output = controller->ExhaustiveControl(input.desired_state, input.current_state, input.setpointSelection, input.inline_thrust);
    control_output->Write(output);

    return LoopControl::Continue;
}

cactus_rt::CyclicThreadConfig ControlThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    // Run at 100 Hz.
    config.period_ns = 10'000'000;

    // Pin this thread on CPU core #3
    // config.cpu_affinity = std::vector<size_t>{3};

    // Run the thread with SCHED_FIFO at real-time priority of 90.
    config.SetFifoScheduler(90);
    return config;
}

Controller ControlThread::ControllerFromFile(const std::string &file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open configuration file: " + file_path);
    }

    try
    {
        json config;
        file >> config;
        return ControllerFromJSON(config);
    }
    catch (const json::exception &e)
    {
        throw std::runtime_error(std::string("JSON parsing error: ") + e.what());
    }
}

Controller ControlThread::ControllerFromJSON(const json &doc)
{
    Vec3 P = {0, 0, 0};
    Vec3 I = {0, 0, 0};
    Vec3 D = {0, 0, 0};

    // Position control parameters
    const auto &position = doc["position"];
    const auto &position_p = position["p"];
    const auto &position_i = position["i"];
    const auto &position_d = position["d"];

    // Velocity control parameters
    const auto &velocity = doc["velocity"];
    const auto &velocity_p = velocity["p"];
    const auto &velocity_i = velocity["i"];
    const auto &velocity_d = velocity["d"];

    // Attitude control parameters
    const auto &attitude = doc["attitude"];
    const auto &attitude_p = attitude["p"];
    const auto &attitude_i = attitude["i"];
    const auto &attitude_d = attitude["d"];

    // Rate control parameters
    const auto &rate = doc["rate"];
    const auto &rate_p = rate["p"];
    const auto &rate_i = rate["i"];
    const auto &rate_d = rate["d"];

    // Build position controller
    for (int i = 0; i < 3; i++)
    {
        P[i] = position_p[i].get<double>();
        I[i] = position_i[i].get<double>();
        D[i] = position_d[i].get<double>();
    }
    const PID posControl = {P, I, D, Controller::POS_DT, SPEED_LIMIT};

    // Build velocity controller
    for (int i = 0; i < 3; i++)
    {
        P[i] = velocity_p[i].get<double>();
        I[i] = velocity_i[i].get<double>();
        D[i] = velocity_d[i].get<double>();
    }
    const PID velControl = {P, I, D, Controller::VEL_DT, ACC_LIMIT};

    // Build attitude controller
    for (int i = 0; i < 3; i++)
    {
        P[i] = attitude_p[i].get<double>();
        I[i] = attitude_i[i].get<double>();
        D[i] = attitude_d[i].get<double>();
    }
    const PID attControl = {P, I, D, Controller::ATT_DT, ANG_RATE_LIMIT};

    // Build rate controller
    for (int i = 0; i < 3; i++)
    {
        P[i] = rate_p[i].get<double>();
        I[i] = rate_i[i].get<double>();
        D[i] = rate_d[i].get<double>();
    }
    const PID rateControl = {P, I, D, Controller::RATE_DT, {TORQUE_LIMIT_XY, TORQUE_LIMIT_XY, TORQUE_LIMIT_Z}, {-0.0, 0.0, 0.}};

    return {rateControl, attControl, velControl, posControl,
            M, R, GIMBAL_LIMIT, GIMBAL_RATE_LIMIT,
            MAX_THRUST, THRUST_RATE_LIMIT};
}

std::ostream& operator<<(std::ostream& os, const ControlInput& input) {
    os << "{ desired_state: " << input.desired_state 
       << ", current_state: " << input.current_state
       << ", setpoint: " << input.setpointSelection
       << ", inline_thrust: " << input.inline_thrust << " }";
    return os;
}
