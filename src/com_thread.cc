#include "com_thread.h"
#include "DroneController.h"
#include "control_thread.h"
#include "shared_memory.h"
#include <cactus_rt/rt.h>
#include <fcntl.h>   // File control
#include <termios.h> // Terminal I/O

constexpr int BAUDRATE = B115200;
constexpr const char *DEVICE = "/dev/serial0";

using cactus_rt::CyclicThread;

// TODO: Stop using exceptions everywhere
// TODO: Fix segmentation fault in loopback test when reconnecting
// => Remote debugger needed

ComThread::ComThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output)
    : CyclicThread("ComThread", MakeConfig())
{
    ComThread::control_input = control_input;
    ComThread::control_output = control_output;

    uart_manager = new CM4UART(BAUDRATE, DEVICE, Logger());

    // uart_manager->RegisterHandler((int)PacketId::ControlInput, [this](Payload &payload)
    //                               { ReceiveControlInput(payload); });
    uart_manager->RegisterHandler((int)PacketId::ControlOutput, [this](Payload &payload)
                                  { ReceiveControlOutput(payload); });
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

    ControlOutput output = control_output->Read();
    LOG_INFO(Logger(), "Sending control output: d1={}, d2={}, thrust={}, mz={}", output.d1, output.d2, output.thrust, output.mz);

    SendControlOutput(output);

    uart_manager->SendUARTPackets();

    return LoopControl::Continue;
}

void ComThread::SendControlOutput(const ControlOutput &output)
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
        return;
    }

    uart_manager->SendUARTPacket((int)PacketId::ControlOutput, payload);
}

void ComThread::ReceiveControlOutput(Payload &payload)
{
    ControlOutput output;
    bool success = true;

    success &= payload.ReadDouble(output.d1);
    success &= payload.ReadDouble(output.d2);
    success &= payload.ReadDouble(output.thrust);
    success &= payload.ReadDouble(output.mz);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to read control output from payload");
        return;
    }

    LOG_INFO(Logger(), "Received control output: d1={}, d2={}, thrust={}, mz={}", output.d1, output.d2, output.thrust, output.mz);

    control_output->Write(output);
}

// void ComThread::ReceiveControlInput(Payload &payload)
// {
//     DroneState state;
//     state.attitude = ReadVec3(payload);
//     state.rate = ReadVec3(payload);
//     state.attitude_count = payload.ReadInt();
//     state.rate_count = payload.ReadInt();

//     AttRemoteInput remote_input;
//     remote_input.att_ref = ReadVec3(payload);
//     remote_input.inline_thrust = payload.ReadDouble();
//     remote_input.yaw_rate_ref = payload.ReadDouble();
//     remote_input.arm = payload.ReadBool();

//     ControlInput input;
//     input.state = state;
//     input.remote_input = remote_input;
//     control_input->Write(input);
// }

// void ComThread::WriteVec3(Vec3 vec, Payload &payload)
// {
//     payload.WriteDouble(vec.x);
//     payload.WriteDouble(vec.y);
//     payload.WriteDouble(vec.z);
// }

// Vec3 ComThread::ReadVec3(Payload &payload)
// {
//     double x = payload.ReadDouble();
//     double y = payload.ReadDouble();
//     double z = payload.ReadDouble();
//     return Vec3(x, y, z);
// }
