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
    FCFThread(SharedMemory<FSMStates> *control_memory, SharedMemory<FSMStates> *fsm_memory, const std::string &filename);

protected:
    LoopControl Loop(int64_t elapsed_ns) noexcept final;

private:
    static cactus_rt::CyclicThreadConfig MakeConfig();

    // Pointeurs vers la mémoire partagée
    SharedMemory<FSMStates> *control_memory;
    SharedMemory<FSMStates> *fsm_memory;

    // Fichier JSON contenant les points de vol
    std::string filename;

    // Variables pour la gestion des points et du temps
    double timer;  // Chronomètre (précision 0.01s)
    size_t i;      // Index du temps attendu dans `times`
    std::vector<std::vector<double>> points; // Liste des points 3D
    std::vector<double> times; // Liste des temps associés

    // Méthodes
    bool lecturePoints();      // Chargement des points depuis JSON en INIT
    void initialiserTimer();   // Réinitialisation du timer en IDLE
    void verifierTemps();      // Vérification du timer en AUTOMATIC_FLIGHT
};

#endif // FCF_MODULE_THREAD_H
