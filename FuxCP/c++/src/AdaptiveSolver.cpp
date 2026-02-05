// 
// Created by Sacha
// ADAPTIVE SOLVER: When a 4-voice counterpoint problem times out with hard constraints,
// automatically diagnose which of the 5 known blocking constraints are causing the timeout,
// then retry with those constraints relaxed (posted as soft/cost-based instead of hard).
//

#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <gecode/int.hh>
#include <gecode/search.hh>
#include "../headers/Utilities.hpp"
#include "../headers/CounterpointUtils.hpp"
#include "../headers/CounterpointProblems/CounterpointProblem.hpp"

using namespace Gecode;
using namespace std;

// =============================================================================
// Configuration
// =============================================================================

const int INITIAL_TIMEOUT_SECONDS = 30;   // Timeout for initial hard solve
const int DIAG_TIMEOUT_SECONDS = 3;       // Short timeout per constraint diagnostic
const int RETRY_TIMEOUT_SECONDS = 30;     // Timeout for soft retry
const int MAX_SOLUTIONS = 3;              // Solutions to find before declaring success

// Cantus firmus
vector<int> cantusFirmus = {60, 62, 65, 64, 67, 65, 64, 62, 60};

// Parameters
vector<int> melodic_params = {0, 1, 2, 576, 5, 10, 25, 40};
vector<int> general_params = {4, 1, 1, 2, 2, 2, 8, 1};
vector<int> specific_params = {8, 4, 0, 2, 1, 8, 50};
vector<int> importance = {8, 7, 5, 2, 9, 3, 14, 12, 6, 11, 4, 10, 1, 13};
int borrowMode = 1;

ofstream outputFile;

// The 5 known blocking constraints
struct BlockingConstraint {
    int id;
    string name;
    string description;
};

const vector<BlockingConstraint> BLOCKING_CANDIDATES = {
    {V4_1H4,  "V4_1H4",  "Last chord same as CF fundamental"},
    {V4_2M2,  "V4_2M2",  "Melodic intervals <= minor sixth (2sp)"},
    {SP3_3H2, "SP3_3H2", "Dissonance implies diminution (3sp)"},
    {CF_1H1,  "CF_1H1",  "CF harmonic intervals are consonances"},
    {SP2_2H2, "SP2_2H2", "Arsis harmonies not dissonant (2sp)"},
};

// =============================================================================
// Utility functions
// =============================================================================

string speciesName(Species sp) {
    switch (sp) {
        case FIRST_SPECIES: return "1sp";
        case SECOND_SPECIES: return "2sp";
        case THIRD_SPECIES: return "3sp";
        case FOURTH_SPECIES: return "4sp";
        case FIFTH_SPECIES: return "5sp";
        default: return "???";
    }
}

string spListToString(const vector<Species>& spList) {
    string result = "{";
    for (size_t i = 0; i < spList.size(); i++) {
        result += speciesName(spList[i]);
        if (i < spList.size() - 1) result += ", ";
    }
    result += "}";
    return result;
}

bool isRelevant(const BlockingConstraint& bc, const vector<Species>& spList) {
    // V4_1H4 and CF_1H1 are always relevant for 4-voice
    if (bc.id == V4_1H4 || bc.id == CF_1H1) return true;
    // V4_2M2 and SP2_2H2 only relevant if there's a 2nd species voice
    if (bc.id == V4_2M2 || bc.id == SP2_2H2) {
        for (auto sp : spList) if (sp == SECOND_SPECIES) return true;
        return false;
    }
    // SP3_3H2 only relevant if there's a 3rd species voice
    if (bc.id == SP3_3H2) {
        for (auto sp : spList) if (sp == THIRD_SPECIES) return true;
        return false;
    }
    return true;
}

// =============================================================================
// DFS search wrapper
// =============================================================================

struct SolveResult {
    int solutionsFound;
    double elapsedSeconds;
    bool timedOut;
    bool failed;
    int bestCost;
    int relaxationCost;  // -1 if no relaxation active
};

SolveResult runDFS(const vector<Species>& spList, const vector<int>& v_type, int timeoutSec) {
    SolveResult sr = {0, 0.0, false, false, -1, -1};
    
    auto* problem = create_problem(cantusFirmus, spList, v_type,
                                   melodic_params, general_params,
                                   specific_params, importance, borrowMode);
    if (!problem) { sr.failed = true; return sr; }
    if (problem->getHome().failed()) { sr.failed = true; delete problem; return sr; }
    
    Search::Options opts;
    Search::TimeStop stop(timeoutSec * 1000);
    opts.stop = &stop;
    
    DFS<CounterpointProblem> engine(problem, opts);
    
    auto startTime = chrono::steady_clock::now();
    
    while (CounterpointProblem* s = engine.next()) {
        sr.solutionsFound++;
        
        int cost = 0;
        for (int i = 0; i < s->cost().size(); i++) {
            cost += s->cost()[i].val();
        }
        if (sr.bestCost < 0 || cost < sr.bestCost) {
            sr.bestCost = cost;
        }
        
        // If relaxation is active, the first cost element is relaxation cost
        bool hasRelaxation = false;
        for (int i = 0; i < (int)softConstraints.size(); i++) {
            if (softConstraints[i]) { hasRelaxation = true; break; }
        }
        if (hasRelaxation && s->cost().size() > 0) {
            sr.relaxationCost = s->cost()[0].val();
        }
        
        delete s;
        if (sr.solutionsFound >= MAX_SOLUTIONS) break;
    }
    
    auto endTime = chrono::steady_clock::now();
    sr.elapsedSeconds = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    sr.timedOut = engine.stopped() && sr.solutionsFound < MAX_SOLUTIONS;
    
    delete problem;
    return sr;
}

// =============================================================================
// Generate all 19 third-species test cases
// =============================================================================

struct TestCase {
    string name;
    vector<Species> spList;
    vector<int> v_type;
};

vector<TestCase> generateAllTestCases() {
    vector<Species> species_options = {FIRST_SPECIES, SECOND_SPECIES, THIRD_SPECIES};
    vector<TestCase> tests;
    vector<int> v_type = {0, 1, 2};
    
    for (auto sp1 : species_options) {
        for (auto sp2 : species_options) {
            for (auto sp3 : species_options) {
                if (sp1 == THIRD_SPECIES || sp2 == THIRD_SPECIES || sp3 == THIRD_SPECIES) {
                    string name = "4v_" + speciesName(sp1) + "_" + speciesName(sp2) + "_" + speciesName(sp3);
                    tests.push_back({name, {sp1, sp2, sp3}, v_type});
                }
            }
        }
    }
    return tests;
}

// =============================================================================
// Adaptive solve for one combination
// =============================================================================

void adaptiveSolve(const string& testName, const vector<Species>& spList, const vector<int>& v_type) {
    cout << "\n================================================================" << endl;
    cout << "ADAPTIVE SOLVE: " << testName << " " << spListToString(spList) << endl;
    cout << "================================================================" << endl;
    
    outputFile << "\n================================================================" << endl;
    outputFile << "ADAPTIVE SOLVE: " << testName << " " << spListToString(spList) << endl;
    outputFile << "================================================================" << endl;
    
    // ─── PHASE 1: Try normal hard solve ───
    cout << "\n[Phase 1] Hard solve (all constraints, " << INITIAL_TIMEOUT_SECONDS << "s timeout)..." << flush;
    outputFile << "\n[Phase 1] Hard solve (" << INITIAL_TIMEOUT_SECONDS << "s timeout)" << endl;
    
    fill(activeConstraints.begin(), activeConstraints.end(), true);
    fill(softConstraints.begin(), softConstraints.end(), false);
    
    SolveResult hardResult = runDFS(spList, v_type, INITIAL_TIMEOUT_SECONDS);
    
    string hardStatus;
    if (hardResult.failed) hardStatus = "FAILED";
    else if (hardResult.timedOut && hardResult.solutionsFound == 0) hardStatus = "TIMEOUT";
    else if (hardResult.timedOut) hardStatus = "TIMEOUT_PARTIAL (" + to_string(hardResult.solutionsFound) + " sol)";
    else hardStatus = "OK (" + to_string(hardResult.solutionsFound) + " sol, cost=" + to_string(hardResult.bestCost) + ")";
    
    cout << " " << hardStatus << " in " << fixed << setprecision(2) << hardResult.elapsedSeconds << "s" << endl;
    outputFile << "  Result: " << hardStatus << " in " << fixed << setprecision(2) << hardResult.elapsedSeconds << "s" << endl;
    
    // If hard solve works, we're done!
    if (!hardResult.timedOut && !hardResult.failed && hardResult.solutionsFound > 0) {
        cout << "  => Hard solve succeeded. No relaxation needed." << endl;
        outputFile << "  => Hard solve succeeded. No relaxation needed." << endl;
        outputFile << "  Final: HARD_OK, cost=" << hardResult.bestCost << ", time=" << fixed << setprecision(2) << hardResult.elapsedSeconds << "s" << endl;
        return;
    }
    
    if (hardResult.failed) {
        cout << "  => Problem infeasible at creation. Cannot proceed." << endl;
        outputFile << "  => FAILED: Infeasible at creation." << endl;
        return;
    }
    
    // ─── PHASE 2: Quick diagnostic ───
    cout << "\n[Phase 2] Diagnosing blocking constraints (" << DIAG_TIMEOUT_SECONDS << "s per constraint)..." << endl;
    outputFile << "\n[Phase 2] Constraint diagnostic (" << DIAG_TIMEOUT_SECONDS << "s per constraint)" << endl;
    
    vector<BlockingConstraint> blockingFound;
    
    for (const auto& bc : BLOCKING_CANDIDATES) {
        if (!isRelevant(bc, spList)) {
            cout << "  " << bc.name << ": skipped (not relevant)" << endl;
            outputFile << "  " << bc.name << ": skipped" << endl;
            continue;
        }
        
        cout << "  Testing " << bc.name << " (" << bc.description << ")... " << flush;
        
        // Enable all constraints, disable just this one, try hard solve
        fill(activeConstraints.begin(), activeConstraints.end(), true);
        fill(softConstraints.begin(), softConstraints.end(), false);
        activeConstraints[bc.id] = false;
        
        SolveResult diagResult = runDFS(spList, v_type, DIAG_TIMEOUT_SECONDS);
        
        bool unblocking = (!diagResult.timedOut && !diagResult.failed && diagResult.solutionsFound > 0);
        
        if (unblocking) {
            cout << "\033[32mBLOCKING\033[0m (" << diagResult.solutionsFound << " sol in " 
                 << fixed << setprecision(2) << diagResult.elapsedSeconds << "s)" << endl;
            outputFile << "  " << bc.name << ": BLOCKING (disabling finds " << diagResult.solutionsFound 
                      << " sol in " << fixed << setprecision(2) << diagResult.elapsedSeconds << "s)" << endl;
            blockingFound.push_back(bc);
        } else {
            string dStatus = diagResult.failed ? "BREAKING" : (diagResult.timedOut ? "not blocking" : "no solution");
            cout << dStatus << endl;
            outputFile << "  " << bc.name << ": " << dStatus << endl;
        }
    }
    
    if (blockingFound.empty()) {
        cout << "\n  => No blocking constraints found among candidates." << endl;
        cout << "     The timeout may be caused by other constraints or search space size." << endl;
        outputFile << "\n  => No blocking constraints identified. Timeout cause unknown." << endl;
        return;
    }
    
    // ─── PHASE 3: Retry with soft constraints ───
    cout << "\n[Phase 3] Retrying with " << blockingFound.size() << " constraint(s) relaxed (" 
         << RETRY_TIMEOUT_SECONDS << "s timeout)..." << endl;
    outputFile << "\n[Phase 3] Retry with soft constraints (" << RETRY_TIMEOUT_SECONDS << "s timeout)" << endl;
    
    cout << "  Relaxing:";
    outputFile << "  Relaxing:";
    for (const auto& bc : blockingFound) {
        cout << " " << bc.name;
        outputFile << " " << bc.name;
    }
    cout << endl;
    outputFile << endl;
    
    // Enable all constraints, set blocking ones as soft
    fill(activeConstraints.begin(), activeConstraints.end(), true);
    fill(softConstraints.begin(), softConstraints.end(), false);
    for (const auto& bc : blockingFound) {
        softConstraints[bc.id] = true;
    }
    
    SolveResult softResult = runDFS(spList, v_type, RETRY_TIMEOUT_SECONDS);
    
    string softStatus;
    if (softResult.failed) softStatus = "FAILED";
    else if (softResult.timedOut && softResult.solutionsFound == 0) softStatus = "TIMEOUT";
    else if (softResult.timedOut) softStatus = "TIMEOUT_PARTIAL (" + to_string(softResult.solutionsFound) + " sol)";
    else softStatus = "OK (" + to_string(softResult.solutionsFound) + " sol)";
    
    cout << "  Result: " << softStatus << " in " << fixed << setprecision(2) << softResult.elapsedSeconds << "s" << endl;
    outputFile << "  Result: " << softStatus << " in " << fixed << setprecision(2) << softResult.elapsedSeconds << "s" << endl;
    
    if (softResult.solutionsFound > 0) {
        cout << "  Best cost: " << softResult.bestCost 
             << " (relaxation violations: " << softResult.relaxationCost << ")" << endl;
        outputFile << "  Best cost: " << softResult.bestCost 
                  << " (relaxation violations: " << softResult.relaxationCost << ")" << endl;
        
        if (softResult.relaxationCost == 0) {
            cout << "  \033[32m=> Solution respects ALL original constraints (0 violations)!\033[0m" << endl;
            outputFile << "  => Solution respects ALL original constraints (0 violations)." << endl;
        } else {
            cout << "  \033[33m=> Solution violates " << softResult.relaxationCost 
                 << " relaxed constraint position(s).\033[0m" << endl;
            outputFile << "  => Solution violates " << softResult.relaxationCost 
                      << " relaxed constraint position(s)." << endl;
        }
    } else {
        cout << "  => Soft solve also failed." << endl;
        outputFile << "  => Soft solve also failed." << endl;
    }
    
    // ─── SUMMARY ───
    outputFile << "\n  --- SUMMARY ---" << endl;
    outputFile << "  Hard solve: " << hardStatus << endl;
    outputFile << "  Blocking constraints: ";
    for (size_t i = 0; i < blockingFound.size(); i++) {
        outputFile << blockingFound[i].name;
        if (i < blockingFound.size() - 1) outputFile << ", ";
    }
    outputFile << endl;
    outputFile << "  Soft solve: " << softStatus << endl;
    if (softResult.solutionsFound > 0) {
        outputFile << "  Total cost: " << softResult.bestCost 
                  << " (relaxation: " << softResult.relaxationCost << ")" << endl;
    }
    
    // Reset softConstraints
    fill(softConstraints.begin(), softConstraints.end(), false);
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    cout << "================================================================" << endl;
    cout << " ADAPTIVE CONSTRAINT RELAXATION SOLVER" << endl;
    cout << " 4-Voice Counterpoint with 3rd Species" << endl;
    cout << "================================================================" << endl;
    
    string filename = "adaptive_results.txt";
    
    time_t now = time(0);
    char* dt = ctime(&now);
    
    outputFile.open(filename);
    outputFile << "================================================================" << endl;
    outputFile << " ADAPTIVE CONSTRAINT RELAXATION SOLVER" << endl;
    outputFile << "================================================================" << endl;
    outputFile << "Date: " << dt;
    outputFile << "Initial timeout: " << INITIAL_TIMEOUT_SECONDS << "s" << endl;
    outputFile << "Diagnostic timeout: " << DIAG_TIMEOUT_SECONDS << "s per constraint" << endl;
    outputFile << "Retry timeout: " << RETRY_TIMEOUT_SECONDS << "s" << endl;
    outputFile << "Max solutions: " << MAX_SOLUTIONS << endl;
    outputFile << endl;
    
    if (argc < 2) {
        cout << endl;
        cout << "Usage:" << endl;
        cout << "  ./AdaptiveSolver all        - Run on all 19 third-species combinations" << endl;
        cout << "  ./AdaptiveSolver X_Y_Z      - Run on specific combo (e.g. 3_3_3, 1_3_3)" << endl;
        cout << endl;
        cout << "Examples:" << endl;
        cout << "  ./AdaptiveSolver 3_3_3      - Triple third species (hardest)" << endl;
        cout << "  ./AdaptiveSolver 1_3_3      - {1sp, 3sp, 3sp}" << endl;
        cout << "  ./AdaptiveSolver all        - Test everything" << endl;
        outputFile.close();
        return 0;
    }
    
    string arg = argv[1];
    
    if (arg == "all") {
        vector<TestCase> tests = generateAllTestCases();
        
        int hardOK = 0, softOK = 0, softFail = 0;
        
        cout << "\nRunning " << tests.size() << " test cases...\n" << endl;
        outputFile << "Running " << tests.size() << " test cases.\n" << endl;
        
        for (const auto& test : tests) {
            adaptiveSolve(test.name, test.spList, test.v_type);
        }
        
        cout << "\n================================================================" << endl;
        cout << " ALL TESTS COMPLETE" << endl;
        cout << "================================================================" << endl;
    } else {
        // Parse specific combo like "3_3_3"
        vector<Species> spList;
        bool valid = true;
        
        if (arg.length() >= 5 && arg[1] == '_' && arg[3] == '_') {
            for (int pos = 0; pos < 3 && valid; pos++) {
                char c = arg[pos * 2];
                switch (c) {
                    case '1': spList.push_back(FIRST_SPECIES); break;
                    case '2': spList.push_back(SECOND_SPECIES); break;
                    case '3': spList.push_back(THIRD_SPECIES); break;
                    case '4': spList.push_back(FOURTH_SPECIES); break;
                    case '5': spList.push_back(FIFTH_SPECIES); break;
                    default: valid = false;
                }
            }
        } else {
            valid = false;
        }
        
        if (valid && spList.size() == 3) {
            string name = "4v_" + speciesName(spList[0]) + "_" + speciesName(spList[1]) + "_" + speciesName(spList[2]);
            adaptiveSolve(name, spList, {0, 1, 2});
        } else {
            cout << "Invalid argument: " << arg << endl;
            cout << "Usage: ./AdaptiveSolver all | X_Y_Z" << endl;
            outputFile.close();
            return 1;
        }
    }
    
    outputFile << "\n================================================================" << endl;
    outputFile << "ADAPTIVE SOLVER COMPLETE" << endl;
    outputFile << "================================================================" << endl;
    
    outputFile.close();
    
    cout << "\nResults saved to: " << filename << endl;
    
    return 0;
}
