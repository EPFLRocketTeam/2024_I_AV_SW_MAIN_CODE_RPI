#include "fcf_module_thread.h"
#include "fsm_states.h"
#include <cactus_rt/rt.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <list>
#include <chrono>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;
using cactus_rt::CyclicThread;

FCFThread::FCFThread(SharedMemory<std::list<double>> *control_state, 
                     SharedMemory<FSMStates> *fsm_state_memory, 
                     SharedMemory<std::vector<double>> *current_state_memory, 
                     SharedMemory<std::vector<double>> *guidance_waypoint_output_memory, 
                     const std::string& filename)

    : CyclicThread("FCFThread", MakeConfig()), 
      control_memory(control_memory), 
      fsm_state_memory(fsm_state_memory), 
      current_state_memory(current_state_memory), 
      guidance_waypoint_output_memory(guidance_waypoint_output_memory), 
      filename(filename), 
      //timer(0), 
      point(0) { // Initialisation de point ici
    position.clear();
    speed.clear();
    thrust.clear();
    inclinaison.clear();
    guidance_mode.clear();

}

// // Gestion de la boucle d'exécution
// CyclicThread::LoopControl FCFThread::Loop(int64_t elapsed_ns) noexcept {
//     FSMStates current_state = fsm_state_memory->Read();
    
//     switch (current_state) {
//         case FSMStates::INIT:
//             lecturePoints();
//             break;
//         case FSMStates::IDLE:
//             initialiserTimer();
//             break;
//         case FSMStates::AUTOMATIC_FLIGHT:
//             verifierTemps();
//             break;
//         case FSMStates::FORCED_LANDING:
//             updateGuidanceWaypoint();
//             break;
//         default:
//             break;
//     }

//     return LoopControl::Continue;
// }
CyclicThread::LoopControl FCFThread::Loop(int64_t elapsed_ns) noexcept {
    FSMStates current_state = fsm_state_memory->Read();
    //std::cout << "[LOOP] État FSM actuel: " << static_cast<int>(current_state) << std::endl;

    switch (current_state) {
        case FSMStates::INIT:
            //std::cout << "[LOOP] State INIT - lecturePoints()" << std::endl;
            lecturePoints();
            break;
        case FSMStates::IDLE:
            //std::cout << "[LOOP] State IDLE - initialiserTimer()" << std::endl;
            initialiserTimer();
            break;
        case FSMStates::AUTOMATIC_FLIGHT:
            //std::cout << "[LOOP] State AUTOMATIC_FLIGHT - verifierTemps()" << std::endl;
            verifierTemps();
            break;
        case FSMStates::FORCED_LANDING:
            //std::cout << "[LOOP] State FORCED_LANDING - updateGuidanceWaypoint()" << std::endl;
            updateGuidanceWaypoint();
            break;
        default:
            //std::cout << "[LOOP] State non géré" << std::endl;
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
    //std::cout << "[DEBUG] Ouverture du fichier : " << filename << std::endl;
    ifstream fichier(filename);
    if (!fichier) {
        cerr << "Erreur : impossible d'ouvrir le fichier " << filename << endl;
        return false;
    }
    
    json data;
    fichier >> data;
    
    try {
        position = data.at("position").get<vector<vector<double>>>();
        speed = data.at("speed").get<vector<vector<double>>>();
        thrust = data.at("thrust").get<vector<double>>();
        inclinaison = data.at("inclinaison").get<vector<double>>();
        guidance_mode = data.at("guidance_mode").get<vector<double>>();
        times = data.at("times").get<vector<double>>();
    } catch (const json::exception& e) {
        cerr << "Erreur lors de la lecture des données JSON : " << e.what() << endl;
        return false;
    }
    
    //cout << "[INIT] Trajectoire et temps chargés." << endl;
    //afficheDonnees();
    return true;
}

// // Initialisation du timer
// void FCFThread::initialiserTimer() {
//     timer = 0;
//     point = 0; // Réinitialisation du point
//     cout << "[IDLE] Timer initialisé à 0." << endl;
//     //afficheDonnees();
// }
void FCFThread::initialiserTimer() {
    start_time = std::chrono::steady_clock::now();
    point = 0;
    //cout << "[IDLE] Timer initialisé (chrono)." << endl;
}

// // Vérification du timer
// void FCFThread::verifierTemps() {
//     if (point < times.size()) {
//         double expected_time = times[point];

//         if (timer >= expected_time) {
//             point++;
//         }
//     }
//     timer += 0.01;
//     cout << "le timer est : " << timer << endl;
// }
void FCFThread::verifierTemps() {
    auto now = std::chrono::steady_clock::now();
    double elapsed_seconds = std::chrono::duration<double>(now - start_time).count();

    if (point < times.size()) {
        double expected_time = times[point];

        if (elapsed_seconds >= expected_time) {
            point++;
        }
    }

    cout << "[TIMER] Temps écoulé : " << elapsed_seconds << " s" << endl;
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
    //cout << "[UPDATE] Guidance waypoint mis à jour avec z = 0." << endl;
}
// void FCFThread::afficheDonnees()  {
//     cout << "=== Contenu des données lues ===" << endl;

//     cout << "Position:" << endl;
//     for ( auto& pos : position) {
//         cout << "  [";
//         for (size_t i = 0; i < pos.size(); ++i) {
//             cout << pos[i];
//             if (i != pos.size() - 1) cout << ", ";
//         }
//         cout << "]" << endl;
//     }

//     cout << "Speed:" << endl;
//     for ( auto& spd : speed) {
//         cout << "  [";
//         for (size_t i = 0; i < spd.size(); ++i) {
//             cout << spd[i];
//             if (i != spd.size() - 1) cout << ", ";
//         }
//         cout << "]" << endl;
//     }

//     cout << "Thrust:" << endl;
//     for ( auto& thr : thrust) {
//         cout << "  " << thr << endl;
//     }

//     cout << "Inclinaison:" << endl;
//     for ( auto& incl : inclinaison) {
//         cout << "  " << incl << endl;
//     }

//     cout << "Guidance_mode:" << endl;
//     for ( auto& mode : guidance_mode) {
//         cout << "  " << mode << endl;
//     }

//     cout << "Times:" << endl;
//     for ( auto& t : times) {
//         cout << "  " << t << endl;
//     }

//     cout << "=== Fin des données ===" << endl;
// }
