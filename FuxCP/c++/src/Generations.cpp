// 
// Created by Bryce Burignat.
// This file contains the generation framework implementation.  
// It is based on the testing framework :
//      - 'make compile_and_gen' automatically compiles, then run './Main gen'
//      - additionnal arguments can be added to specify the generation 
// 

#include <iostream>
#include <fstream>  // For file operations
#include <cmath>
#include <chrono>
#include <signal.h> // For testing blocking constrains in generations
#include <gecode/int.hh> // Ensure you include the necessary Gecode headers
#include <gecode/search.hh>
#include "../headers/Generations.hpp"
#include "../headers/Utilities.hpp"

using namespace chrono;

Generations::Generations(char* gen){
    cantusFirmus = {60, 62, 65, 64, 67, 65, 64, 62, 60};
    cfSize = cantusFirmus.size();
    melodic_params = {0, 1, 2, 576, 5, 10, 25, 40}; // Last ones augmented to avoid crazy leaps
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    borrowMode = 0; // Allow notes not in the current mode
    log_folder = "log/";
    midi_folder = "midi/";
    cf_name = "firstTestExample"; // TODO : change by name + mode + borrowMode if  (ex: DO_major_borrowed of FA_melodic_minor)

    // Default state : only strict constrains !
    fill(activeConstraints.begin(), activeConstraints.end(), true);
    fill(softConstraints.begin(), softConstraints.end(), false);
    
    if(strcmp(gen, "gen") == 0){
        gen_default();
    } else if(strcmp(gen, "bryce") == 0){
        gen_bryce();
    }
    else {
        std::cout << "Test for constraint not found : " << gen << std::endl;
        throw invalid_argument("Test for constraint not found !");
    }
}

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

vector<int> Generations::getCf(){
    return cantusFirmus;
}

int Generations::getBMode(){
    return borrowMode;
}

vector<int> Generations::getCp(){
    return cp;
}

int Generations::getIdx(){
    return idx;
}

void Generations::reset_cf_name(bool undefined=false){
    if (undefined) { cf_name = "undefined"; }
    else { cf_name = "firstTestExample"; }
}

//============================== Initialisation functions ================================
// Default data for GenerationCase, BenchResult, MusicalStats and spList

static GenerationCase generationCase(){
    GenerationCase gen_case;

    gen_case.spList = {};
    gen_case.v_type = {};
    gen_case.n_voices = 2;
    gen_case.timeout_ms = 600000; // 10min
    gen_case.stagnation_ms = 120000; // 2min

    gen_case.multiple_vtypes = false;
    gen_case.v_types = {};
    gen_case.v_types_1sp = {};

    gen_case.checkpoints = {1, 2, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95, 100,
                            150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000,
                            1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1650, 1700, 1750, 1800, 1850, 1900, 1950, 2000,
                            2050, 2100, 2150, 2200, 2250, 2300, 2350, 2400, 2450, 2500, 2550, 2600, 2650, 2700, 2750, 2800, 2850, 2900, 2950, 3000,
                            3100, 3200, 3300, 3400, 3500, 3600, 3700, 3800, 3900, 4000, 4100, 4200, 4300, 4400, 4500, 4600, 4700, 4800, 4900, 5000,
                            5100, 5200, 5300, 5400, 5500, 5600, 5700, 5800, 5900, 6000, 6100, 6200, 6300, 6400, 6500, 6600, 6700, 6800, 6900, 7000,
                            7100, 7200, 7300, 7400, 7500, 7600, 7700, 7800, 7900, 8000, 8100, 8200, 8300, 8400, 8500, 8600, 8700, 8800, 8900, 9000,
                            9100, 9200, 9300, 9400, 9500, 9600, 9700, 9800, 9900, 10000,
                            11000, 12000, 13000, 14000, 15000, 16000, 17000, 18000, 19000, 20000,
                            22000, 24000, 26000, 28000, 30000, 32000, 34000, 36000, 38000, 40000, 42000, 44000, 46000, 48000, 50000, 52000, 54000,
                            56000, 58000, 60000, 62000, 64000, 66000, 68000, 70000, 72000, 74000, 76000, 78000, 80000, 82000, 84000, 86000, 88000,
                            90000, 92000, 94000, 96000, 98000, 100000};
    gen_case.log_file_path = "default_logs.csv"; // filename for the moment (default_log)
    gen_case.stats_file_path = "default_stats.csv"; // filename for the moment (default_stats)

    return gen_case;
}

static vector<Species> classic_spList(Species species, int n_voices){
    if (n_voices == 2){
        return {species};
    } else if (n_voices == 3){
        return {FIRST_SPECIES, species};
    } else {
        return {FIRST_SPECIES, FIRST_SPECIES, species};
    }
}

/*
 *   Generator for classical counterpoints : definition for multiple vtype cases.
 */
static GenerationCase generate_classic_gen_case(Species species,
                                         int n_voices,
                                         const vector<int>& v_types,
                                         const vector<int>& v_types_1sp = {},
                                         int timeout_ms = 600000){
    // Creating struct
    GenerationCase gen_case = generationCase();
    gen_case.n_voices = n_voices;
    gen_case.v_types = v_types;
    gen_case.v_types_1sp = v_types_1sp;
    gen_case.timeout_ms = timeout_ms;
    gen_case.multiple_vtypes = true;
    gen_case.spList = classic_spList(species, n_voices);

    return gen_case;
}

/*
 *   Generator for classical counterpoints : normal definition.
 */
static GenerationCase generate_classic_gen_case(Species species,
                                         int n_voices,
                                         vector<int> v_type,
                                         int timeout_ms = 600000){
    // Creating struct
    GenerationCase gen_case = generationCase();
    gen_case.n_voices = n_voices;
    gen_case.spList = classic_spList(species, n_voices);
    gen_case.v_type = v_type;
    gen_case.timeout_ms = timeout_ms;

    return gen_case;
}

/*
 *   Generator for counterpoints : normal definition.
 */
static GenerationCase generate_general_gen_case(vector<Species> spList,
                                         vector<int> v_type,
                                         int timeout_ms = 600000,
                                         string log_file_path="",
                                         string stats_file_path=""){
    // Creating struct
    GenerationCase gen_case = generationCase();
    gen_case.n_voices = spList.size()+1;
    gen_case.spList = spList;
    gen_case.v_type = v_type;
    gen_case.timeout_ms = timeout_ms;
    if (log_file_path != "") { gen_case.log_file_path = log_file_path; }
    if (stats_file_path != "") { gen_case.stats_file_path = stats_file_path; }
    
    return gen_case;
}


static BenchResult benchResult(){
    BenchResult br;
    br.solutions = 0;
    br.termination = "None";
    br.ms_first = -1.0;
    br.ms_total = -1.0;
    br.checkpoint_times = {};
    br.best_cost = "";
    br.solution = {};
    return br;
}

//============================== Static util functions ================================

/**
 * Express vector of integers as a string 
 * @param v the vector containing the ints to join
 * @param sep the character used as separator between ints
 * @return string
 */
static string join_ints(const vector<int>& v, char sep=' ') {
    ostringstream oss;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) oss << sep;
        oss << v[i];
    }
    return oss.str();
}

/**
 * Add escape char ('\') where needed
 * @param s the string to adapt
 * @return string
 */
static string csv_escape(const string& s) {
    string out = "\"";
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out += c;
    }
    out += "\"";
    return out;
}

/**
 * Log all data from generations in a csv, in the log folder.
 * @param log_file_path the local path to the file
 * @param gen_case the generation data
 * @param bench the benchmarking data
 * @param music the anaytic data computed on the result
 * @param midi_file_path the path to the midi file (to be logged)
 */
static void log_gen_data_in_csv(
    const vector<int> cantusFirmus,
    const string& cf_name,
    const GenerationCase& gen_case,
    const BenchResult& bench
) {
    const bool file_exists = static_cast<bool>(ifstream(gen_case.log_file_path));
    ofstream out(gen_case.log_file_path, ios::app);

    if (!file_exists) {
        out << "timestamp,cf_name,cf_notes,n_voices,sp_list,v_type,timeout_ms,stagnation_ms,termination,"
               "solutions,first_ms,total_ms,best_cost,complete_solution,"
               "explored_nodes,fails,restarts,propagations,max_depth,"
               "midi_file\n";
    }

    auto now = chrono::system_clock::now();
    auto now_t = chrono::system_clock::to_time_t(now);

    Search::Statistics stats = bench.gecodeStats;
    out << csv_escape(string(ctime(&now_t)).substr(0, 24)) << ","
        << csv_escape(cf_name) << ","
        << csv_escape(join_ints(cantusFirmus)) << ","
        << gen_case.n_voices << ","
        << csv_escape(join_ints(vector<int>(gen_case.spList.begin(), gen_case.spList.end()))) << ","
        << csv_escape(join_ints(gen_case.v_type)) << ","
        << gen_case.timeout_ms << ","
        << gen_case.stagnation_ms << ","
        << bench.termination << ","
        << bench.solutions << ","
        << bench.ms_first << ","
        << bench.ms_total << ","
        << csv_escape(bench.best_cost) << ","
        << csv_escape(int_var_array_to_string(bench.solution)) << ","
        << stats.node << ","
        << stats.fail << ","
        << stats.restart << ","
        << stats.propagate << ","
        << stats.depth << ",";
    

    out << csv_escape(gen_case.midi_file_path) << "\n" << endl;
}

/**
 * log the current costs
 * @param notes the notes of the voice
 * @return MusicalStats object
 */
static void log_intermediate_data_in_csv(string stat_file_path, string filename, int iteration, int duration, string costs, IntVarArray solution) {
    const bool file_exists = static_cast<bool>(ifstream(stat_file_path));
    ofstream out(stat_file_path, ios::app);

    if (!file_exists) {
        out << "timestamp,iteration,duration(ms),costs,solution\n";
    }

    auto now = chrono::system_clock::now();
    auto now_t = chrono::system_clock::to_time_t(now);

    string timestamp = csv_escape(string(ctime(&now_t)).substr(0, 24));
    //replace(timestamp.begin(), timestamp.end(), " ", "_");
    out << timestamp << ","
        << filename << ","
        << iteration << ","
        << duration << ","
        << csv_escape(costs) << ","
        << csv_escape(int_var_array_to_string(solution));
    
    out << "\n";
}

/**
 * Log all data from generations in the console
 * @param bench the benchmarking data
 * @param music the anaytic data computed on the result
 * @param midi_file_path the path to the midi file (to be logged)
 * @param log_file_path the local path to the file
 */
static void log_gen_data_in_console(
    const BenchResult& bench,
    const string& midi_file_path,
    const string& log_file_path){

    if (bench.solutions > 0) {
        cout
            << "termination        : " << bench.termination;
            if (bench.termination == "EXHAUSTIVE") cout << " (BEST)";
        cout
            << "\n"
            << "solutions     : " << bench.solutions << "\n"
            << "counterpoints : " << bench.solution << "\n"
            << "first_ms      : " << bench.ms_first << "\n"
            << "total_ms      : " << bench.ms_total << "\n"
            << "best_cost     : " << bench.best_cost << "\n";

        if (!bench.checkpoint_times.empty()) {
            cout << "checkpoint_ms : ";
            for (const auto& pair_kms : bench.checkpoint_times) {
                cout << pair_kms.first << "->" << pair_kms.second << "  ";
            }
            cout << "\n";
        }
    } else {
        cout
            << "termination        : " << bench.termination;
            if (bench.termination == "EXHAUSTIVE") cout << " (BEST)";
        cout
            << "\n"
            << "solutions     : 0\n"
            << "total_ms      : " << bench.ms_total << "\n"
            << "log          : " << log_file_path << "\n\n";
    }

    Search::Statistics stats = bench.gecodeStats;
    cout << "--- Statistiques Gecode ---" << endl;
    cout << "Nœuds explorés : " << stats.node << endl;
    cout << "Échecs : " << stats.fail << endl;
    cout << "Redémarrages : " << stats.restart << endl;
    cout << "Propagations : " << stats.propagate << endl;
    cout << "Profondeur max : " << stats.depth << endl;
    cout << endl;
}

static string generate_descriptive_filename(GenerationCase gen_case, string cf_name){
    string file_path = cf_name + "_"
        + to_string(gen_case.n_voices) + "v_";
    for (int sp : gen_case.spList) {
        file_path += to_string(sp+1) + "-";
    }
    file_path +="sp_vtype_";
    for (int vt : gen_case.v_type) {
        file_path += to_string(vt) + "_";
    }
    return file_path;
}

/**
 * Compute time since a given time point 
 * @param t0 the reference time
 * @return duration (in ms)
 */
static inline double ms_since(chrono::steady_clock::time_point t0) {
  auto now = chrono::steady_clock::now();
  return chrono::duration<double, milli>(now - t0).count();
}

//=======================================================================================
//============================== COUNTERPOINT GENERATION ================================

void Generations::generate_counterpoints(GenerationCase gen_case, bool midi=true, bool log=true, bool stats=false){
    // v_types has to be specified
    if (gen_case.multiple_vtypes){
        if (gen_case.v_types.empty() || gen_case.n_voices > 2 && gen_case.v_types_1sp.empty()){
            cout << "ERROR : v_types not specified !" << endl;
        }
    } else {
        if (gen_case.v_type.empty()) {
            cout << "ERROR : v_type not specified !" << endl;
        }
    }

    // Params
    Species species = gen_case.spList.back();
    int n_voices = gen_case.n_voices;
    cfSize = cantusFirmus.size();
    if (log){
        gen_case.log_file_path = log_folder + gen_case.log_file_path;
    } else {
        gen_case.log_file_path = "None";
    }

    cout << "===== generate counterpoints " << n_voices << "v " << species+1 << "sp " << " =====" << endl;

    fill(activeConstraints.begin(), activeConstraints.end(), true);
    if (gen_case.multiple_vtypes){
        int n_types_4v = 1;
        int n_types_3v = 1;
        if (n_voices >= 3) {
            n_types_3v = gen_case.v_types_1sp.size();
        }
        if (n_voices >= 4) {
            n_types_4v = gen_case.v_types_1sp.size();
        }

        for (int vidx_1sp_2 = 0; vidx_1sp_2 < n_types_4v; vidx_1sp_2++){
            for (int vidx_1sp_1 = 0; vidx_1sp_1 < n_types_3v; vidx_1sp_1++){
                for (int vidx_species = 0; vidx_species < gen_case.v_types.size(); vidx_species++){
                    switch(n_voices) {
                        case 4:
                            gen_case.v_type = {gen_case.v_types_1sp.at(vidx_1sp_2), gen_case.v_types_1sp.at(vidx_1sp_1), gen_case.v_types.at(vidx_species)};
                            break;
                        case 3:
                            gen_case.v_type = {gen_case.v_types_1sp.at(vidx_1sp_1), gen_case.v_types.at(vidx_species)};
                            break;
                        default: // 2 voices
                            gen_case.v_type = {gen_case.v_types.at(vidx_species)};
                            break;;
                    }
                    
                    auto* problem = create_problem(cantusFirmus, gen_case.spList, gen_case.v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                    generation_BAB_bench(problem, gen_case, midi, log, stats);
                    delete problem;
                }
            }
        }
    }
    else {
        auto* problem = create_problem(cantusFirmus, gen_case.spList, gen_case.v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        generation_BAB_bench(problem, gen_case, midi, log, stats);
        delete problem;
    }
    
}

void Generations::generation_BAB_bench(CounterpointProblem* problem, GenerationCase& gen_case, bool midi, bool log, bool stats){
    StagnationStop stopObj(gen_case.timeout_ms, gen_case.stagnation_ms);
    Search::Options opt;
    opt.stop = &stopObj;

    auto t0 = chrono::steady_clock::now();
    auto ms_since_start = [&]() {
        auto now = chrono::steady_clock::now();
        return chrono::duration<double, milli>(now - t0).count();
    };

    BenchResult bench = benchResult();
    size_t next_checkpoint = 0;
    string descriptive_filename = generate_descriptive_filename(gen_case, cf_name);
    gen_case.stats_file_path = log_folder + gen_case.stats_file_path;

    BAB<CounterpointProblem> e(problem, opt);
    CounterpointProblem* best = nullptr;
    double current_ms;
    double previous_ms = 0;
    while (CounterpointProblem* s = e.next()) {
        current_ms = ms_since_start();

        bench.solutions++;
        delete best; // keep only the best-so-far
        best = s;

        if (bench.solutions == 1) {
            bench.ms_first = current_ms;
        }

        string current_cost = intVarArgs_to_string(best->cost());
        if (current_cost != bench.best_cost) {
            bench.best_cost = current_cost;
        }

        if (next_checkpoint < gen_case.checkpoints.size() && bench.solutions == gen_case.checkpoints[next_checkpoint]) {
            bench.checkpoint_times.push_back({bench.solutions, current_ms});

            IntVarArray solution = s->getSolutionArray();
            cout << "=== Iteration " << bench.solutions << " : " << solution << endl;
            if (stats){ 
                log_intermediate_data_in_csv(gen_case.stats_file_path, descriptive_filename, bench.solutions, current_ms, current_cost, solution);
            }
            next_checkpoint++;
        }

        previous_ms = current_ms;
    }

    bench.ms_total = ms_since_start();
    bool stopped = e.stopped();
    bool timed_out = stopped; // compatibilité
    bench.termination = stopped ? stopObj.stopReason() : "EXHAUSTIVE";

    bench.gecodeStats = e.statistics();
    double total_ms = ms_since_start();

    Species species = gen_case.spList.back();

    if (best) { // there is a solution !
        size_t cfSize = cantusFirmus.size();
        bench.solution = best->getSolutionArray();

        // Part to change when saveMidi can handle 3+ voices
        auto voice_notes = extract_notes(best, gen_case.spList, cfSize);

        if (midi){
            gen_case.midi_file_path = midi_folder + descriptive_filename + ".mid";
            saveMidiGeneral(gen_case.midi_file_path, cantusFirmus, voice_notes, gen_case.spList);
        }
        
        delete best;
    }
    else {
        gen_case.midi_file_path = "None";
    }

    if (log){
        log_gen_data_in_csv(cantusFirmus, cf_name, gen_case, bench);
    }
    log_gen_data_in_console(bench, gen_case.midi_file_path, gen_case.log_file_path);
}

//=======================================================================================
//=======================================================================================

/*
 * Generic tests for classic counterpoints
 * (default timeout 720000ms = 12min)
 */
void Generations::all_classic_counterpoints(int timeout_ms = 720000, bool midi = true, bool log = true, bool stats = false){
    cf_name += "_classic";
    
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 2, {1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 2, {0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 2, {-1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 2, {1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 2, {0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 2, {-1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 2, {1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 2, {0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 2, {-1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    // generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 2, {1}, timeout_ms), midi, log, stats);   // No result
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 2, {0}, timeout_ms), midi, log, stats);
    // generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 2, {-1}, timeout_ms), midi, log, stats);  // No result
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 2, {1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 2, {0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 2, {-1}, timeout_ms), midi, log, stats);
    
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 3, {4, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 3, {0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 3, {2, 0}, timeout_ms), midi, log, stats);     
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 3, {2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 3, {-2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 3, {4, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 3, {0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 3, {2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 3, {2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 3, {-2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 3, {4, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 3, {0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 3, {2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 3, {2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 3, {-2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 3, {4, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 3, {0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 3, {2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 3, {2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 3, {-2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 3, {4, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 3, {0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 3, {2, 0}, timeout_ms), midi, log, stats);
    // generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 3, {2, -1}, timeout_ms), midi, log, stats);    // No result
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 3, {-2, 0}, timeout_ms), midi, log, stats);
    
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {-2, 2, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {-1, 2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {-2, 1, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {0, 2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {-2, 0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {-1, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {-2, -1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {-2, 2, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {-1, 2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {-2, 1, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {0, 2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {-2, 0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {-1, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {-2, -1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {-2, 2, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {-1, 2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {-2, 1, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {0, 2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {-2, 0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {-1, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {-2, -1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {-2, 2, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {-1, 2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {-2, 1, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {0, 2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {-2, 0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {-1, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {-2, -1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-2, 2, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-1, 2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-2, 1, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {0, 2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-2, 0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-1, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-2, -1, 2}, timeout_ms), midi, log, stats);
}

/*
 * First batch of generic tests for multi-species counterpoints
 * (default timeout 720000ms = 12min)
 */
void Generations::multi_species_counterpoints_batch1(int timeout_ms = 720000, bool midi = true, bool log = true, bool stats = false){ // Generic tests
    cf_name += "_multi_Batch1";


    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, SECOND_SPECIES}, {1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES}, {0, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, THIRD_SPECIES}, {0, 2}, timeout_ms), midi, log, stats);
    //generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, THIRD_SPECIES}, {1, 0}, timeout_ms), midi, log, stats);
    //generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, THIRD_SPECIES}, {-1, 2}, timeout_ms), midi, log, stats);
    //generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, THIRD_SPECIES}, {-1, -2}, timeout_ms), midi, log, stats);                       // Best ?
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES}, {0, 1, 2}, timeout_ms), midi, log, stats);

    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES}, {1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES}, {0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES}, {2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, FIFTH_SPECIES}, {2, 0}, timeout_ms), midi, log, stats);

    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FOURTH_SPECIES}, {2, 1, 0}, timeout_ms), midi, log, stats);

    // No results
    /*
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, THIRD_SPECIES}, {1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, FIFTH_SPECIES}, {0, 2}, timeout_ms), midi, log, stats);

    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FOURTH_SPECIES}, {2, 2, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FOURTH_SPECIES}, {2, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FOURTH_SPECIES}, {1, 2, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FIFTH_SPECIES}, {0, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FIFTH_SPECIES}, {2, 0, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FIFTH_SPECIES}, {1, 2, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, FOURTH_SPECIES, FIFTH_SPECIES}, {0, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, FOURTH_SPECIES, FIFTH_SPECIES}, {2, 0, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, FOURTH_SPECIES, FIFTH_SPECIES}, {1, 2, 0}, timeout_ms), midi, log, stats);
    
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES}, {1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES}, {-2, 2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES}, {2, -2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES}, {0, 2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES}, {0, -2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES}, {0, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES}, {0, -2, -2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES}, {0, -1, -2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES}, {-1, -2, -2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES}, {-3, -1, -1}, timeout_ms, log_file, stats_file), midi, log, stats);
    
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES}, {1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES}, {0, -2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES}, {-1, -1}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES}, {0, 2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES}, {1, -1}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES, FIFTH_SPECIES}, {0, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES, FIFTH_SPECIES}, {0, -2, -2}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES, FIFTH_SPECIES}, {0, -1, -1}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES, FIFTH_SPECIES}, {-2, -1, -1}, timeout_ms, log_file, stats_file), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({FIFTH_SPECIES, FIFTH_SPECIES, FIFTH_SPECIES}, {-1, -1, 2}, timeout_ms, log_file, stats_file), midi, log, stats);
    */
}

/*
 * Reference tests for counterpoints (v_types chosen by occurenxes in fux's examples + faisability)
 * (default timeout 720000ms = 12min)
 */
void Generations::reference_counterpoints_for_testing(int timeout_ms = 720000, bool midi = true, bool log = true, bool stats = false){
    midi_folder = "midi_bryce/";
    cf_name += "_ref";
    
    /*generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 2, {1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 2, {-1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 2, {-1}, timeout_ms), midi, log, stats); // Could be {0}
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 2, {-1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 2, {-1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 2, {2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 2, {0}, timeout_ms), midi, log, stats);*/

    /*generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 3, {1, -2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 3, {2, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 3, {1, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 3, {2, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 3, {-2, 1}, timeout_ms), midi, log, stats);     
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 3, {-1, -1}, timeout_ms), midi, log, stats);*/
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 3, {1, 2}, timeout_ms), midi, log, stats);
    /*generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 3, {1, -1}, timeout_ms), midi, log, stats);     
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 3, {-2, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 3, {2, -2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 3, {-2, -2}, timeout_ms), midi, log, stats);     
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 3, {2, -2}, timeout_ms), midi, log, stats); 
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 3, {2, -2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 3, {-2, 2}, timeout_ms), midi, log, stats);     
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 3, {-2, 0}, timeout_ms), midi, log, stats);*/

    /*generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {-1, -2, -3}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {0, -1, -3}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIRST_SPECIES, 4, {-1, 1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {-2, -1, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {-2, -1, 1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(SECOND_SPECIES, 4, {2, 1, -2}, timeout_ms), midi, log, stats);*/
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {-2, -1, 0}, timeout_ms), midi, log, stats);
    /*generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {1, -2, -2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(THIRD_SPECIES, 4, {1, 1, -2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {0, -2, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {0, 2, -2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FOURTH_SPECIES, 4, {-3, -1, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-2, 1, 0}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-2, -1, -1}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_classic_gen_case(FIFTH_SPECIES, 4, {-2, -1, 1}, timeout_ms), midi, log, stats);*/

    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES}, {0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FIFTH_SPECIES}, {2, 2, 1}, timeout_ms), midi, log, stats);

    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, SECOND_SPECIES}, {1, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES}, {0, 1, 2}, timeout_ms), midi, log, stats);

    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, THIRD_SPECIES}, {0, 2}, timeout_ms), midi, log, stats);
    generate_counterpoints(generate_general_gen_case({THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES}, {0, 1, 2}, timeout_ms), midi, log, stats);

    generate_counterpoints(generate_general_gen_case({SECOND_SPECIES, THIRD_SPECIES, FOURTH_SPECIES}, {2, 1, 0}, timeout_ms), midi, log, stats);
}

void Generations::gen_default(){ // Generic tests (Currently : ~24h for every test with stats)
    midi_folder = "midi_bryce/";
    reference_counterpoints_for_testing(60000, true, true, true); // 1min

    // Personnal tests
    reset_cf_name();
    cf_name += "_testError";
    string log_file = "test_logs.csv";
    string stats_file = "test_stats.csv";
    int timeout_ms = 600000; // 10min
    bool midi = true;
    bool log = true;
    bool stats = true;

    //reference_counterpoints_for_testing(timeout_ms, midi, log, stats); // 10min
}

void Generations::gen_bryce(){ // manual single test
    cout << "===== gen_bryce 2 =====" << endl;
    cantusFirmus = {60,   62,   65,   64,   67,   65,   64,   62,   60};
    cfSize = cantusFirmus.size();
    melodic_params = {0, 1, 2, 576, 5, 10, 25, 40};
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    borrowMode = 1;

    midi_folder = "midi_bryce/";
    GenerationCase gen_case = generationCase();
    gen_case.spList = {FIRST_SPECIES};
    gen_case.v_type = {1};

    //activeConstraints[SP1_1H6] = true;
    auto* problem = create_problem(cantusFirmus, gen_case.spList, gen_case.v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    cout << "=== Problem defined" << endl;

    // ===== Additional and necessary Constrains =====
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    // rel(problem->getHome(), problem->getSolutionArray()[1], IRT_EQ, 54);

    //auto home = problem->getHome();
    //branch(home, problem->cost(), INT_VAR_NONE(), INT_VAL_MAX()); // Solves all "ValOfUnassignedVar" problems + accelerate every test
    
    cout << "=== Getting solutions" << endl;

    // Run a short DFS on randomized branches to get fast a first "decent" solution, and bound globalCost to it, to fasten BAB ?
    /*
    DFS<CounterpointProblem> e(problem);
    int nb_sol = 0;
    int max_solutions = 100;
    
    while(CounterpointProblem* pb = e.next()){
          
        nb_sol++;
        if (nb_sol % 100 == 0) {
            cout << "Solution " << nb_sol << ": " << endl;
            cout << pb->to_string() << endl;
            cout << pb->getSize() << endl;
            // cout << int_vector_to_string(cantusFirmus) << endl;
        }

        delete pb;
        if (nb_sol > max_solutions) {
            cout << "Found " << max_solutions << " solutions. Stopping search." << endl;
            break;
        }
        
        cout << "OK" << endl;
    }*/

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%10000 == 0){
            cout << "==========\nITERATION " << myCount << endl;
            cout << "=== BAB:\n" << s->to_string() << endl;
            cout << "=== END GEN " << myCount << endl;
            //cout << "H INTERVALS:\n" << s->getCounterpoint_1()->getHIntervals() << endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        cout << "BEST (" << myCount << "): " << best->getSolutionArray() << endl;
        delete best;
    }
    
    delete problem;

}


