// =====================================================================
// GenerateCounterpoint.cpp — point d'entrée unifié de génération.
// Format aligné sur Generations.cpp : utilise GenerationCase + bench BAB
// séparé. Toutes les données autrefois hardcodées (registre CF, presets de
// paramètres, campagnes) sont chargées depuis FuxCP/config/*.csv.
//
// Usage CLI :
//   ./GenerateCounterpoint <nb_voix> <especes...> [options]
//
// Modes spéciaux :
//   ./GenerateCounterpoint --campaign <id>   (lance toutes les lignes)
//   ./GenerateCounterpoint --list-cf
//   ./GenerateCounterpoint --list-presets
//   ./GenerateCounterpoint --list-campaigns
//
// Options :
//   -c CF_ID            (défaut : 1)
//   --preset NAME       (défaut : default)
//   -t timeout_ms       (défaut : 300000)
//   -s stagnation_ms    (défaut : 120000)
//   -o output_root      (défaut : ../results)
//   --subdir NAME       (sous-dossier sous output_root)
//   -m lex|total|mixed  (défaut : lex)
//   -v vt1,vt2,...      (v_types des voix de contrepoint, séparés par ',')
// =====================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <map>
#include <string>
#include <utility>
#include <tuple>
#include <algorithm>
#include <gecode/int.hh>
#include <gecode/search.hh>

#include "../headers/Utilities.hpp"
#include "../headers/Midi.hpp"
#include "../headers/CounterpointUtils.hpp"
#include "../headers/CounterpointProblems/CounterpointProblem.hpp"
#include "../headers/CounterpointProblems/TwoVoiceCounterpoint.hpp"
#include "../headers/CounterpointProblems/ThreeVoiceCounterpoint.hpp"
#include "../headers/CounterpointProblems/FourVoiceCounterpoint.hpp"
#include "../headers/ConfigLoader.hpp"

using namespace std;
using namespace std::chrono;
using namespace Gecode;

// =============================================================
// Structures de données (alignées sur Generations.hpp)
// =============================================================

struct GenCase {
    int cf_id;
    vector<int> cf;
    string cf_name;          // libellé court (ex "Do Majeur (court)")
    string cf_scale;         // gamme indicative (informatif)

    int n_voices;
    vector<Species> spList;  // espèces (sans le CF, dans l'ordre voix 1..n-1)
    vector<int> v_type;      // taille = n_voices-1

    // Paramètres (issus du preset)
    string preset_name;
    vector<int> melodic_params;
    vector<int> general_params;
    vector<int> specific_params;
    vector<int> importance;
    int borrow_mode;
    ObjectiveMode obj_mode;

    int timeout_ms;
    int stagnation_ms;

    string output_root;       // racine, ex "../results"
    string output_subdir;     // sous-dossier optionnel ex "test_gammes"
};

struct BenchOutcome {
    int    nb_solutions      = 0;
    int    nb_improvements   = 0;
    double best_cost         = 1e9;
    double ms_first_solution = -1;
    double ms_last_improve   = -1;
    double ms_total          = 0;
    string termination       = "NONE";
    Search::Statistics stats;
    vector<tuple<int,double,double>> improvements; // (sol#, ms, cost scalaire)
    vector<tuple<int,double,double>> checkpoints;  // (sol#, ms, best_cost scalaire)
    // Évolution du vecteur de coûts lex (toutes les solutions BAB).
    // Chaque entrée = (sol#, ms, scalaire, vecteur_lex_stringifié)
    vector<tuple<int,double,double,string>> solutions_log;
    vector<int> solution;
};

// =============================================================
// Stop : timeout total + détection de stagnation
// =============================================================

class StagnationStop : public Search::Stop {
    Search::TimeStop maxStop;
    steady_clock::time_point lastImprovement;
    int stagnation_ms;
    bool stagnation_triggered = false;
    bool max_triggered = false;
public:
    StagnationStop(int max_ms, int stag_ms)
        : maxStop(max_ms), stagnation_ms(stag_ms),
          lastImprovement(steady_clock::now()) {}

    void notifyImprovement() { lastImprovement = steady_clock::now(); }

    bool stop(const Search::Statistics& s, const Search::Options& o) {
        if (maxStop.stop(s, o)) { max_triggered = true; return true; }
        if (stagnation_ms > 0) {
            auto el = duration_cast<milliseconds>(steady_clock::now() - lastImprovement).count();
            if (el > stagnation_ms) { stagnation_triggered = true; return true; }
        }
        return false;
    }
    string stopReason() const {
        if (stagnation_triggered) return "STAGNATION";
        if (max_triggered)        return "TIMEOUT_MAX";
        return "EXHAUSTIVE";
    }
};

// =============================================================
// Helpers
// =============================================================

static const int CHECKPOINT_INTERVAL = 1000;

static Species int_to_species(int sp) {
    switch (sp) {
        case 1: return FIRST_SPECIES;
        case 2: return SECOND_SPECIES;
        case 3: return THIRD_SPECIES;
        case 4: return FOURTH_SPECIES;
        case 5: return FIFTH_SPECIES;
        default: return THIRD_SPECIES;
    }
}

static string midi_to_french(int note) {
    static const string names[] = {"Do","Do#","Ré","Mib","Mi","Fa","Fa#","Sol","Lab","La","Sib","Si"};
    int oct = note / 12 - 1;
    return names[note % 12] + to_string(oct);
}

static const char* obj_mode_label(ObjectiveMode m) {
    switch (m) {
        case OBJECTIVE_TOTAL: return "total (somme globale)";
        case OBJECTIVE_MIXED: return "mixed (somme + score lex pond\u00e9r\u00e9)";
        default:              return "lex (lexicographique par priorit\u00e9)";
    }
}
static const char* obj_mode_short(ObjectiveMode m) {
    switch (m) {
        case OBJECTIVE_TOTAL: return "total";
        case OBJECTIVE_MIXED: return "mixed";
        default:              return "lex";
    }
}
static const char* cost_label(ObjectiveMode m) {
    switch (m) {
        case OBJECTIVE_TOTAL: return "Co\u00fbt total";
        case OBJECTIVE_MIXED: return "Score mixte";
        default:              return "Somme lex (indic.)";
    }
}

static ObjectiveMode parse_obj_mode(const string& s) {
    if (s == "total") return OBJECTIVE_TOTAL;
    if (s == "mixed") return OBJECTIVE_MIXED;
    return OBJECTIVE_LEX;
}

static string species_tag_for(int nb_voix, const vector<int>& sp_input) {
    string tag = to_string(nb_voix) + "voices_1";  // CF en première position
    for (int s : sp_input) tag += "_" + to_string(s);
    return tag;
}

static void mkdir_p(const string& path) {
    string cmd = "mkdir -p '" + path + "'";
    int r = system(cmd.c_str());
    (void)r;
}

// =============================================================
// Listing helpers
// =============================================================

static void cmd_list_cf(const map<int, CFEntry>& cfs) {
    cout << "Cantus firmus disponibles :" << endl;
    cout << "  ID  | Nom                  | Gamme indicative   | Description" << endl;
    cout << "  ----+----------------------+--------------------+-----------------------------" << endl;
    for (auto& kv : cfs) {
        cout << "  " << setw(3) << kv.first << " | "
             << setw(20) << left << kv.second.name << " | "
             << setw(18) << left << kv.second.scale << " | "
             << kv.second.description << endl;
    }
}

static void cmd_list_presets(const map<string, PresetEntry>& presets) {
    cout << "Presets disponibles :" << endl;
    for (auto& kv : presets) {
        cout << "  " << setw(15) << left << kv.first
             << " borrow=" << kv.second.borrow_mode
             << "  - " << kv.second.description << endl;
    }
}

static void cmd_list_campaigns(const vector<CampaignRow>& camps) {
    cout << "Campagnes disponibles :" << endl;
    map<string,int> counts;
    map<string,string> first_desc;
    for (auto& c : camps) {
        counts[c.campaign_id]++;
        if (first_desc.find(c.campaign_id) == first_desc.end()) {
            first_desc[c.campaign_id] = c.description;
        }
    }
    for (auto& kv : counts) {
        cout << "  " << setw(20) << left << kv.first
             << " (" << kv.second << " configs) - " << first_desc[kv.first] << endl;
    }
}

// =============================================================
// Construction d'un GenCase à partir des CSV + arguments
// =============================================================

static bool resolve_cf(GenCase& gc, const map<int, CFEntry>& cfs) {
    auto it = cfs.find(gc.cf_id);
    if (it == cfs.end()) {
        cerr << "CF inconnu : " << gc.cf_id << endl;
        return false;
    }
    gc.cf = it->second.notes;
    if (gc.cf.empty()) {
        cerr << "CF id=" << gc.cf_id << " : aucune note dans cantus_firmus.csv" << endl;
        return false;
    }
    gc.cf_name  = it->second.name;
    gc.cf_scale = it->second.scale;
    return true;
}

static bool apply_preset(GenCase& gc, const map<string, PresetEntry>& presets) {
    auto it = presets.find(gc.preset_name);
    if (it == presets.end()) {
        cerr << "Preset inconnu : " << gc.preset_name << endl;
        return false;
    }
    const auto& p = it->second;
    gc.melodic_params  = p.melodic_params;
    gc.general_params  = p.general_params;
    gc.specific_params = p.specific_params;
    gc.importance      = p.importance;
    gc.borrow_mode     = p.borrow_mode;
    return true;
}

// =============================================================
// Logging (txt + csv) — une seule génération
// =============================================================

static void write_txt_report(const string& path, const GenCase& gc,
                             const vector<int>& sp_input,
                             const BenchOutcome& bo) {
    ofstream r(path);
    r << "========================================\n"
      << "  RÉSULTATS : " << species_tag_for(gc.n_voices, sp_input) << "\n"
      << "========================================\n\n";
    r << "--- Configuration ---\n"
      << "CF                : " << gc.cf_id << " (" << gc.cf_name << ")\n"
      << "Gamme indicative  : " << gc.cf_scale << "\n"
      << "Preset            : " << gc.preset_name << "\n"
      << "Borrow mode       : " << gc.borrow_mode << "\n"
      << "Mode d'objectif   : " << obj_mode_label(gc.obj_mode) << "\n"
      << "Nombre de voix    : " << gc.n_voices << "\n"
      << "Espèces (incl CF) : 1";
    for (int s : sp_input) r << " " << s;
    r << "\nv_type            :";
    for (int v : gc.v_type) r << " " << v;
    r << "\nTimeout max (ms)  : " << gc.timeout_ms << "\n"
      << "Stagnation (ms)   : " << gc.stagnation_ms << "\n"
      << "Cantus firmus     :";
    for (int n : gc.cf) r << " " << n;
    r << "\n                   ";
    for (int n : gc.cf) r << " " << midi_to_french(n);
    r << "\n\n";

    r << "--- Métriques de recherche ---\n"
      << "Temps total (ms)            : " << fixed << setprecision(1) << bo.ms_total << "\n"
      << "Temps 1ère solution (ms)    : " << fixed << setprecision(1) << bo.ms_first_solution << "\n"
      << "Temps dernière amélio. (ms) : " << fixed << setprecision(1) << bo.ms_last_improve << "\n"
      << "Solutions trouvées          : " << bo.nb_solutions << "\n"
      << "Améliorations de coût       : " << bo.nb_improvements << "\n"
      << cost_label(gc.obj_mode) << "           : " << bo.best_cost << "\n"
      << "Terminaison                 : " << bo.termination;
    if (bo.termination == "EXHAUSTIVE") r << " (optimal)";
    r << "\n\n";

    r << "--- Statistiques Gecode ---\n"
      << "Nœuds      : " << bo.stats.node      << "\n"
      << "Échecs     : " << bo.stats.fail      << "\n"
      << "Restarts   : " << bo.stats.restart   << "\n"
      << "Propagat.  : " << bo.stats.propagate << "\n"
      << "Profondeur : " << bo.stats.depth     << "\n\n";

    if (!bo.improvements.empty()) {
        r << "--- Améliorations ---\n"
          << setw(8) << "Sol#" << setw(14) << "Temps (ms)" << setw(12) << "Coût" << "\n";
        for (auto& t : bo.improvements) {
            r << setw(8) << get<0>(t)
              << setw(14) << fixed << setprecision(1) << get<1>(t)
              << setw(12) << fixed << setprecision(1) << get<2>(t) << "\n";
        }
        r << "\n";
    }
    if (!bo.checkpoints.empty()) {
        r << "--- Checkpoints (" << CHECKPOINT_INTERVAL << " sol) ---\n"
          << setw(8) << "Sol#" << setw(14) << "Temps (ms)" << setw(14) << "Best cost" << "\n";
        for (auto& t : bo.checkpoints) {
            r << setw(8) << get<0>(t)
              << setw(14) << fixed << setprecision(1) << get<1>(t)
              << setw(14) << fixed << setprecision(1) << get<2>(t) << "\n";
        }
        r << "\n";
    }
    if (!bo.solutions_log.empty()) {
        r << "--- \u00c9volution du vecteur de co\u00fbts (toutes les solutions BAB) ---\n"
          << setw(8) << "Sol#" << setw(14) << "Temps (ms)"
          << setw(12) << "Cost" << "  Vecteur lex\n";
        for (auto& t : bo.solutions_log) {
            r << setw(8) << get<0>(t)
              << setw(14) << fixed << setprecision(1) << get<1>(t)
              << setw(12) << fixed << setprecision(1) << get<2>(t)
              << "  " << get<3>(t) << "\n";
        }
        r << "\n";
    }
    if (!bo.solution.empty()) {
        r << "--- Solution (notes MIDI) ---\n";
        for (int n : bo.solution) r << n << " ";
        r << "\n";
    }
    r.close();
}

static void write_csv_report(const string& path, const GenCase& gc,
                             const vector<int>& sp_input,
                             const BenchOutcome& bo) {
    ofstream c(path);
    c << "cf_id,cf_name,preset,borrow_mode,nb_voix,especes,v_type,"
         "timeout_ms,stagnation_ms,temps_total_ms,temps_premiere_solution_ms,"
         "temps_derniere_amelioration_ms,nb_solutions,nb_ameliorations,cout_final,"
         "terminaison,noeuds,echecs,redemarrages,propagations,profondeur_max\n";
    c << gc.cf_id << "," << gc.cf_name << "," << gc.preset_name << ","
      << gc.borrow_mode << "," << gc.n_voices << ",";
    c << "1";
    for (int s : sp_input) c << "-" << s;
    c << ",";
    for (size_t i = 0; i < gc.v_type.size(); ++i) {
        if (i > 0) c << "-";
        c << gc.v_type[i];
    }
    c << "," << gc.timeout_ms << "," << gc.stagnation_ms
      << "," << fixed << setprecision(1) << bo.ms_total
      << "," << fixed << setprecision(1) << bo.ms_first_solution
      << "," << fixed << setprecision(1) << bo.ms_last_improve
      << "," << bo.nb_solutions
      << "," << bo.nb_improvements
      << "," << fixed << setprecision(1) << bo.best_cost
      << "," << bo.termination
      << "," << bo.stats.node
      << "," << bo.stats.fail
      << "," << bo.stats.restart
      << "," << bo.stats.propagate
      << "," << bo.stats.depth << "\n";

    if (!bo.improvements.empty()) {
        c << "\n# Améliorations\nsolution_num,temps_ms,cout\n";
        for (auto& t : bo.improvements)
            c << get<0>(t) << "," << fixed << setprecision(1) << get<1>(t)
              << "," << fixed << setprecision(1) << get<2>(t) << "\n";
    }
    if (!bo.checkpoints.empty()) {
        c << "\n# Checkpoints\nsolution_num,temps_ms,meilleur_cout\n";
        for (auto& t : bo.checkpoints)
            c << get<0>(t) << "," << fixed << setprecision(1) << get<1>(t)
              << "," << fixed << setprecision(1) << get<2>(t) << "\n";
    }
    if (!bo.solutions_log.empty()) {
        c << "\n# Evolution couts\nsolution_num,temps_ms,cout_scalaire,vecteur_lex\n";
        for (auto& t : bo.solutions_log) {
            string lex = get<3>(t);
            // CSV-safe : remplacer virgules par points-virgules dans le vecteur lex
            for (char& ch : lex) if (ch == ',') ch = ';';
            c << get<0>(t) << "," << fixed << setprecision(1) << get<1>(t)
              << "," << fixed << setprecision(1) << get<2>(t)
              << ",\"" << lex << "\"\n";
        }
    }
    c.close();
}

static void write_error_txt(const string& path, const GenCase& gc,
                            const vector<int>& sp_input,
                            const BenchOutcome& bo) {
    ofstream e(path);
    e << "========================================\n"
      << "  AUCUNE SOLUTION : " << species_tag_for(gc.n_voices, sp_input) << "\n"
      << "========================================\n\n"
      << "CF             : " << gc.cf_id << " (" << gc.cf_name << ")\n"
      << "Preset         : " << gc.preset_name << "\n"
      << "Nb voix        : " << gc.n_voices << "\n"
      << "Espèces        : 1";
    for (int s : sp_input) e << " " << s;
    e << "\nTimeout (ms)   : " << gc.timeout_ms << "\n"
      << "Stagnation (ms): " << gc.stagnation_ms << "\n"
      << "Terminaison    : " << bo.termination << "\n"
      << "Temps écoulé   : " << fixed << setprecision(1) << bo.ms_total << " ms\n"
      << "Nœuds          : " << bo.stats.node << "\n"
      << "Échecs         : " << bo.stats.fail << "\n";
    e.close();
}

// =============================================================
// Bench BAB (équivalent generation_BAB_bench de Generations.cpp)
// =============================================================

static BenchOutcome run_bench(CounterpointProblem* problem, GenCase& gc) {
    BenchOutcome bo;
    StagnationStop stopObj(gc.timeout_ms, gc.stagnation_ms);
    Search::Options opt;
    opt.stop = &stopObj;

    auto t0 = steady_clock::now();
    auto ms_since = [&]() {
        return duration<double, milli>(steady_clock::now() - t0).count();
    };

    BAB<CounterpointProblem> e(problem, opt);
    CounterpointProblem* best = nullptr;

    while (CounterpointProblem* s = e.next()) {
        bo.nb_solutions++;
        double now = ms_since();
        double cost = s->getCost();
        string lex = intVarArgs_to_string(s->cost());

        // Trace live : on voit l'évolution des coûts en suivant le log.
        // En mode lex on affiche le vecteur complet (le plus parlant) ;
        // en mode total/mixed on affiche le scalaire optimisé directement.
        if (gc.obj_mode == OBJECTIVE_LEX) {
            cout << "  [Sol " << bo.nb_solutions
                 << " | t=" << fixed << setprecision(0) << now << "ms"
                 << " | somme=" << fixed << setprecision(1) << cost
                 << " | lex=" << lex << "]" << endl;
        } else {
            cout << "  [Sol " << bo.nb_solutions
                 << " | t=" << fixed << setprecision(0) << now << "ms"
                 << " | " << cost_label(gc.obj_mode) << "="
                 << fixed << setprecision(1) << cost << "]" << endl;
        }
        bo.solutions_log.emplace_back(bo.nb_solutions, now, cost, lex);

        if (bo.nb_solutions == 1) bo.ms_first_solution = now;

        if (cost < bo.best_cost) {
            bo.nb_improvements++;
            delete best;
            best = s;
            bo.best_cost = cost;
            bo.ms_last_improve = now;
            bo.improvements.emplace_back(bo.nb_solutions, now, cost);
            stopObj.notifyImprovement();
        } else {
            delete s;
        }
        if (bo.nb_solutions % CHECKPOINT_INTERVAL == 0) {
            bo.checkpoints.emplace_back(bo.nb_solutions, now, bo.best_cost);
        }
    }

    bool stopped = e.stopped();
    bo.termination = stopped ? stopObj.stopReason() : "EXHAUSTIVE";
    bo.stats = e.statistics();
    bo.ms_total = ms_since();

    if (best) {
        IntVarArray sa = best->getSolutionArray();
        bo.solution.reserve(sa.size());
        for (int i = 0; i < sa.size(); ++i) bo.solution.push_back(sa[i].val());
        delete best;
    }
    return bo;
}

// =============================================================
// Exécution d'un GenCase complet (problème + bench + sauvegarde)
// =============================================================

static int execute_gen_case(GenCase& gc, const vector<int>& sp_input) {
    string species_tag = species_tag_for(gc.n_voices, sp_input);
    string base_dir = gc.output_root;
    if (!gc.output_subdir.empty()) base_dir += "/" + gc.output_subdir;
    base_dir += "/" + to_string(gc.n_voices) + "voices/cf" + to_string(gc.cf_id);
    string txt_dir  = base_dir + "/txt";
    string csv_dir  = base_dir + "/csv";
    string midi_dir = base_dir + "/midi";
    mkdir_p(txt_dir); mkdir_p(csv_dir); mkdir_p(midi_dir);

    // Description du CF (créée 1 seule fois)
    string cf_desc = base_dir + "/cantus_firmus.txt";
    if (!ifstream(cf_desc).good()) {
        ofstream f(cf_desc);
        for (size_t i = 0; i < gc.cf.size(); ++i) { if (i) f << " "; f << gc.cf[i]; }
        f << "\n";
        for (size_t i = 0; i < gc.cf.size(); ++i) { if (i) f << " "; f << midi_to_french(gc.cf[i]); }
        f << "\n" << gc.cf_name << " (gamme indicative : " << gc.cf_scale << ")\n";
    }

    cout << "\n=== Génération : cf" << gc.cf_id << " (" << gc.cf_name << ") | "
         << species_tag << " | preset=" << gc.preset_name
         << " | obj=" << obj_mode_short(gc.obj_mode)
         << " | timeout=" << gc.timeout_ms << "ms | stagn=" << gc.stagnation_ms << "ms ===" << endl;

    // Activation des contraintes musicales (par défaut toutes actives)
    fill(activeConstraints.begin(), activeConstraints.end(), true);

    // Création du problème via create_problem (méthode unifiée, cf Generations.cpp)
    CounterpointProblem* problem = create_problem(
        gc.cf, gc.spList, gc.v_type,
        gc.melodic_params, gc.general_params, gc.specific_params,
        gc.importance, gc.borrow_mode, gc.obj_mode);

    BenchOutcome bo = run_bench(problem, gc);
    delete problem;

    if (!bo.solution.empty()) {
        // Découpage des voix pour MIDI
        vector<pair<vector<int>, Species>> voices;
        int offset = 0;
        for (int v = 0; v < (int)gc.spList.size(); ++v) {
            int sz = branchingNotesSize(gc.spList[v], gc.cf.size());
            vector<int> notes(bo.solution.begin() + offset,
                              bo.solution.begin() + offset + sz);
            voices.emplace_back(notes, gc.spList[v]);
            offset += sz;
        }
        string midi_path = midi_dir + "/" + species_tag + ".mid";
        saveMidiMultiVoice(midi_path, gc.cf, voices);

        write_txt_report(txt_dir + "/" + species_tag + ".txt", gc, sp_input, bo);
        write_csv_report(csv_dir + "/" + species_tag + ".csv", gc, sp_input, bo);

        cout << "  -> Solutions=" << bo.nb_solutions
             << " Améliorations=" << bo.nb_improvements
             << " " << cost_label(gc.obj_mode) << "=" << bo.best_cost
             << " Temps=" << fixed << setprecision(0) << bo.ms_total << "ms"
             << " (" << bo.termination << ")" << endl;
    } else {
        write_error_txt(txt_dir + "/error_" + species_tag + ".txt", gc, sp_input, bo);
        cout << "  -> AUCUNE SOLUTION (" << bo.termination << ", "
             << fixed << setprecision(0) << bo.ms_total << " ms)" << endl;
    }
    return 0;
}

// =============================================================
// CLI
// =============================================================

struct CliArgs {
    int    nb_voix       = 0;
    vector<int> sp_input;          // espèces des voix de contrepoint
    int    cf_id         = 1;
    string preset_name   = "default";
    int    timeout_ms    = 300000;
    int    stagnation_ms = 120000;
    string output_root   = "../../results";
    string output_subdir = "";
    ObjectiveMode obj_mode = OBJECTIVE_LEX;
    vector<int> v_types_override;

    string campaign_id;
    bool list_cf = false, list_presets = false, list_campaigns = false;
};

static void print_usage() {
    cout << "Usage : ./GenerateCounterpoint <nb_voix> <especes...> [options]\n"
         << "        ./GenerateCounterpoint --campaign <id>\n"
         << "        ./GenerateCounterpoint --list-cf | --list-presets | --list-campaigns\n\n"
         << "Options :\n"
         << "  -c CF_ID            id du cantus firmus (cf cantus_firmus.csv)\n"
         << "  --preset NAME       preset de paramètres (cf presets.csv)\n"
         << "  -t timeout_ms       timeout total maximum\n"
         << "  -s stagnation_ms    arrêt si pas d'amélioration depuis N ms (0 = off)\n"
         << "  -o output_root      racine des résultats (défaut ../results)\n"
         << "  --subdir NAME       sous-dossier sous output_root\n"
         << "  -m lex|total|mixed  mode d'objectif\n"
         << "  -v vt1,vt2,...      v_types des voix de contrepoint\n";
}

static bool parse_cli(int argc, char* argv[], CliArgs& a) {
    int i = 1;
    if (i < argc) {
        string s = argv[i];
        if (s == "--list-cf")        { a.list_cf        = true; return true; }
        if (s == "--list-presets")   { a.list_presets   = true; return true; }
        if (s == "--list-campaigns") { a.list_campaigns = true; return true; }
        if (s == "--campaign" && i + 1 < argc) { a.campaign_id = argv[i+1]; return true; }
    }
    if (argc < 3) { print_usage(); return false; }
    a.nb_voix = atoi(argv[1]);
    if (a.nb_voix < 2 || a.nb_voix > 4) {
        cerr << "nb_voix doit être 2, 3 ou 4." << endl;
        return false;
    }
    int n_sp = a.nb_voix - 1;
    if (argc < 2 + n_sp) {
        cerr << "Il faut " << n_sp << " espèces (hors CF)." << endl;
        return false;
    }
    for (int k = 0; k < n_sp; ++k) {
        int sp = atoi(argv[2 + k]);
        if (sp < 1 || sp > 5) { cerr << "Espèce invalide : " << sp << endl; return false; }
        a.sp_input.push_back(sp);
    }
    int j = 2 + n_sp;
    while (j < argc) {
        string s = argv[j];
        auto need = [&](int extra) { return j + extra < argc; };
        if      (s == "-c" && need(1))        { a.cf_id = atoi(argv[++j]); }
        else if (s == "--preset" && need(1))  { a.preset_name = argv[++j]; }
        else if (s == "-t" && need(1))        { a.timeout_ms = atoi(argv[++j]); }
        else if (s == "-s" && need(1))        { a.stagnation_ms = atoi(argv[++j]); }
        else if (s == "-o" && need(1))        { a.output_root = argv[++j]; }
        else if (s == "--subdir" && need(1))  { a.output_subdir = argv[++j]; }
        else if (s == "-m" && need(1))        { a.obj_mode = parse_obj_mode(argv[++j]); }
        else if (s == "-v" && need(1)) {
            string v = argv[++j];
            for (auto& part : ConfigLoader::split(v, ',')) {
                string p = ConfigLoader::trim(part);
                if (!p.empty()) a.v_types_override.push_back(atoi(p.c_str()));
            }
        }
        else { cerr << "Argument inconnu : " << s << endl; return false; }
        ++j;
    }
    return true;
}

static GenCase build_gc_from_cli(const CliArgs& a) {
    GenCase gc;
    gc.cf_id          = a.cf_id;
    gc.n_voices       = a.nb_voix;
    gc.preset_name    = a.preset_name;
    gc.timeout_ms     = a.timeout_ms;
    gc.stagnation_ms  = a.stagnation_ms;
    gc.output_root    = a.output_root;
    gc.output_subdir  = a.output_subdir;
    gc.obj_mode       = a.obj_mode;
    for (int s : a.sp_input) gc.spList.push_back(int_to_species(s));
    if (!a.v_types_override.empty()) gc.v_type = a.v_types_override;
    else gc.v_type = vector<int>(a.nb_voix - 1, 0);
    return gc;
}

static GenCase build_gc_from_campaign(const CampaignRow& row, int cf_id, const string& output_root,
                                      ObjectiveMode obj_override) {
    GenCase gc;
    gc.cf_id          = cf_id;
    gc.n_voices       = row.n_voices;
    gc.preset_name    = row.preset.empty() ? "default" : row.preset;
    gc.timeout_ms     = row.timeout_ms;
    gc.stagnation_ms  = row.stagnation_ms;
    gc.output_root    = output_root;
    gc.output_subdir  = row.output_subdir;
    gc.obj_mode       = obj_override;
    for (int s : row.species) gc.spList.push_back(int_to_species(s));
    if (!row.v_types.empty()) gc.v_type = row.v_types;
    else gc.v_type = vector<int>(row.n_voices - 1, 0);
    return gc;
}

// =============================================================
// main
// =============================================================

int main(int argc, char* argv[]) {
    string config_dir = ConfigLoader::find_config_dir();
    auto cfs      = ConfigLoader::load_cantus_firmus(config_dir);
    auto presets  = ConfigLoader::load_presets(config_dir);
    auto camps    = ConfigLoader::load_campaigns(config_dir);

    CliArgs a;
    if (!parse_cli(argc, argv, a)) return 1;

    if (a.list_cf)        { cmd_list_cf(cfs); return 0; }
    if (a.list_presets)   { cmd_list_presets(presets); return 0; }
    if (a.list_campaigns) { cmd_list_campaigns(camps); return 0; }

    // === Mode campagne ===
    if (!a.campaign_id.empty()) {
        vector<CampaignRow> selected;
        for (auto& c : camps) if (c.campaign_id == a.campaign_id) selected.push_back(c);
        if (selected.empty()) {
            cerr << "Aucune ligne pour la campagne : " << a.campaign_id << endl;
            return 1;
        }
        cout << "=== Campagne " << a.campaign_id << " : " << selected.size()
             << " configs × CF associés ===" << endl;
        int total = 0, done = 0;
        for (auto& row : selected) total += row.cf_ids.size();
        for (auto& row : selected) {
            for (int cf_id : row.cf_ids) {
                done++;
                cout << "\n[" << done << "/" << total << "] " << row.description << endl;
                GenCase gc = build_gc_from_campaign(row, cf_id, a.output_root, a.obj_mode);
                if (!resolve_cf(gc, cfs))      { cerr << "  CF non résolu, skip." << endl; continue; }
                if (!apply_preset(gc, presets)){ cerr << "  Preset non résolu, skip." << endl; continue; }
                execute_gen_case(gc, row.species);
            }
        }
        cout << "\n=== Campagne terminée (" << done << "/" << total << ") ===" << endl;
        return 0;
    }

    // === Mode unique ===
    GenCase gc = build_gc_from_cli(a);
    if (!resolve_cf(gc, cfs))       return 1;
    if (!apply_preset(gc, presets)) return 1;
    return execute_gen_case(gc, a.sp_input);
}
