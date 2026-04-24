// 
// Created by Bryce Burignat. 
// This file is the header file of the generation framework implementation.
// It is based on the testing framework.
// 

#ifndef FUX_GEN_HPP
#define FUX_GEN_HPP

#include "Utilities.hpp"
#include "Midi.hpp"
#include "Parts/Part.hpp"
#include "CounterpointUtils.hpp"
#include "CounterpointProblems/CounterpointProblem.hpp"
#include "Parts/CantusFirmus.hpp"

using namespace Gecode;
using namespace std;

// -----------------------------

struct GenerationCase {
    vector<Species> spList;
    vector<int> v_type;
    int n_voices;
    int timeout_ms;
    int stagnation_ms;

    // In case of multiple_vtypes
    bool multiple_vtypes;
    vector<int> v_types;            
    vector<int> v_types_1sp;

    // For logging purpose
    vector<int> checkpoints;
    string midi_file_path;
    string log_file_path;
    string stats_file_path;
};

struct BenchResult {
    int solutions;
    string termination;
    double ms_first;
    double ms_total;
    vector<pair<int,double>> checkpoint_times;
    string best_cost;
    IntVarArray solution;
    Search::Statistics gecodeStats;
};

// -------------------------------
class Generations{

protected:
    vector<int> cantusFirmus;
    vector<int> cp;
    int idx;
    int cfSize;
    vector<int> melodic_params;
    vector<int> general_params;
    vector<int> specific_params;
    vector<int> importance;
    int borrowMode;
    string log_folder;
    string midi_folder;
    string cf_name;

public:

// ===============================================

    Generations(char* gen);

    vector<Species> getSpList();
    vector<int> getCf();
    vector<int> getVType();
    int getBMode();
    vector<int> getCp();
    int getIdx();
    void reset_cf_name(bool undefined);

    void generate_counterpoints(GenerationCase gen_case, bool midi, bool log, bool stats);
    void generation_BAB_bench(CounterpointProblem* problem, GenerationCase& gen_case, bool midi, bool log, bool stats);

    void all_classic_counterpoints(int timeout_ms, bool midi, bool log, bool stats);
    void multi_species_counterpoints_batch1(int timeout_ms, bool midi, bool log, bool stats);
    void reference_counterpoints_for_testing(int timeout_ms, bool midi, bool log, bool stats);
    void gen_default();
    void gen_bryce();
};

#endif