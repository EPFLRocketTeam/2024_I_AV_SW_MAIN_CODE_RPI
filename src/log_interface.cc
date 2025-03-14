
#include "log_interface.h"
#include <cactus_rt/rt.h>
#include <iostream>

using cactus_rt::CyclicThread;

LogInterface::LogInterface(SharedMemory<ControlOutput> *control_memory,
                 SharedMemory<std::vector<double>> *current_state_memory,
                 SharedMemory<std::vector<double>> *waypoint_state_memory,
                 SharedMemory<std::vector<double>> *guidance_output_memory,
                 SharedMemory<FSMStates> *fsm_state_memory)
    : CyclicThreadStateDependant(fsm_state_memory, "LogInterface", MakeConfig()),control_memory_(control_memory),
                                                                                    current_state_memory_(current_state_memory), 
                                                                                    waypoint_state_memory_(waypoint_state_memory), 
                                                                                    guidance_output_memory_(guidance_output_memory)

{

}
CyclicThread::LoopControl LogInterface::run(int64_t elapsed_ns) noexcept
{
    LogData();
    std::this_thread::sleep_for(std::chrono::seconds(delay)); // Attend delay seconde
    return LoopControl::Continue;
}

LogInterface::~LogInterface() 
{
}

void LogInterface::LogData() 
{
    // Récupère les données des mémoires partagées
    ControlOutput control_output = control_memory_->Read();
    std::vector<double> current_state = current_state_memory_->Read();
    std::vector<double> waypoint_state = waypoint_state_memory_->Read();
    std::vector<double> guidance_output = guidance_output_memory_->Read();
    FSMStates fsm_state = fsm_state_memory_->Read();

    // Utilise LOG_INFO pour enregistrer les données dans le fichier de log
    LOG_INFO(Logger(), "FSM State: {}", static_cast<int>(fsm_state));
    LOG_INFO(Logger(), "Control Output: [{}, {}, {}, {}]", control_output.d1, control_output.d2, control_output.thrust, control_output.mz);

    // Log les vecteurs sous forme de chaîne de caractères
    std::string current_state_str = "[";
    for (size_t i = 0; i < current_state.size(); ++i) {
        current_state_str += std::to_string(current_state[i]);
        if (i != current_state.size() - 1) current_state_str += ", ";
    }
    current_state_str += "]";
    LOG_INFO(Logger(), "Current State: {}", current_state_str);

    std::string waypoint_state_str = "[";
    for (size_t i = 0; i < waypoint_state.size(); ++i) {
        waypoint_state_str += std::to_string(waypoint_state[i]);
        if (i != waypoint_state.size() - 1) waypoint_state_str += ", ";
    }
    waypoint_state_str += "]";
    LOG_INFO(Logger(), "Waypoint State: {}", waypoint_state_str);

    std::string guidance_output_str = "[";
    for (size_t i = 0; i < guidance_output.size(); ++i) {
        guidance_output_str += std::to_string(guidance_output[i]);
        if (i != guidance_output.size() - 1) guidance_output_str += ", ";
    }
    guidance_output_str += "]";
    LOG_INFO(Logger(), "Guidance Output: {}", guidance_output_str);

    LOG_INFO(Logger(), "--------------------------------------");
} 

cactus_rt::CyclicThreadConfig GuidanceThread::MakeConfig()
{
    cactus_rt::CyclicThreadConfig config;

    config.period_ns = period_ns;

    config.cpu_affinity = std::vector<size_t>{CPU_core};

    // Run the thread with SCHED_FIFO at real-time priority of 80.
    config.SetFifoScheduler(priority); // 70
    return config;
}