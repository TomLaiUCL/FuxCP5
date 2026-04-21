// Générateur générique de contrepoints à n voix avec métriques détaillées
// Utilisation : ./GenerateCounterpoint <nb_voix> <especes...> [-c cf_number] [-t timeout_ms] [-s stagnation_ms]
// Exemple : ./GenerateCounterpoint 4 3 3 3 (pour 4 voix, toutes en 3e espèce)
// Exemple : ./GenerateCounterpoint 2 1 -c 2 (2 voix, 1ère espèce, CF n°2)
// Exemple : ./GenerateCounterpoint 2 5 -c 3 -t 300000 -s 120000 (CF n°3, timeout max 5min, stagnation 2min)
// Exemple : ./GenerateCounterpoint 3 2 3 -c 1 -t 60000 (3 voix, CF n°1, timeout 60s)

#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <utility>
#include <iomanip>
#include <map>
#include <algorithm>
#include "../headers/Parts/Midi.hpp"
#include "../headers/CounterpointProblems/FourVoiceCounterpoint.hpp"
#include "../headers/CounterpointUtils.hpp"
#include "../headers/Parts/CantusFirmus.hpp"
#include "../headers/Utilities.hpp"

using namespace std;
using namespace std::chrono;

// Registre des cantus firmus disponibles
struct CFInfo {
    string filename;
    string tonality;
};

map<int, CFInfo> cf_registry = {
    {1, {"cf1_do_majeur.txt", "Do Majeur"}},
    {2, {"cf2_la_mineur.txt", "La Mineur"}},
    {3, {"cf3_fa_majeur.txt", "Fa Majeur"}},
    {4, {"cf4_re_dorien.txt", "Ré Dorien"}},
    {5, {"cf5_do_majeur_fux.txt", "Do Majeur (Fux)"}},
    {6, {"cf6_do_majeur_long.txt", "Do Majeur long (13 notes)"}},
    {7, {"cf7_fa_majeur_long.txt", "Fa Majeur expressif (14 notes)"}},
    {8, {"cf8_sol_majeur.txt", "Sol Majeur (13 notes)"}},
    {9, {"cf9_re_majeur_long.txt", "Ré Majeur long (13 notes)"}},
    {10, {"cf10_sib_majeur.txt", "Sib Majeur (11 notes)"}}
};

// Conversion MIDI → nom de note français
string midiToFrench(int note) {
    const string names[] = {"Do", "Do#", "Ré", "Mib", "Mi", "Fa", "Fa#", "Sol", "Lab", "La", "Sib", "Si"};
    int octave = note / 12 - 1;
    return names[note % 12] + to_string(octave);
}

// Intervalle de checkpoint (toutes les N solutions)
const int CHECKPOINT_INTERVAL = 1000;

// Structure pour stocker un checkpoint
struct Checkpoint {
    int solution_num;
    double elapsed_ms;
    double cost;
};

// Classe de stop combinant timeout max et détection de stagnation
class StagnationStop : public Search::Stop {
    Search::TimeStop maxStop;
    steady_clock::time_point lastImprovement;
    int stagnation_ms;
    bool stagnation_triggered;
    bool max_triggered;
public:
    StagnationStop(int max_ms, int stag_ms)
        : maxStop(max_ms), stagnation_ms(stag_ms),
          lastImprovement(steady_clock::now()),
          stagnation_triggered(false), max_triggered(false) {}

    void notifyImprovement() {
        lastImprovement = steady_clock::now();
    }

    bool stop(const Search::Statistics& s, const Search::Options& o) {
        if (maxStop.stop(s, o)) {
            max_triggered = true;
            return true;
        }
        if (stagnation_ms > 0) {
            auto elapsed = duration_cast<milliseconds>(steady_clock::now() - lastImprovement).count();
            if (elapsed > stagnation_ms) {
                stagnation_triggered = true;
                return true;
            }
        }
        return false;
    }

    bool wasStagnation() const { return stagnation_triggered; }
    bool wasMaxTimeout() const { return max_triggered; }

    string stopReason() const {
        if (stagnation_triggered) return "STAGNATION";
        if (max_triggered) return "TIMEOUT_MAX";
        return "EXHAUSTIVE";
    }
};

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

// Charge un cantus firmus depuis un fichier (notes MIDI séparées par des espaces ou retours à la ligne)
vector<int> loadCfFromFile(const string& path) {
    vector<int> cf;
    ifstream f(path);
    if (!f.is_open()) {
        cerr << "Impossible d'ouvrir le fichier CF : " << path << endl;
        return cf;
    }
    int note;
    while (f >> note) {
        cf.push_back(note);
    }
    return cf;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage : ./GenerateCounterpoint <nb_voix> <especes...> [-c cf_number] [-t timeout_ms] [-s stagnation_ms]" << endl;
        cout << "  nb_voix : 2, 3 ou 4 (incluant le cantus firmus)" << endl;
        cout << "  especes : 1 à 5 pour chaque voix de contrepoint" << endl;
        cout << "  -c cf_number   : (optionnel) numéro du cantus firmus (défaut : 1)" << endl;
        cout << "  -t timeout_ms  : (optionnel) timeout max en ms (défaut : 300000 = 5 min)" << endl;
        cout << "  -s stagnation_ms : (optionnel) arrêt si pas d'amélioration depuis N ms (défaut : 120000 = 2 min)" << endl;
        cout << "  -v variety_cost : (optionnel) coût de variété, plus élevé = moins de répétitions (défaut : 2)" << endl;
        cout << "Exemple : ./GenerateCounterpoint 2 1" << endl;
        cout << "Exemple : ./GenerateCounterpoint 3 2 3 -c 2 -t 300000 -s 120000" << endl;
        cout << "Exemple : ./GenerateCounterpoint 2 1 -c 6 -v 8 (CF long, haute variété)" << endl;
        return 1;
    }
    int nb_voix = atoi(argv[1]);
    if (nb_voix < 2 || nb_voix > 4) {
        cout << "Nombre de voix supporté : 2, 3 ou 4 (incluant le cantus firmus)" << endl;
        return 1;
    }
    int nb_species_args = nb_voix - 1;
    if (argc < nb_species_args + 2) {
        cout << "Il faut fournir " << nb_species_args << " espèces (hors cantus firmus) !" << endl;
        return 1;
    }
    // Lecture des espèces
    vector<int> species_input;
    vector<Species> species;
    species.push_back(CANTUS_FIRMUS);
    for (int i = 0; i < nb_species_args; ++i) {
        int sp = atoi(argv[2 + i]);
        if (sp < 1 || sp > 5) {
            cout << "Espèce non supportée : " << sp << " (seulement 1 à 5)" << endl;
            return 1;
        }
        species_input.push_back(sp);
        species.push_back(int_to_species(sp));
    }

    // Parsing des arguments optionnels après les espèces
    int timeout_ms = 300000;    // défaut : 5 min
    int stagnation_ms = 120000; // défaut : 2 min sans amélioration
    int cf_number = 1;          // défaut : CF n°1
    int variety_override = -1;  // -1 = pas de surcharge (utilise défaut = 2)
    int next_arg = nb_species_args + 2;
    while (next_arg < argc) {
        string arg = argv[next_arg];
        if (arg == "-t" && next_arg + 1 < argc) {
            timeout_ms = atoi(argv[next_arg + 1]);
            if (timeout_ms <= 0) {
                cout << "Timeout invalide, utilisation du défaut (300000 ms)" << endl;
                timeout_ms = 300000;
            }
            next_arg += 2;
        } else if (arg == "-s" && next_arg + 1 < argc) {
            stagnation_ms = atoi(argv[next_arg + 1]);
            if (stagnation_ms < 0) {
                cout << "Stagnation invalide, utilisation du défaut (120000 ms)" << endl;
                stagnation_ms = 120000;
            }
            next_arg += 2;
        } else if (arg == "-c" && next_arg + 1 < argc) {
            cf_number = atoi(argv[next_arg + 1]);
            if (cf_registry.find(cf_number) == cf_registry.end()) {
                cout << "CF n°" << cf_number << " inconnu. CF disponibles :" << endl;
                for (auto& kv : cf_registry) cout << "  " << kv.first << " : " << kv.second.tonality << endl;
                return 1;
            }
            next_arg += 2;
        } else if (arg == "-v" && next_arg + 1 < argc) {
            variety_override = atoi(argv[next_arg + 1]);
            if (variety_override < 0) variety_override = 2;
            next_arg += 2;
        } else {
            cout << "Argument inconnu : " << arg << endl;
            next_arg++;
        }
    }

    // Chargement du CF depuis le registre
    vector<int> cf = loadCfFromFile(cf_registry[cf_number].filename);
    if (cf.empty()) {
        cout << "Erreur : impossible de charger le CF n°" << cf_number << " (" << cf_registry[cf_number].filename << ")" << endl;
        return 1;
    }
    cout << "CF n°" << cf_number << " (" << cf_registry[cf_number].tonality << ") chargé : " << cf.size() << " notes" << endl;

    // Tag pour les noms de fichiers
    vector<int> all_species;
    all_species.push_back(1); // cantus firmus
    for(auto sp : species_input) all_species.push_back(sp);
    string species_tag = to_string(nb_voix) + "voices";
    for(auto sp : all_species) species_tag += "_" + to_string(sp);

    cout << "=== Génération : " << species_tag << " | timeout_max=" << timeout_ms << " ms | stagnation=" << stagnation_ms << " ms ===" << endl;

    // Activation de TOUTES les contraintes musicales
    fill(activeConstraints.begin(), activeConstraints.end(), true);

    // Paramètres par défaut
    vector<int> v_type(nb_voix, 0);
    vector<int> melodic_params = {0, 1, 1, 576, 2, 2, 2, 1};
    vector<int> general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    if (variety_override >= 0) {
        general_params[4] = variety_override;  // surcharge du coût de variété
        cout << "Coût de variété surchargé : " << variety_override << endl;
    }
    vector<int> specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    vector<int> importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    int borrowMode = 1;

    auto start = steady_clock::now();
    auto ms_since_start = [&]() -> double {
        return duration<double, milli>(steady_clock::now() - start).count();
    };

    CounterpointProblem* problem = nullptr;
    if(nb_voix == 2) {
        problem = new TwoVoiceCounterpoint(cf, species[1], v_type[1], melodic_params, general_params, specific_params, importance, borrowMode);
    } else if(nb_voix == 3) {
        problem = new ThreeVoiceCounterpoint(cf, {species[1], species[2]}, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    } else if(nb_voix == 4) {
        problem = new FourVoiceCounterpoint(cf, {species[1], species[2], species[3]}, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    }

    // Recherche BAB avec timeout max + détection de stagnation
    StagnationStop stopObj(timeout_ms, stagnation_ms);
    Search::Options opt;
    opt.stop = &stopObj;
    BAB<CounterpointProblem> e(problem, opt);

    CounterpointProblem* best = nullptr;
    int nb_solutions = 0;
    int nb_improvements = 0;
    double best_cost = 1e9;
    double ms_first_solution = -1;
    double ms_last_improvement = -1;
    vector<Checkpoint> checkpoints;
    // Historique des améliorations : (numéro solution, temps ms, coût)
    vector<Checkpoint> improvements;

    while (CounterpointProblem* s = e.next()) {
        nb_solutions++;
        double cost = s->getCost();
        double now_ms = ms_since_start();

        if (nb_solutions == 1) {
            ms_first_solution = now_ms;
        }

        if (cost < best_cost) {
            nb_improvements++;
            delete best;
            best = s;
            best_cost = cost;
            ms_last_improvement = now_ms;
            improvements.push_back({nb_solutions, now_ms, cost});
            stopObj.notifyImprovement();
        } else {
            delete s;
        }

        // Checkpoint toutes les CHECKPOINT_INTERVAL solutions
        if (nb_solutions % CHECKPOINT_INTERVAL == 0) {
            checkpoints.push_back({nb_solutions, now_ms, best_cost});
        }
    }

    bool stopped = e.stopped();
    bool timed_out = stopped; // compatibilité
    string stop_reason = stopped ? stopObj.stopReason() : "EXHAUSTIVE";
    Search::Statistics stats = e.statistics();
    double total_ms = ms_since_start();

    // Arborescence de sortie (créée dès maintenant, quelle que soit l'issue)
    string voices_dir = to_string(nb_voix) + "voices";
    string cf_dir = "cf" + to_string(cf_number);
    string base_dir = "../results/" + voices_dir + "/" + cf_dir;
    string txt_dir = base_dir + "/txt";
    string csv_dir = base_dir + "/csv";
    string midi_dir = base_dir + "/midi";
    system(("mkdir -p " + txt_dir + " " + csv_dir + " " + midi_dir).c_str());

    // Fichier descriptif du cantus firmus (créé une seule fois)
    {
        string cf_desc_path = base_dir + "/cantus_firmus.txt";
        ifstream check(cf_desc_path);
        if (!check.good()) {
            ofstream cf_desc(cf_desc_path);
            for (size_t i = 0; i < cf.size(); ++i) {
                if (i > 0) cf_desc << " ";
                cf_desc << cf[i];
            }
            cf_desc << endl;
            for (size_t i = 0; i < cf.size(); ++i) {
                if (i > 0) cf_desc << " ";
                cf_desc << midiToFrench(cf[i]);
            }
            cf_desc << endl;
            cf_desc << cf_registry[cf_number].tonality << endl;
        }
    }

    // Sauvegarde du résultat
    if (best) {
        vector<int> solVec;
        for (int i = 0; i < best->getSolutionArray().size(); ++i) {
            solVec.push_back(best->getSolutionArray()[i].val());
        }

        // Découpage du solutionArray en voix individuelles
        int cfSize = cf.size();
        vector<pair<vector<int>, Species>> voices;
        int offset = 0;
        for (int v = 0; v < nb_voix - 1; ++v) {
            int sz = branchingNotesSize(species[v + 1], cfSize);
            vector<int> voice_notes(solVec.begin() + offset, solVec.begin() + offset + sz);
            voices.push_back({voice_notes, species[v + 1]});
            offset += sz;
        }

        // Fichier MIDI
        string midi_name = midi_dir + "/" + species_tag + ".mid";
        saveMidiMultiVoice(midi_name, cf, voices);

        // === Fichier résultat TXT détaillé ===
        string res_name = txt_dir + "/" + species_tag + ".txt";
        ofstream res(res_name);
        res << "========================================" << endl;
        res << "  RÉSULTATS : " << species_tag << endl;
        res << "========================================" << endl;
        res << endl;

        res << "--- Configuration ---" << endl;
        res << "Nombre de voix : " << nb_voix << endl;
        res << "Espèces : ";
        for(auto sp : all_species) res << sp << " ";
        res << endl;
        res << "Timeout max (ms) : " << timeout_ms << endl;
        res << "Stagnation (ms) : " << stagnation_ms << endl;
        res << "Cantus Firmus : ";
        for(auto n : cf) res << n << " ";
        res << endl;
        res << endl;

        res << "--- Métriques de recherche ---" << endl;
        res << "Temps total (ms) : " << fixed << setprecision(1) << total_ms << endl;
        res << "Temps 1ère solution (ms) : " << fixed << setprecision(1) << ms_first_solution << endl;
        res << "Temps dernière amélioration (ms) : " << fixed << setprecision(1) << ms_last_improvement << endl;
        res << "Solutions trouvées : " << nb_solutions << endl;
        res << "Améliorations de coût : " << nb_improvements << endl;
        res << "Coût final : " << best_cost << endl;
        res << "Terminaison : " << stop_reason;
        if (stop_reason == "EXHAUSTIVE") res << " (optimal)";
        res << endl;
        res << endl;

        res << "--- Statistiques Gecode ---" << endl;
        res << "Nœuds explorés : " << stats.node << endl;
        res << "Échecs : " << stats.fail << endl;
        res << "Redémarrages : " << stats.restart << endl;
        res << "Propagations : " << stats.propagate << endl;
        res << "Profondeur max : " << stats.depth << endl;
        res << endl;

        if (!improvements.empty()) {
            res << "--- Historique des améliorations ---" << endl;
            res << setw(12) << "Solution #" << setw(14) << "Temps (ms)" << setw(12) << "Coût" << endl;
            for(auto& imp : improvements) {
                res << setw(12) << imp.solution_num 
                    << setw(14) << fixed << setprecision(1) << imp.elapsed_ms 
                    << setw(12) << fixed << setprecision(1) << imp.cost << endl;
            }
            res << endl;
        }

        if (!checkpoints.empty()) {
            res << "--- Checkpoints (toutes les " << CHECKPOINT_INTERVAL << " solutions) ---" << endl;
            res << setw(12) << "Solution #" << setw(14) << "Temps (ms)" << setw(16) << "Meilleur coût" << endl;
            for(auto& cp : checkpoints) {
                res << setw(12) << cp.solution_num 
                    << setw(14) << fixed << setprecision(1) << cp.elapsed_ms 
                    << setw(16) << fixed << setprecision(1) << cp.cost << endl;
            }
            res << endl;
        }

        res << "--- Solution ---" << endl;
        res << "Cantus Firmus : ";
        for(auto n : cf) res << n << " ";
        res << endl;
        for (size_t v = 0; v < voices.size(); ++v) {
            res << "Voix " << (v + 1) << " (espèce " << species_input[v] << ") : ";
            for(auto n : voices[v].first) res << n << " ";
            res << endl;
        }
        res << "Solution complète : ";
        for(auto n : solVec) res << n << " ";
        res << endl;
        res.close();

        // === Fichier CSV ===
        string csv_name = csv_dir + "/" + species_tag + ".csv";
        ofstream csv(csv_name);
        // En-tête
        csv << "species_tag,nb_voix,especes,timeout_ms,stagnation_ms,temps_total_ms,temps_premiere_solution_ms,"
            << "temps_derniere_amelioration_ms,nb_solutions,nb_ameliorations,cout_final,"
            << "terminaison,noeuds,echecs,redemarrages,propagations,profondeur_max" << endl;
        // Données
        csv << species_tag << "," << nb_voix << ",";
        for(size_t i = 0; i < all_species.size(); ++i) {
            if(i > 0) csv << "-";
            csv << all_species[i];
        }
        csv << "," << timeout_ms
            << "," << stagnation_ms
            << "," << fixed << setprecision(1) << total_ms
            << "," << fixed << setprecision(1) << ms_first_solution
            << "," << fixed << setprecision(1) << ms_last_improvement
            << "," << nb_solutions
            << "," << nb_improvements
            << "," << fixed << setprecision(1) << best_cost
            << "," << stop_reason
            << "," << stats.node
            << "," << stats.fail
            << "," << stats.restart
            << "," << stats.propagate
            << "," << stats.depth
            << endl;

        // Section historique améliorations
        if (!improvements.empty()) {
            csv << endl << "# Historique améliorations" << endl;
            csv << "solution_num,temps_ms,cout" << endl;
            for(auto& imp : improvements) {
                csv << imp.solution_num << "," 
                    << fixed << setprecision(1) << imp.elapsed_ms << "," 
                    << fixed << setprecision(1) << imp.cost << endl;
            }
        }

        // Section checkpoints
        if (!checkpoints.empty()) {
            csv << endl << "# Checkpoints" << endl;
            csv << "solution_num,temps_ms,meilleur_cout" << endl;
            for(auto& cp : checkpoints) {
                csv << cp.solution_num << "," 
                    << fixed << setprecision(1) << cp.elapsed_ms << "," 
                    << fixed << setprecision(1) << cp.cost << endl;
            }
        }
        csv.close();

        cout << "Résultats sauvegardés : " << res_name << " | " << csv_name << " | " << midi_name << endl;
        cout << "  Solutions=" << nb_solutions << " | Améliorations=" << nb_improvements 
             << " | Coût=" << best_cost << " | " << fixed << setprecision(0) << total_ms << " ms"
             << " | " << stop_reason << endl;
        delete best;
    } else {
        // Aucune solution : écriture d'un fichier d'erreur
        string err_name = txt_dir + "/error_" + species_tag + ".txt";
        ofstream err(err_name);
        err << "========================================" << endl;
        err << "  ERREUR : " << species_tag << endl;
        err << "========================================" << endl;
        err << endl;
        err << "Aucune solution trouvée." << endl;
        err << endl;
        err << "--- Configuration ---" << endl;
        err << "Nombre de voix : " << nb_voix << endl;
        err << "Espèces : ";
        for(auto sp : all_species) err << sp << " ";
        err << endl;
        err << "Cantus Firmus (CF n°" << cf_number << " " << cf_registry[cf_number].tonality << ") : ";
        for(auto n : cf) err << n << " ";
        err << endl;
        err << "Timeout max (ms) : " << timeout_ms << endl;
        err << "Stagnation (ms) : " << stagnation_ms << endl;
        err << endl;
        err << "--- Cause ---" << endl;
        err << "Raison d'arrêt : " << stop_reason << endl;
        err << "Temps écoulé (ms) : " << fixed << setprecision(1) << total_ms << endl;
        err << endl;
        err << "--- Statistiques Gecode ---" << endl;
        err << "Nœuds explorés : " << stats.node << endl;
        err << "Échecs : " << stats.fail << endl;
        err << "Propagations : " << stats.propagate << endl;
        err.close();
        cout << "ERREUR - Aucune solution : " << err_name << " (" << stop_reason << ", "
             << fixed << setprecision(0) << total_ms << " ms)" << endl;
    }
    delete problem;
    return 0;
}
