#include "com_thread.h"
#include "DroneController.h"
#include "control_thread.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

constexpr int BAUDRATE = B115200;
constexpr const char *DEVICE = "/dev/serial0";
constexpr bool DEBUG = true;

using cactus_rt::CyclicThread;

// TODO:
// - Loopback tests, Teensy et RPI
// - Sending and receiving control input/output
// - Adapt control code Teensy side
// - Drone flight!
// - Cleanup:
//  - Initialization in begin() instead of constructor
//  - Uniform error handling using bool
//  - Fix tests
//  - Find source of segfault using debugger (Or is it just due to unhandled exceptions?)
//    => Remote debugger needed
//  - Write more tests

ComThread::ComThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output)
    : CyclicThread("ComThread", MakeConfig())
{
    // Could use a initializer list instead
    ComThread::control_input = control_input;
    ComThread::control_output = control_output;

    uart_manager = new CM4UART(BAUDRATE, DEVICE, Logger());
    uart_manager->RegisterHandler((int)PacketId::ControlInput, [this](Payload &payload)
                                  { ReceiveControlInput(payload); });
    // uart_manager->RegisterHandler((int)PacketId::ControlOutput, [this](Payload &payload)
    //                               { ReceiveControlOutput(payload); });

    bool success = uart_manager->Begin();
    if (!success)
    {
        throw std::runtime_error("Failed to initialize UART");
    }
}

cactus_rt::CyclicThreadConfig ComThread::MakeConfig()
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

ComThread::~ComThread()
{
    delete uart_manager;
}

CyclicThread::LoopControl ComThread::Loop(int64_t elapsed_ns) noexcept
{
    uart_manager->ReceiveUARTPackets();

    // ControlInput input = control_input->Read();
    // SendControlInput(input);

    ControlOutput output;
    if (DEBUG)
    {
        output.d1 = 0.1;
        output.d2 = 0.2;
        output.thrust = 0.3;
        output.mz = 0.4;
    }
    else
    {
        output = control_output->Read();
    }

    SendControlOutput(output);
    uart_manager->SendUARTPackets();

    return LoopControl::Continue;
}

bool ComThread::SendControlOutput(const ControlOutput &output)
{
    Payload payload;
    bool success = true;
    success &= payload.WriteDouble(output.d1);
    success &= payload.WriteDouble(output.d2);
    success &= payload.WriteDouble(output.thrust);
    success &= payload.WriteDouble(output.mz);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to write control output to payload");
        return false;
    }

    success = uart_manager->SendUARTPacket((int)PacketId::ControlOutput, payload);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to send control output");
        return false;
    }

    LOG_INFO(Logger(), "Sent control output: d1={}, d2={}, thrust={}, mz={}", output.d1, output.d2, output.thrust, output.mz);

    return true;
}

// void ComThread::ReceiveControlOutput(Payload &payload)
// {
//     ControlOutput output;
//     bool success = true;

//     success &= payload.ReadDouble(output.d1);
//     success &= payload.ReadDouble(output.d2);
//     success &= payload.ReadDouble(output.thrust);
//     success &= payload.ReadDouble(output.mz);

//     if (!success)
//     {
//         LOG_ERROR(Logger(), "Failed to read control output from payload");
//         return;
//     }

//     LOG_INFO(Logger(), "Received control output: d1={}, d2={}, thrust={}, mz={}", output.d1, output.d2, output.thrust, output.mz);

//     // control_output->Write(output);
// }

// bool ComThread::SendControlInput(const ControlInput &input)
// {
//     Payload payload;
//     bool success = true;

//     // Write state
//     success &= WriteVec3(payload, input.state.attitude);
//     success &= WriteVec3(payload, input.state.rate);
//     success &= payload.WriteInt(input.state.attitude_count);
//     success &= payload.WriteInt(input.state.rate_count);

//     // Write remote input
//     success &= WriteVec3(payload, input.remote_input.att_ref);
//     success &= payload.WriteDouble(input.remote_input.inline_thrust);
//     success &= payload.WriteDouble(input.remote_input.yaw_rate_ref);
//     success &= payload.WriteBool(input.remote_input.arm);

//     if (!success)
//     {
//         LOG_ERROR(Logger(), "Failed to write control input to payload");
//         return false;
//     }

//     success = uart_manager->SendUARTPacket((int)PacketId::ControlInput, payload);

//     if (!success)
//     {
//         LOG_ERROR(Logger(), "Failed to send control input");
//         return false;
//     }

//     LOG_INFO(Logger(), "Sent control input:  att_count={}, rate_count={}, inline_thrust={}, yaw_rate_ref={}, arm={}",
//              input.state.attitude_count, input.state.rate_count,
//              input.remote_input.inline_thrust, input.remote_input.yaw_rate_ref, input.remote_input.arm);

//     return true;
// }

void ComThread::ReceiveControlInput(Payload &payload)
{
    bool success = true;
    ControlInput input;

    // Read state
    success &= ReadVec3(payload, input.state.attitude);
    success &= ReadVec3(payload, input.state.rate);
    success &= payload.ReadInt(input.state.attitude_count);
    success &= payload.ReadInt(input.state.rate_count);

    // Read remote input
    success &= ReadVec3(payload, input.remote_input.att_ref);
    success &= payload.ReadDouble(input.remote_input.inline_thrust);
    success &= payload.ReadDouble(input.remote_input.yaw_rate_ref);
    success &= payload.ReadBool(input.remote_input.arm);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to read control input from payload");
        return;
    }

    LOG_INFO(Logger(), "Received control input:  att_count={}, rate_count={}, inline_thrust={}, yaw_rate_ref={}, arm={}",
             input.state.attitude_count, input.state.rate_count,
             input.remote_input.inline_thrust, input.remote_input.yaw_rate_ref, input.remote_input.arm);

    if (!DEBUG)
    {
        control_input->Write(input);
    }
}

bool ComThread::WriteVec3(Payload &payload, Vec3 vec)
{
    bool success = true;
    success &= payload.WriteDouble(vec.x);
    success &= payload.WriteDouble(vec.y);
    success &= payload.WriteDouble(vec.z);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to write Vec3 to payload");
        return false;
    }

    return true;
}

bool ComThread::ReadVec3(Payload &payload, Vec3 &vec)
{
    double x, y, z;
    bool success = true;
    success &= payload.ReadDouble(x);
    success &= payload.ReadDouble(y);
    success &= payload.ReadDouble(z);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to read Vec3 from payload");
        return false;
    }

    vec = Vec3(x, y, z);
    return true;
}