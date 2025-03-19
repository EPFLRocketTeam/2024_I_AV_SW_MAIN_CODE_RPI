#include "god.h"

using namespace std;

GOD::GOD() { 
    // initialize the memories
    control_memory.Write(ControlOutput{0, 0, 0, 0}); //What should the initial values be?
    fsm_state_memory.Write(FSMStates::IDLE);
    current_state_memory.Write(vector<double>{0, 0, 0, 0, 0, 0, 0, 0, 0});
    waypoint_state_memory.Write(vector<double>{0, 0, 2, 0, 0, 0, 0, 0, 0});
}

string GOD::log_data() const
{
    string data = "";

    // Récupère les données des mémoires partagées
    ControlOutput control_output = control_memory.Read();
    vector<double> current_state = current_state_memory.Read();
    vector<double> waypoint_state = waypoint_state_memory.Read();
    vector<double> guidance_output = guidance_output_memory.Read();
    FSMStates fsm_state = fsm_state_memory.Read();

    // Utilise LOG_INFO pour enregistrer les données dans le fichier de log
    data += "FSM State: {}" + static_cast<int>(fsm_state) + (string) "\n";
    data += "Control Output: [{}, {}, {}, {}]" + to_string(control_output.d1) + to_string(control_output.d2) + to_string(control_output.thrust) + to_string(control_output.mz) + (string) "\n";

    // Log les vecteurs sous forme de chaine de caracteres
    string current_state_str = "[";
    for (size_t i = 0; i < current_state.size(); ++i) {
        current_state_str += to_string(current_state[i]);
        if (i != current_state.size() - 1) current_state_str += ", ";
    }
    current_state_str += "]";
    data += "Current State: {}" + current_state_str + "\n";

    string waypoint_state_str = "[";
    for (size_t i = 0; i < waypoint_state.size(); ++i) {
        waypoint_state_str += to_string(waypoint_state[i]);
        if (i != waypoint_state.size() - 1) waypoint_state_str += ", ";
    }
    waypoint_state_str += "]";
    data += "Waypoint State: {}" + waypoint_state_str + (string) "\n";

    string guidance_output_str = "[";
    for (size_t i = 0; i < guidance_output.size(); ++i) {
        guidance_output_str += to_string(guidance_output[i]);
        if (i != guidance_output.size() - 1) guidance_output_str += ", ";
    }
    guidance_output_str += "]";
    data +=  "Guidance Output: {}" + guidance_output_str + (string) "\n";
    data += "--------------------------------------\n";
    return data;
}

