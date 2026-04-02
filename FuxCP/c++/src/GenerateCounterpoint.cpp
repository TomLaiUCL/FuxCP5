// Générateur générique de contrepoints à n voix
// Utilisation : ./GenerateCounterpoint <nb_voix> <especes...>
// Exemple : ./GenerateCounterpoint 4 3 3 3 3 (pour 4 voix, toutes en 3e espèce)

#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include "../headers/Parts/Midi.hpp"
#include "../headers/CounterpointProblems/FourVoiceCounterpoint.hpp"
#include "../headers/CounterpointUtils.hpp"
#include "../headers/Parts/CantusFirmus.hpp"
#include "../headers/Utilities.hpp"

using namespace std;
using namespace std::chrono;

// Cantus firmus par défaut
vector<int> default_cf = {60, 62, 65, 64, 67, 65, 64, 62, 60};

// Conversion d'un int en Species
Species int_to_species(int sp) {
    switch(sp) {
        case 1: return FIRST_SPECIES;
        case 2: return SECOND_SPECIES;
        case 3: return THIRD_SPECIES;
        case 4: return FOURTH_SPECIES;
        case 5: return FIFTH_SPECIES;
        default: return THIRD_SPECIES;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage : ./GenerateCounterpoint <nb_voix> <especes...> (hors cantus firmus)\nExemple : ./GenerateCounterpoint 2 1 (pour CF + 1ère espèce)" << endl;
        return 1;
    }
    int nb_voix = atoi(argv[1]);
    if (nb_voix < 2 || nb_voix > 4) {
        cout << "Nombre de voix supporté : 2, 3 ou 4 (incluant le cantus firmus)" << endl;
        return 1;
    }
    // argc inclut le nom du programme ; on attend : <programme> <nb_voix> <espèce1> ... <espèceN>
    if (argc != nb_voix + 1) {
        cout << "Nombre d'arguments fourni (" << argc << ") != " << (nb_voix + 1) << " (programme + nb_voix + " << (nb_voix-1) << " espèces)" << endl;
        cout << "Il faut fournir " << (nb_voix-1) << " espèces (hors cantus firmus) !" << endl;
        return 1;
    }
    // Espèces : cantus firmus + voix générées
    vector<int> species_input;
    vector<Species> species;
    species.push_back(CANTUS_FIRMUS); // CF toujours en première voix
    for (int i = 0; i < nb_voix-1; ++i) {
        int sp = atoi(argv[2+i]);
        if (sp < 1 || sp > 3) {
            cout << "Espèce non supportée : " << sp << " (seulement 1, 2 ou 3)" << endl;
            return 1;
        }
        species_input.push_back(sp);
        species.push_back(int_to_species(sp));
    }
    // Paramètres par défaut
    vector<int> v_type(nb_voix, 0);
    vector<int> melodic_params = {0, 1, 1, 576, 2, 2, 2, 1};
    vector<int> general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    vector<int> specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    vector<int> importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    int borrowMode = 1;

    auto start = steady_clock::now();
    CounterpointProblem* problem = nullptr;
    if(nb_voix == 2) {
    problem = new TwoVoiceCounterpoint(default_cf, species[1], v_type[1], melodic_params, general_params, specific_params, importance, borrowMode);
    } else if(nb_voix == 3) {
        problem = new ThreeVoiceCounterpoint(default_cf, {species[1], species[2]}, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    } else if(nb_voix == 4) {
        problem = new FourVoiceCounterpoint(default_cf, {species[1], species[2], species[3]}, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    }

    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int nb_iter = 0;
    double best_cost = 1e9;
    auto last_best = steady_clock::now();
    while (true) {
        auto now = steady_clock::now();
        if (duration_cast<seconds>(now - start).count() > 15) break;
        CounterpointProblem* s = e.next();
        if (!s) break;
        double cost = s->getCost();
        if (cost < best_cost) {
            delete best;
            best = s;
            best_cost = cost;
            last_best = now;
        } else {
            delete s;
        }
        nb_iter++;
    }
    auto end = steady_clock::now();
    double elapsed = duration_cast<milliseconds>(end - start).count()/1000.0;

    // Sauvegarde du résultat
    if (best) {
        vector<int> solVec;
        for (int i = 0; i < best->getSolutionArray().size(); ++i) {
            solVec.push_back(best->getSolutionArray()[i].val());
        }
        // Fichier MIDI
        // Nom de fichier : <nvoices>voices_<esp_cf>_<esp_1>_..._<esp_N>.mid
        vector<int> all_species;
        all_species.push_back(1); // cantus firmus (toujours 1)
        for(auto sp : species_input) all_species.push_back(sp);
        string species_tag = to_string(nb_voix) + "voices";
        for(auto sp : all_species) species_tag += "_" + to_string(sp);
        string midi_name = "../results/" + species_tag + ".mid";
        saveMidi(midi_name, default_cf, solVec, species[1]);
        // Fichier résultat
        string res_name = "../results/" + species_tag + ".txt";
        ofstream res(res_name);
        res << "Coût de la solution : " << best_cost << endl;
        res << "Nombre d'itérations : " << nb_iter << endl;
        res << "Temps d'exécution (s) : " << elapsed << endl;
        res << "Espèces : ";
        for(auto sp : all_species) res << sp << " ";
        res << endl;
        res << "Solution : ";
        for(auto n : solVec) res << n << " ";
        res << endl;
        res.close();
        cout << "Résultats sauvegardés dans " << res_name << " et " << midi_name << endl;
        delete best;
    } else {
        cout << "Aucune solution trouvée." << endl;
    }
    delete problem;
    return 0;
}
