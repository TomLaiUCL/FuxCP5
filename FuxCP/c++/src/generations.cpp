// 
// Created by Bryce Burignat.
// This file contains the generation framework implementation.  
// It is based on the testing framework.
//

#include <iostream>
#include <fstream>  // For file operations
#include <cmath>
#include <chrono>
#include <signal.h> // For testing blocking constrains in generations
#include "../headers/generations.hpp"
#include <gecode/int.hh> // Ensure you include the necessary Gecode headers


Generations::Generations(char* gen){
    cantusFirmus = {60, 62, 65, 64, 67, 65, 64, 62, 60};
    cfSize = cantusFirmus.size();
    melodic_params = {0, 1, 2, 576, 5, 10, 25, 40}; // Last ones augmented to avoid crazy leaps
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    borrowMode = 1;
    log_folder = "log/";
    midi_folder = "midi/";
    cf_name = "undefined";

    fill(activeConstraints.begin(), activeConstraints.end(), true);
    
    if(strcmp(gen, "bryce")==0){
        gen_bryce();
    } else if(strcmp(gen, "bryce_2")==0){
        gen_bryce_2();
    }
    else {
        std::cout << "Test for constraint not found : " << gen << std::endl;
        throw invalid_argument("Test for constraint not found !");
    }
}

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


// ----- Initialisation functions -----
// Default data for GenerationCase, BenchResult, MusicalStats and spList

static GenerationCase generationCase(){
    GenerationCase gen_case;

    gen_case.spList = {};
    gen_case.v_type = {};
    gen_case.n_voices = 2;
    gen_case.timeout_ms = 600000; // 10min

    gen_case.multiple_vtypes = false;
    gen_case.v_types = {};            
    gen_case.v_types_1sp = {};

    gen_case.checkpoints = {};
    gen_case.log_file_path = "default_log.csv"; // filename for the moment

    return gen_case;
}

static BenchResult benchResult(){
    BenchResult br;
    br.solutions = 0;
    br.improvements = 0;
    br.timed_out = false;
    br.ms_first = -1.0;
    br.ms_total = -1.0;
    br.checkpoint_times = {};
    br.best_cost = "";
    br.solution = {};
    return br;
}

static MusicalStats musicalStats(){
    MusicalStats ms;
    ms.min_note = 999;
    ms.max_note = -999;
    ms.largest_leap = 0;
    ms.repeated = 0;
    ms.steps = 0;
    ms.skips = 0;
    return ms;
}

static vector<Species> default_spList(Species species, int n_voices){
    if (n_voices == 2){
        return {species};
    } else if (n_voices == 3){
        return {FIRST_SPECIES, species};
    } else {
        return {FIRST_SPECIES, FIRST_SPECIES, species};
    }
}

// ----- Static util functions -----

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
    const BenchResult& bench,
    const MusicalStats* music
) {
    const bool file_exists = static_cast<bool>(ifstream(gen_case.log_file_path));
    ofstream out(gen_case.log_file_path, ios::app);

    if (!file_exists) {
        out << "run_id,timestamp,cf_name,cf_notes,species,n_voices,v_type,sp_list,timeout_ms,timed_out,"
               "solutions,improvements,first_ms,total_ms,best_cost,complete_solution,"
               "ambitus,largest_leap,repeated,steps,skips,midi_file\n";
    }

    auto now = chrono::system_clock::now();
    auto now_t = chrono::system_clock::to_time_t(now);

    static int run_id = 0;
    run_id++;

    out << run_id << ","
        << csv_escape(string(ctime(&now_t)).substr(0, 24)) << ","
        << csv_escape(cf_name) << ","
        << csv_escape(join_ints(cantusFirmus)) << ","
        << (int)gen_case.spList.back() + 1 << ","
        << gen_case.n_voices << ","
        << csv_escape(join_ints(gen_case.v_type)) << ","
        << csv_escape(join_ints(vector<int>(gen_case.spList.begin(), gen_case.spList.end()))) << ","
        << gen_case.timeout_ms << ","
        << (bench.timed_out ? 1 : 0) << ","
        << bench.solutions << ","
        << bench.improvements << ","
        << bench.ms_first << ","
        << bench.ms_total << ","
        << csv_escape(bench.best_cost) << ","
        << bench.solution << ",";

    if (music) {
        out << (music->max_note - music->min_note) << ","
            << music->largest_leap << ","
            << music->repeated << ","
            << music->steps << ","
            << music->skips << ",";
    } else {
        out << ",,,,,";
    }

    out << csv_escape(gen_case.midi_file_path) << "\n";
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
    const MusicalStats* music,
    const string& midi_file_path,
    const string& log_file_path){

    if (bench.solutions > 0) {
        cout
            << "termination        : " << (bench.timed_out ? "TIMEOUT" : "OPTIMAL") << "\n"
            << "solutions     : " << bench.solutions << "\n"
            << "improvements  : " << bench.improvements << "\n"
            << "counterpoints     : " << bench.solution << "\n"
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

        if (music){
            cout
                << "ambitus       : " << (music->max_note - music->min_note) << "\n"
                << "largest_leap  : " << music->largest_leap << "\n"
                << "repeated      : " << music->repeated << "\n"
                << "steps         : " << music->steps << "\n"
                << "skips         : " << music->skips << "\n"
                << "midi          : " << midi_file_path << "\n"
                << "log          : " << log_file_path << "\n\n";
        }
    } else {
        cout
            << "status        : " << (bench.timed_out ? "TIMEOUT" : "NO_SOLUTION") << "\n"
            << "solutions     : 0\n"
            << "total_ms      : " << bench.ms_total << "\n"
            << "log          : " << log_file_path << "\n\n";
    }
}

static string generate_midi_filename(GenerationCase gen_case, string cf_name){
    string midi_file_path = cf_name + "_"
        + to_string(gen_case.n_voices) + "v_"
        + to_string((int)gen_case.spList.back()+1) + "sp_vtype_";
    for (int vt : gen_case.v_type) {
        midi_file_path += to_string(vt) + "_";
    }
    midi_file_path += ".mid";
    return midi_file_path;
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

/**
 * Compute musical statistics on one voice
 * @param notes the notes of the voice
 * @return MusicalStats object
 */
static MusicalStats analyze_notes(const vector<int>& notes) {
    MusicalStats s = musicalStats();

    if (notes.empty()) return s;

    for (int n : notes) {
        s.min_note = min(s.min_note, n);
        s.max_note = max(s.max_note, n);
    }

    for (size_t i = 1; i < notes.size(); ++i) {
        int d = abs(notes[i] - notes[i-1]);
        s.largest_leap = max(s.largest_leap, d);
        if (d == 0) s.repeated++;
        else if (d <= 2) s.steps++;
        else s.skips++;
    }
    return s;
}

//=======================================================================================
//============================== COUNTERPOINT GENERATION ================================

void Generations::generate_counterpoints(GenerationCase& gen_case, bool midi=true, bool log=true){
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

    // Checkpoints
    if (gen_case.checkpoints.empty()){
        if (n_voices == 2) {
            gen_case.checkpoints = {1, 2, 5, 10, 20, 50};
        } else if (n_voices == 3) {
            gen_case.checkpoints = {1, 10, 100, 1000, 5000};
        } else {
            gen_case.checkpoints = {1, 100, 1000, 10000, 20000};
        }
    }

    cout << "===== generate counterpoints " << n_voices << "v " << species+1 << "sp " << " =====" << endl;
    
    // Voices & Constrains definition
    int n_types_4v;
    int n_types_3v;
    if (n_voices == 4) {
        gen_case.spList = {FIRST_SPECIES, FIRST_SPECIES, species};
        n_types_3v = gen_case.v_types_1sp.size();
        n_types_4v = gen_case.v_types_1sp.size();
    }
    else if (n_voices == 3) {
        gen_case.spList = {FIRST_SPECIES, species};
        n_types_3v = gen_case.v_types_1sp.size();
        n_types_4v = 1;
    }
    else { // 2v
        gen_case.spList = {species};
        n_types_3v = 1;
        n_types_4v = 1;
    }

    if (gen_case.multiple_vtypes){
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
                    generation_BAB_bench(problem, gen_case, midi, log);
                    delete problem;
                }
            }
        }
    }
    else {
        auto* problem = create_problem(cantusFirmus, gen_case.spList, gen_case.v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        generation_BAB_bench(problem, gen_case, midi, log);
        delete problem;
    }
    
}

void Generations::generate_classic_counterpoints(Species species,
                                         int n_voices,
                                         const vector<int>& v_types,
                                         const vector<int>& v_types_1sp = {},
                                         int timeout_ms = 600000,
                                         bool midi=true,
                                         bool log=true){
    // Creating struct
    GenerationCase gen_case = generationCase();
    gen_case.n_voices = n_voices;
    gen_case.v_types = v_types;
    gen_case.v_types_1sp = v_types_1sp;
    gen_case.timeout_ms = timeout_ms;
    gen_case.multiple_vtypes = true;
    gen_case.spList = default_spList(species, n_voices);

    generate_counterpoints(gen_case, midi, log);
}

void Generations::generate_classic_counterpoints(Species species,
                                         int n_voices,
                                         vector<int> v_type,
                                         int timeout_ms = 600000,
                                         bool midi=true,
                                         bool log=true){
    // Creating struct
    GenerationCase gen_case = generationCase();
    gen_case.n_voices = n_voices;
    gen_case.spList = default_spList(species, n_voices);
    gen_case.v_type = v_type;
    gen_case.timeout_ms = timeout_ms;

    generate_counterpoints(gen_case, midi, log);
}


void Generations::generation_BAB_bench(CounterpointProblem* problem,
                          GenerationCase& gen_case,
                          bool midi,
                          bool log){
    Search::Options opt;
    Search::TimeStop ts(gen_case.timeout_ms);
    opt.stop = &ts;

    auto t0 = chrono::steady_clock::now();

    auto ms_since_start = [&]() {
        auto now = chrono::steady_clock::now();
        return chrono::duration<double, milli>(now - t0).count();
    };

    BAB<CounterpointProblem> e(problem, opt);
    CounterpointProblem* best = nullptr;

    BenchResult bench = benchResult();
    size_t next_checkpoint = 0;
    string last_cost_str;

    while (CounterpointProblem* s = e.next()) {
        bench.solutions++;

        if (bench.solutions == 1) {
            bench.ms_first = ms_since_start();
        }

        if (next_checkpoint < gen_case.checkpoints.size() &&
            bench.solutions == gen_case.checkpoints[next_checkpoint]) {
                bench.checkpoint_times.push_back({bench.solutions, ms_since_start()});
                next_checkpoint++;
                cout << "=== Iteration " << bench.solutions << " : " << s->getSolutionArray() << endl;
        }
        
        delete best; // keep only the best-so-far
        best = s;

        ostringstream oss;
        oss << best->cost();
        string current_cost = oss.str();
        if (current_cost != last_cost_str) {
            bench.improvements++;
            last_cost_str = current_cost;
            bench.best_cost = current_cost;
        }
    }

    bench.ms_total = ms_since_start();
    bench.timed_out = e.stopped();

    Species species = gen_case.spList.back();
    MusicalStats music;

    if (best) { // there is a solution !
        size_t cfSize = cantusFirmus.size();
        bench.solution = best->getSolutionArray();

        // Part to change when saveMidi can handle 3+ voices
        auto voice_notes = extract_last_voice_notes(best, gen_case.spList, cfSize);
        music = analyze_notes(voice_notes);

        if (midi){
            gen_case.midi_file_path = midi_folder + generate_midi_filename(gen_case, cf_name);
            saveMidi(gen_case.midi_file_path, cantusFirmus, voice_notes, species);
        }
        
        delete best;
    }
    else {
        gen_case.midi_file_path = "None";
    }

    if (log){
        log_gen_data_in_csv(cantusFirmus, cf_name, gen_case, bench, &music);
    }
    log_gen_data_in_console(bench, &music, gen_case.midi_file_path, gen_case.log_file_path);
    
}

//=======================================================================================
//=======================================================================================

void Generations::gen_bryce(){ // Generic tests
    midi_folder = "midi_bryce/";
    generate_classic_counterpoints(FOURTH_SPECIES, 2, {2}, 7200000);
    generate_classic_counterpoints(THIRD_SPECIES, 2, {2}, 7200000);
    generate_classic_counterpoints(FOURTH_SPECIES, 3, {4, 2}, 7200000);
    generate_classic_counterpoints(FOURTH_SPECIES, 3, {1, 2}, 7200000);
    generate_classic_counterpoints(FOURTH_SPECIES, 3, {2, 1}, 7200000);
    generate_classic_counterpoints(THIRD_SPECIES, 3, {4, 2}, 7200000);
    generate_classic_counterpoints(THIRD_SPECIES, 3, {1, 2}, 7200000);
    generate_classic_counterpoints(THIRD_SPECIES, 3, {2, 1}, 7200000);
}

void Generations::gen_bryce_2(){
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


