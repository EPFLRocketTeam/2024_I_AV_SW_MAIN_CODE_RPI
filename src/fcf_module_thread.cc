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

FCFThread::FCFThread(SharedMemory<FSMStates> *control_memory, SharedMemory<FSMStates> *fsm_memory, const string& filename)
    : CyclicThread("FCFThread", MakeConfig()), control_memory(control_memory), fsm_memory(fsm_memory), filename(filename), timer(0) {
    points.clear();
}

// Gestion de la boucle d'exécution
CyclicThread::LoopControl FCFThread::Loop(int64_t elapsed_ns) noexcept {
    FSMStates current_state = fsm_memory->Read(); // Lire l'état actuel de la mémoire partagée
    
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
        default:
            break;
    }

    return LoopControl::Continue;
}

// Configuration du thread
cactus_rt::CyclicThreadConfig FCFThread::MakeConfig() {
    cactus_rt::CyclicThreadConfig config;
    config.period_ns = 10'000'000; // 100 Hz
    config.SetFifoScheduler(90);
    return config;
}

// Lecture du fichier JSON pour extraire les points et le temps
bool FCFThread::lecturePoints() {
    ifstream fichier(filename);
    if (!fichier) {
        cerr << "Erreur : impossible d'ouvrir le fichier " << filename << endl;
        return false;
    }
    
    json data;
    fichier >> data;
    
    try {
        points = data.at("points").get<vector<vector<double>>>(); // Liste de points 3D
        times = data.at("times").get<vector<double>>(); // Liste des temps
    } catch (const json::exception& e) {
        cerr << "Erreur lors de la lecture des données JSON : " << e.what() << endl;
        return false;
    }
    
    cout << "[INIT] Points et temps chargés depuis le JSON." << endl;
    return true;
}

// Initialisation du timer dans l'état IDLE
void FCFThread::initialiserTimer() {
    timer = 0;
    cout << "[IDLE] Timer initialisé à 0." << endl;
}

// Vérification du timer dans AUTOMATIC_FLIGHT
void FCFThread::verifierTemps() {
    if (timer < times.size()) {
        double expected_time = times[timer];
        if (abs(expected_time - timer) > 1.0) {
            cerr << "[ERREUR] Temps actuel: " << timer << ", attendu: " << expected_time << endl;
        } else {
            cout << "[AUTOMATIC_FLIGHT] Temps validé: " << timer << endl;
        }
        timer++;
    }
}
