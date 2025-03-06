#ifndef COM_THREAD_H
#define COM_THREAD_H

#include <cactus_rt/rt.h>
#include "shared_memory.h"
#include "DroneController.h"
#include "CM4UART.h"
#include "control_thread.h"

using cactus_rt::CyclicThread;
typedef unsigned char byte;

class ComThread : public CyclicThread
{
public:
    ComThread(SharedMemory<ControlInput> *control_input, SharedMemory<ControlOutput> *control_output);
    ~ComThread();

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();
    void ConfigureModules();
    std::string EncodeControlOutput(ControlOutput &control_data);
    void DecodeControlInput(const std::string &message);

    static SharedMemory<ControlInput> *control_input;
    static SharedMemory<ControlOutput> *control_output;

    CM4UART *uart_manager;

    // Manager manager;
    
    // struct ControlInputModules
    // {
    //     OneFloatModule d1;
    //     OneFloatModule d2;
    //     OneFloatModule thrust;
    //     OneFloatModule mz;
    // } control_input_modules;

    // struct ControlOutputModules
    // {
    //     ThreeFloatModule drone_attitude;
    //     ThreeFloatModule drone_rate;
    //     OneFloatModule drone_attitude_count; // Actually an int
    //     OneFloatModule drone_rate_count;     // Actually an int
        
    //     ThreeFloatModule remote_att_ref;
    //     OneFloatModule remote_inline_thrust;
    //     OneFloatModule remote_yaw_rate_ref;
    //     OneFloatModule remote_armed; // Actually a bool: 0 is false, 1 is true
    // } control_output_modules;

};

#endif // COM_THREAD_H