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

ComThread::ComThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output)
    : CyclicThread("ComThread", MakeConfig()), control_input(control_input), control_output(control_output)
{
    uart_manager = new CM4UART(BAUDRATE, DEVICE, Logger());
    uart_manager->RegisterHandler((int)PacketId::ControlInput, [this](Payload &payload)
                                  { ReceiveControlInput(payload); });

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

    ControlOutput output;
    output = control_output->Read();

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

void ComThread::ReceiveControlInput(Payload &payload)
{
    bool success = true;
    ControlInput input;

    // Read state
    success &= ReadState(payload, input.desired_state);
    success &= ReadState(payload, input.current_state);
    success &= ReadSetpointSelection(payload, input.setpointSelection);
    success &= payload.ReadDouble(input.inline_thrust);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to read control input from payload");
        return;
    }

    LOG_INFO(Logger(), "Received control input: thrust={}, rest_of_the_payload=TODO", input.inline_thrust);

    control_input->Write(input);
}

bool ComThread::WriteVec3(Payload &payload, const Vec3 &vec)
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

bool ComThread::ReadState(Payload &payload, State &state)
{
    bool success = true;
    success &= ReadVec3(payload, state.pos);
    success &= ReadVec3(payload, state.vel);
    success &= ReadVec3(payload, state.att);
    success &= ReadVec3(payload, state.rate);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to read state from payload");
        return false;
    }

    return true;
}

bool ComThread::WriteState(Payload &payload, const State &state)
{
    bool success = true;
    success &= WriteVec3(payload, state.pos);
    success &= WriteVec3(payload, state.vel);
    success &= WriteVec3(payload, state.att);
    success &= WriteVec3(payload, state.rate);

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to write state to payload");
        return false;
    }

    return true;
}

bool ComThread::WriteSetpointSelection(Payload &payload, const SetpointSelection &setpoint)
{
    // Pack all 12 boolean values into 2 bytes to minimize payload size
    uint8_t buffer[2] = {0, 0};

    // Pack posSPActive (3 bits)
    if (setpoint.posSPActive[0])
        buffer[0] |= (1 << 0);
    if (setpoint.posSPActive[1])
        buffer[0] |= (1 << 1);
    if (setpoint.posSPActive[2])
        buffer[0] |= (1 << 2);

    // Pack velSPActive (3 bits)
    if (setpoint.velSPActive[0])
        buffer[0] |= (1 << 3);
    if (setpoint.velSPActive[1])
        buffer[0] |= (1 << 4);
    if (setpoint.velSPActive[2])
        buffer[0] |= (1 << 5);

    // Pack attSPActive (3 bits across byte boundary)
    if (setpoint.attSPActive[0])
        buffer[0] |= (1 << 6);
    if (setpoint.attSPActive[1])
        buffer[0] |= (1 << 7);
    if (setpoint.attSPActive[2])
        buffer[1] |= (1 << 0);

    // Pack rateSPActive (3 bits)
    if (setpoint.rateSPActive[0])
        buffer[1] |= (1 << 1);
    if (setpoint.rateSPActive[1])
        buffer[1] |= (1 << 2);
    if (setpoint.rateSPActive[2])
        buffer[1] |= (1 << 3);

    // Write the packed bytes to the payload
    bool success = payload.WriteBytes(buffer, sizeof(buffer));

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to write SetpointSelection to payload");
        return false;
    }

    return true;
}

bool ComThread::ReadSetpointSelection(Payload &payload, SetpointSelection &setpoint)
{
    // Read 2 bytes from the payload
    uint8_t buffer[2];
    bool success = payload.ReadBytes(buffer, sizeof(buffer));

    if (!success)
    {
        LOG_ERROR(Logger(), "Failed to read SetpointSelection from payload");
        return false;
    }

    // Unpack posSPActive
    setpoint.posSPActive[0] = (buffer[0] & (1 << 0)) != 0;
    setpoint.posSPActive[1] = (buffer[0] & (1 << 1)) != 0;
    setpoint.posSPActive[2] = (buffer[0] & (1 << 2)) != 0;

    // Unpack velSPActive
    setpoint.velSPActive[0] = (buffer[0] & (1 << 3)) != 0;
    setpoint.velSPActive[1] = (buffer[0] & (1 << 4)) != 0;
    setpoint.velSPActive[2] = (buffer[0] & (1 << 5)) != 0;

    // Unpack attSPActive
    setpoint.attSPActive[0] = (buffer[0] & (1 << 6)) != 0;
    setpoint.attSPActive[1] = (buffer[0] & (1 << 7)) != 0;
    setpoint.attSPActive[2] = (buffer[1] & (1 << 0)) != 0;

    // Unpack rateSPActive
    setpoint.rateSPActive[0] = (buffer[1] & (1 << 1)) != 0;
    setpoint.rateSPActive[1] = (buffer[1] & (1 << 2)) != 0;
    setpoint.rateSPActive[2] = (buffer[1] & (1 << 3)) != 0;

    return true;
}