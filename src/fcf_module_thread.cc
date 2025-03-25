#include "fcf_module_thread.h"
#include "fsm_states.h"
#include <cactus_rt/rt.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
using cactus_rt::CyclicThread;

FCFThread::FCFThread(SharedMemory<ControlOutput> *control_memory, SharedMemory<FSMStates> *fsm_memory, const string& filename)
    : CyclicThread("FCFThread", MakeConfig()), control_memory(control_memory), fsm_memory(fsm_memory), filename(filename) {
    lecture();
}

// Gestion de la boucle d'exécution
CyclicThread::LoopControl FCFThread::Loop(int64_t elapsed_ns) noexcept {
    FSMStates current_state = fsm_memory->Read(); // Lire l'état actuel de la mémoire partagée
    
    switch (current_state) {
        case FSMStates::MANUAL_FLIGHT:
            gestionManualFlight();
            break;
        case FSMStates::AUTOMATIC_FLIGHT:
            gestionAutomaticFlight();
            break;
        default:
            break;
    }

    affichage();
    return LoopControl::Continue;
}

// Configuration du thread
cactus_rt::CyclicThreadConfig FCFThread::MakeConfig() {
    cactus_rt::CyclicThreadConfig config;
    config.period_ns = 10'000'000; // 100 Hz
    config.SetFifoScheduler(90);
    return config;
}

// Lecture du fichier JSON
bool FCFThread::lecture() {
    ifstream fichier(filename);
    if (!fichier) {
        cerr << "Erreur : impossible d'ouvrir le fichier " << filename << endl;
        return false;
    }
    
    json data;
    fichier >> data;
    
    try {
        position = data.at("position").get<vector<double>>();
        vitesse = data.at("vitesse").get<vector<double>>();
        thrust = data.at("thrust").get<double>();
        pitch = data.at("pitch").get<double>();
        yaw = data.at("yaw").get<double>();
    } catch (const json::exception& e) {
        cerr << "Erreur lors de la lecture des données JSON : " << e.what() << endl;
        return false;
    }
    
    return true;
}

// Gestion du mode MANUAL_FLIGHT
void FCFThread::gestionManualFlight() {
    cout << "[MANUAL_FLIGHT] Contrôle manuel actif." << endl;
    // Ajouter ici les traitements spécifiques au mode manuel
}

// Gestion du mode AUTOMATIC_FLIGHT
void FCFThread::gestionAutomaticFlight() {
    cout << "[AUTOMATIC_FLIGHT] Mode automatique actif." << endl;
    calcul(); // Appelle la fonction de calcul automatique
}

// Gestion du temps
double FCFThread::temps() {
    time += 0.01;
    return time;
}


