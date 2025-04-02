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

FCFThread::FCFThread(SharedMemory<FSMStates> *control_memory, 
                     SharedMemory<FSMStates> *fsm_memory, 
                     SharedMemory<std::vector<double>> *current_state_memory, 
                     SharedMemory<std::vector<double>> *guidance_waypoint_output_memory, 
                     const string& filename)
    : CyclicThread("FCFThread", MakeConfig()), 
      control_memory(control_memory), 
      fsm_memory(fsm_memory), 
      current_state_memory(current_state_memory), 
      guidance_waypoint_output_memory(guidance_waypoint_output_memory), 
      filename(filename), 
      timer(0), 
      point(0) { // Initialisation de point ici
    position.clear();
    speed.clear();
    thrust.clear();
    inclinaison.clear();
    guidance_mode.clear();

}

// Gestion de la boucle d'exécution
CyclicThread::LoopControl FCFThread::Loop(int64_t elapsed_ns) noexcept {
    FSMStates current_state = fsm_memory->Read();
    
    switch (current_state) {
        case FSMStates::INIT:
            lecturePoints();
            break;
        case FSMStates::IDLE:
            initialiserTimer();
            break;
        case FSMStates::AUTOMATIC_FLIGHT:
            verifierTemps();
            break;
        case FSMStates::FORCED_LANDING:
            updateGuidanceWaypoint();
            break;
        default:
            break;
    }

    return LoopControl::Continue;
}

// Configuration du thread
cactus_rt::CyclicThreadConfig FCFThread::MakeConfig() {
    cactus_rt::CyclicThreadConfig config;
    config.period_ns = 10'000'000;
    config.SetFifoScheduler(90);
    return config;
}

// Lecture du fichier JSON
bool FCFThread::lecturePoints() {
    ifstream fichier(filename);
    if (!fichier) {
        cerr << "Erreur : impossible d'ouvrir le fichier " << filename << endl;
        return false;
    }
    
    json data;
    fichier >> data;
    
    try {
        position = data.at("postition").get<vector<vector<double>>>();
        speed = data.at("speed").get<vector<vector<double>>>();
        thrust = data.at("thrust").get<vector<double>>();
        inclinaison = data.at("inclinaison").get<vector<double>>();
        guidance_mode = data.at("guidance_mode").get<vector<double>>();
        times = data.at("times").get<vector<double>>();
    } catch (const json::exception& e) {
        cerr << "Erreur lors de la lecture des données JSON : " << e.what() << endl;
        return false;
    }
    
    cout << "[INIT] Trajectoire et temps chargés." << endl;
    return true;
}

// Initialisation du timer
void FCFThread::initialiserTimer() {
    timer = 0;
    point = 0; // Réinitialisation du point
    cout << "[IDLE] Timer initialisé à 0." << endl;
}

// Vérification du timer
void FCFThread::verifierTemps() {
    if (point < times.size()) {
        double expected_time = times[point];

        if (timer >= expected_time) {
            point++;
        }
    }
    timer += 0.01;
}

void FCFThread::updateGuidanceWaypoint() {
    std::vector<double> current_state = current_state_memory->Read();

    if (current_state.size() < 9) {
        cerr << "Erreur : current_state_memory contient un vecteur de taille incorrecte." << endl;
        return;
    }

    std::vector<double> modified_state = current_state;
    modified_state[2] = 0.0; 

    guidance_waypoint_output_memory->Write(modified_state);
    cout << "[UPDATE] Guidance waypoint mis à jour avec z = 0." << endl;
}
