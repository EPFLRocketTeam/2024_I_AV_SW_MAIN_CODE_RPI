#ifndef LOG_INTERFACE_H
#define LOG_INTERFACE_H

#include <string>
#include <fstream>
#include "shared_memory.h"
#include "cyclic_thread_state_dependant.h"
#include "DroneController.h"
#include <cactus_rt/rt.h>

class LogInterface :  public CyclicThreadStateDependant
{
public:
    LogInterface(SharedMemory<ControlOutput> *control_memory,
                 SharedMemory<std::vector<double>> *current_state_memory,
                 SharedMemory<std::vector<double>> *waypoint_state_memory,
                 SharedMemory<std::vector<double>> *guidance_output_memory,
                 SharedMemory<FSMStates> *fsm_state_memory);
    
    ~LogInterface();



private:
    int delay = 1; //delay entre deux log en s 
    int period_ns = 1'000'000'000; // Run at 1 Hz.
    int CPU_core = 3; // de 1 à 4
    int priority = 70;
    
    LoopControl run(int64_t) noexcept;
    
    // Membres pour accéder aux différentes mémoires partagées
    SharedMemory<ControlOutput> *control_memory_;
    SharedMemory<std::vector<double>> *current_state_memory_;
    SharedMemory<std::vector<double>> *waypoint_state_memory_;
    SharedMemory<std::vector<double>> *guidance_output_memory_;

    // Fonction pour enregistrer les données dans le log
    void LogData();

    static cactus_rt::CyclicThreadConfig MakeConfig();
};

#endif // LOG_INTERFACE_H