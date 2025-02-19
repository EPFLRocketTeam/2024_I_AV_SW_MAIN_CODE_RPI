
#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <cmath>
#include <chrono>
#include <thread>
//#include "json.hpp" 

using namespace std;
using json = nlohmann::json; // Alias pour la bibliothèque JSON

// Déclaration de la fonction
bool lecture(vector<double>& guidance, vector<double>& position,
    vector<double>& vitesse,
    double& thrust, vector<double>& angle, vector<double>& time2reach, double& time,
    const string& texte);
void calcul(vector<double>& guidance,
    vector<double>& position,
    const vector<double>& vitesse,
    const vector<double>& angle, vector<double>& time2reach, double time, double thrust);
void affichage(const vector<double>& position);

int main() { 
    // Variables à remplir par la fonction lecture
    vector<double> guidance;
    vector<double> position;// Tableau de nombres décimaux pour la position
    double thrust;
    vector<double> vitesse;
    vector<double> angle;
    vector<double> time2reach;
    double time;
    string nomFichier;
    ifstream fichier(nomFichier);
    cout << "Entrez le nom du fichier a lire : ";
    cin >> nomFichier;


    // Appel de la fonction lecture
    lecture(guidance, position, vitesse, thrust, angle, time2reach, time, nomFichier);
    // Afficher les valeurs lues si la lecture est réussie
    cout << " les prochains points a atteindre : ";
    for (const auto& d : guidance) {

        cout << d << " ";
    }
    cout << endl;
    cout << "Nombres position lus : ";
    for (const auto& d : position) {
        cout << d << " ";
    }
    cout << endl;
    cout << "Nombres decimaux pour la vitesse lus : ";
    for (const auto& d : vitesse) {
        cout << d << " ";
    }
    cout << endl;
    cout << "la poussee est de : " << thrust << endl;

    cout << "Nombres decimaux pour la angle lus : ";
    for (const auto& d : angle) {
        cout << d << " ";
    }
    cout << endl;
    cout << "le temps en regime stationaire : " ;
    for (const auto& d : time2reach) {
        cout << d << " ";
    }
    cout << endl;
    cout << "au temps : " << time << endl;

    cout << "Texte lu : " << nomFichier << endl;
    calcul(guidance, position, vitesse, angle, time2reach, time, thrust);
    return 0;
}

// Définition de la fonction lecture
bool lecture(vector<double>& guidance, vector<double>& position,
    vector<double>& vitesse, double& thrust,
    vector<double>& angle, vector<double>& time2reach, double& time, const string& texte) {
    // Ouvrir le fichier
    ifstream fichier(texte);
    if (!fichier) {
        return false; // Retourne false si le fichier ne peut pas être ouvert
    }
    json data;
    fichier >> data;
    // Lire les données depuis le fichier
    try {
        // Extraire les données JSON dans les variables
        guidance = data.at("point to reach").get<vector<double>>();
        position = data.at("position").get<vector<double>>();
        vitesse = data.at("vitesse").get<vector<double>>();
        thrust = data.at("thrust").get<double>();
        angle = data.at("angle").get<vector<double>>();
        time2reach = data.at("time to reach").get<vector<double>>();
        time = data.at("time").get<double>();
    }
    catch (const json::exception& e) {
        cerr << "Erreur lors de la lecture des données JSON : " << e.what() << endl;
        return false;
    }

    return true; // Retourne true si tout s'est bien passé
}

void calcul(vector<double>& guidance,
    vector<double>& position,
    const vector<double>& vitesse,
    const vector<double>& angle,
    vector<double>& time2reach, double time, double thrust)
{
    int count(0);
    vector<double> new_guidance;
    new_guidance = guidance;
    size_t last_point = guidance.size();
    // Mise à jour de chaque coordonnée après une seconde
    for (int i = 0; i < 3; ++i) {
        // Mise à jour de la position en fonction de la vitesse et de l'angle
        position[i] += vitesse[i] + vitesse[i] * sin(angle[0]) + vitesse[i] * sin(angle[1]);// Composante selon l'axe i           
    }
    affichage(position);

   // if ((time > time2reach[count]) or (abs(guidance[0] - position[0]) < 0.15) and
     //   (abs(guidance[1] - position[1]) < 0.15) and (abs(guidance[2] - position[2]) < 0.1)) { // test si on a dépacé le temps max ou si on a passeé le point à atteindre 
       // if (last_point > 3) {
         //   guidance.clear();
           // for (int i(0); i < last_point - 3; ++i) {
             //   guidance.push_back(new_guidance[i + 3]);

            //}

      //  }
        affichage(guidance);
        count++; 

   // }
}


void affichage(const vector<double>& position) {
    // Affichage des résultats pour vérification
    cout << "Nouvelle position apres un intervalle de temps : ";
    for (const auto& coord : position) {
        cout << coord << " ";
    }
    cout << endl;

}




