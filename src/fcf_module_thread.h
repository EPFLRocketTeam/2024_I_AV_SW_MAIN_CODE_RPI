#ifndef FCF_MODULE_THREAD_H
#define FCF_MODULE_THREAD_H

#include <cactus_rt/rt.h>
#include "shared_memory.h"
#include "fsm_states.h"
#include <vector>
#include <string>

using cactus_rt::CyclicThread;

class FCFThread : public CyclicThread
{
public:
    FCFThread(SharedMemory<FSMStates> *control_memory, 
              SharedMemory<FSMStates> *fsm_memory, 
              SharedMemory<std::vector<double>> *current_state_memory, 
              SharedMemory<std::vector<double>> *guidance_waypoint_output_memory, 
              const std::string &filename);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();

    // Pointeurs vers la mémoire partagée
    SharedMemory<FSMStates> *control_memory;
    SharedMemory<FSMStates> *fsm_memory;
    SharedMemory<std::vector<double>> *current_state_memory;
    SharedMemory<std::vector<double>> *guidance_waypoint_output_memory;

    // Fichier JSON contenant les points de vol
    std::string filename;

    // Variables pour la gestion des points et du temps
    double timer;  
    size_t point;  
    std::vector<std::vector<double>> position; 
    std::vector<std::vector<double>> speed;
    std::vector<double> thrust;
    std::vector<double> inclinaison;
    std::vector<double> guidance_mode;
    std::vector<double> times;

    // Méthodes
    bool lecturePoints();
    void initialiserTimer();
    void verifierTemps();
    void updateGuidanceWaypoint();
};

#endif // FCF_MODULE_THREAD_H
