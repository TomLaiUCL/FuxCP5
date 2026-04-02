// 
// Created by Sacha
// DIAGNOSTIC TEST: When a 4-voice 3rd species combination times out,
// re-run it with each constraint disabled one at a time to find which
// constraint(s) are blocking the solution.
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

const int DIAG_TIMEOUT_SECONDS = 10;      // Timeout plus court pour le diagnostic (10s)
const int SCREEN_TIMEOUT_SECONDS = 30;    // Timeout pour le screening initial
const int MAX_SOLUTIONS = 3;              // On cherche juste 3 solutions pour confirmer

// Cantus firmus par défaut
vector<int> cantusFirmus = {60, 62, 65, 64, 67, 65, 64, 62, 60};

// Paramètres par défaut
vector<int> melodic_params = {0, 1, 2, 576, 5, 10, 25, 40};
vector<int> general_params = {4, 1, 1, 2, 2, 2, 8, 1};
vector<int> specific_params = {8, 4, 0, 2, 1, 8, 50};
vector<int> importance = {8, 7, 5, 2, 9, 3, 14, 12, 6, 11, 4, 10, 1, 13};
int borrowMode = 1;

ofstream outputFile;

// =============================================================================
// Constraint information structure
// =============================================================================

struct ConstraintInfo {
    int id;
    string name;
    string description;
    string category;  // "4V" = FourVoiceCounterpoint, "SP1" = FirstSpecies, "SP2" = SecondSpecies, "SP3" = ThirdSpecies, "CF", "STRATUM"
};

// All constraints relevant to 4-voice + 3rd species combinations
vector<ConstraintInfo> getAllRelevantConstraints() {
    return {
        // === Cantus Firmus constraints ===
        {CF_1H1,       "CF_1H1",       "CF: harmonic intervals consonances",       "CF"},
        {CF_1P1,       "CF_1P1",       "CF: perfect consonances motion rule",      "CF"},
        {CF_1P3,       "CF_1P3",       "CF: no battuta",                           "CF"},
        
        // === Stratum constraints ===
        {STRATUM_UPPER_1H3,  "STRATUM_UPPER_1H3",  "Stratum: upper voice H3 rule",      "STRATUM"},
        {STRATUM_UPPER_1H10, "STRATUM_UPPER_1H10", "Stratum: upper voice H10 rule",     "STRATUM"},
        {STRATUM_UPPER_1H12, "STRATUM_UPPER_1H12", "Stratum: upper voice H12 rule",     "STRATUM"},
        {STRATUM_1H3,        "STRATUM_1H3",        "Stratum: H3 rule",                  "STRATUM"},
        {STRATUM_1H12,       "STRATUM_1H12",       "Stratum: H12 rule",                 "STRATUM"},
        
        // === 4-Voice Problem-level constraints ===
        {V4_G6,   "V4_G6",   "4V: G6 no chromatic melodies",                     "4V"},
        {V4_1H4,  "V4_1H4",  "4V: H4/G9 last chord same as CF fundamental",      "4V"},
        {V4_1H8,  "V4_1H8",  "4V: H8 prefer harmonic triads",                    "4V"},
        {V4_1M4,  "V4_1M4",  "4V: M4 variety cost (diverse notes)",              "4V"},
        {V4_1P4,  "V4_1P4",  "4V: P4 avoid successive perfect consonances",      "4V"},
        {V4_1P6,  "V4_1P6",  "4V: P6 no move in same direction (all 4 voices)",  "4V"},
        {V4_1P7,  "V4_1P7",  "4V: P7 no successive ascending sixths",            "4V"},
        {V4_2M2,  "V4_2M2",  "4V: 2M2 melodic intervals <= minor sixth",         "4V"},
        {V4_5R9,  "V4_5R9",  "4V: 5R9 two 5sp diversity",                        "4V"},
        {V4_U2,   "V4_U2",   "4V: U2 no minor second between upper voices",      "4V"},
        
        // === 1st Species Part constraints ===
        {SP1_G4,       "SP1_G4",       "1SP: G4 counterpoint in same key",              "SP1"},
        {SP1_G7,       "SP1_G7",       "1SP: G7 melodic intervals should be small",     "SP1"},
        {SP1_1H1,      "SP1_1H1",      "1SP: 1H1 harmonic intervals are consonances",   "SP1"},
        {SP1_1H6,      "SP1_1H6",      "1SP: 1H6 prefer imperfect consonances",         "SP1"},
        {SP1_1M2_4V,   "SP1_1M2_4V",   "1SP: 1M2 melodic intervals <= minor sixth (4V)","SP1"},
        {SP1_1P1_4V,   "SP1_1P1_4V",   "1SP: 1P1 no direct motion to perf cons (4V)",   "SP1"},
        {SP1_1P3_4V,   "SP1_1P3_4V",   "1SP: 1P3 no battuta (4V)",                      "SP1"},
        
        // === 2nd Species Part constraints ===
        {SP2_2H2,      "SP2_2H2",      "2SP: 2H2 arsis harmonies not dissonant",        "SP2"},
        {SP2_2M1,      "SP2_2M1",      "2SP: 2M1 octave leap rule",                     "SP2"},
        {SP2_2P3,      "SP2_2P3",      "2SP: 2P3 no battuta (adapted)",                 "SP2"},
        {SP2_2H3_4V,   "SP2_2H3_4V",   "2SP: 2H3 penultimate rule (4V)",               "SP2"},
        {SP2_1P1_4V,   "SP2_1P1_4V",   "2SP: 1P1 no direct motion to perf cons (4V)",   "SP2"},
        
        // === 3rd Species Part constraints ===
        {SP3_3H2,      "SP3_3H2",      "3SP: 3H2 dissonance implies diminution",        "SP3"},
        {SP3_3H3,      "SP3_3H3",      "3SP: 3H3 cambiata cost",                        "SP3"},
        {SP3_3M1,      "SP3_3M1",      "3SP: 3M1 avoid return to same note (m2 zero)",  "SP3"},
        {SP3_1P3,      "SP3_1P3",      "3SP: 1P3 no battuta (adapted for 3sp)",         "SP3"},
        {SP3_U1,       "SP3_U1",       "3SP: U1 no melodic interval 9-11 semitones",    "SP3"},
        {SP3_U3,       "SP3_U3",       "3SP: U3/3M4 penultimate measure constraint",    "SP3"},
        {SP3_1H7_4V,   "SP3_1H7_4V",   "3SP: 1H7 last interval constraint (4V)",        "SP3"},
        {SP3_3H6_4V,   "SP3_3H6_4V",   "3SP: 3H6 harmonic triad on beat 2/3 (4V)",     "SP3"},
        {SP3_1P1_4V,   "SP3_1P1_4V",   "3SP: 1P1 direct motion cost (4V)",              "SP3"},
    };
}

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

// =============================================================================
// Quick DFS test: returns number of solutions found in the timeout
// =============================================================================

struct QuickResult {
    int solutionsFound;
    double elapsedSeconds;
    bool timedOut;
    bool failed;      // problem infeasible at creation
    int bestCost;
};

QuickResult quickDFS(const vector<Species>& spList, const vector<int>& v_type, int timeoutSec) {
    QuickResult qr = {0, 0.0, false, false, -1};
    
    auto* problem = create_problem(cantusFirmus, spList, v_type,
                                   melodic_params, general_params,
                                   specific_params, importance, borrowMode);
    if (!problem) {
        qr.failed = true;
        return qr;
    }
    
    if (problem->getHome().failed()) {
        qr.failed = true;
        delete problem;
        return qr;
    }
    
    Search::Options opts;
    Search::TimeStop stop(timeoutSec * 1000);
    opts.stop = &stop;
    
    DFS<CounterpointProblem> engine(problem, opts);
    
    auto startTime = chrono::steady_clock::now();
    
    while (CounterpointProblem* s = engine.next()) {
        qr.solutionsFound++;
        
        int cost = 0;
        for (int i = 0; i < s->cost().size(); i++) {
            cost += s->cost()[i].val();
        }
        if (qr.bestCost < 0 || cost < qr.bestCost) {
            qr.bestCost = cost;
        }
        
        delete s;
        
        if (qr.solutionsFound >= MAX_SOLUTIONS) break;
    }
    
    auto endTime = chrono::steady_clock::now();
    qr.elapsedSeconds = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    qr.timedOut = engine.stopped() && qr.solutionsFound < MAX_SOLUTIONS;
    
    delete problem;
    return qr;
}

// =============================================================================
// Diagnostic: disable one constraint at a time
// =============================================================================

struct DiagResult {
    ConstraintInfo constraint;
    QuickResult result;
};

void runDiagnostic(const string& testName, const vector<Species>& spList, const vector<int>& v_type) {
    vector<ConstraintInfo> allConstraints = getAllRelevantConstraints();
    
    // Filter to only constraints relevant to the species in this test
    vector<ConstraintInfo> relevantConstraints;
    bool has1sp = false, has2sp = false, has3sp = false;
    for (auto sp : spList) {
        if (sp == FIRST_SPECIES) has1sp = true;
        if (sp == SECOND_SPECIES) has2sp = true;
        if (sp == THIRD_SPECIES) has3sp = true;
    }
    
    for (const auto& c : allConstraints) {
        // Always include CF, STRATUM, 4V constraints
        if (c.category == "CF" || c.category == "STRATUM" || c.category == "4V") {
            relevantConstraints.push_back(c);
        }
        // Include SP1 only if there's a first species voice
        else if (c.category == "SP1" && has1sp) {
            relevantConstraints.push_back(c);
        }
        // Include SP2 only if there's a second species voice
        else if (c.category == "SP2" && has2sp) {
            relevantConstraints.push_back(c);
        }
        // Always include SP3 since we're testing 3sp
        else if (c.category == "SP3") {
            relevantConstraints.push_back(c);
        }
    }
    
    cout << "\n";
    cout << "================================================================" << endl;
    cout << "DIAGNOSTIC: " << testName << " " << spListToString(spList) << endl;
    cout << "================================================================" << endl;
    cout << "Testing " << relevantConstraints.size() << " constraints (disabling one at a time)" << endl;
    cout << "Timeout per sub-test: " << DIAG_TIMEOUT_SECONDS << "s" << endl;
    cout << endl;
    
    outputFile << "\n================================================================" << endl;
    outputFile << "DIAGNOSTIC: " << testName << " " << spListToString(spList) << endl;
    outputFile << "================================================================" << endl;
    outputFile << "Relevant constraints to test: " << relevantConstraints.size() << endl;
    outputFile << "Timeout per sub-test: " << DIAG_TIMEOUT_SECONDS << "s" << endl;
    outputFile << endl;
    
    // First: run with ALL constraints to confirm timeout
    cout << "[0/" << relevantConstraints.size() << "] Baseline (toutes les contraintes actives)... " << flush;
    fill(activeConstraints.begin(), activeConstraints.end(), true);
    QuickResult baseline = quickDFS(spList, v_type, DIAG_TIMEOUT_SECONDS);
    
    string baselineStatus;
    if (baseline.failed) baselineStatus = "FAILED";
    else if (baseline.timedOut && baseline.solutionsFound == 0) baselineStatus = "TIMEOUT";
    else if (baseline.timedOut) baselineStatus = "TIMEOUT_PARTIAL (" + to_string(baseline.solutionsFound) + " sol)";
    else baselineStatus = "OK (" + to_string(baseline.solutionsFound) + " sol, " + to_string(baseline.bestCost) + " cost)";
    
    cout << baselineStatus << " in " << fixed << setprecision(2) << baseline.elapsedSeconds << "s" << endl;
    outputFile << "BASELINE (all ON): " << baselineStatus << " in " << fixed << setprecision(2) << baseline.elapsedSeconds << "s" << endl;
    outputFile << endl;
    
    if (!baseline.timedOut && !baseline.failed && baseline.solutionsFound > 0) {
        cout << "  => Baseline already works! No need for diagnostic." << endl;
        outputFile << "  => Baseline already works! Skipping diagnostic." << endl;
        return;
    }
    
    // Now disable each constraint one at a time
    vector<DiagResult> diagResults;
    
    outputFile << left << setw(22) << "Constraint" 
               << setw(50) << "Description"
               << setw(12) << "Solutions"
               << setw(10) << "Time(s)"
               << setw(10) << "Cost"
               << setw(12) << "Status" << endl;
    outputFile << string(116, '-') << endl;
    
    for (size_t i = 0; i < relevantConstraints.size(); i++) {
        const auto& cons = relevantConstraints[i];
        
        cout << "[" << (i+1) << "/" << relevantConstraints.size() << "] Disabling " 
             << cons.name << "... " << flush;
        
        // Enable all, then disable just this one
        fill(activeConstraints.begin(), activeConstraints.end(), true);
        activeConstraints[cons.id] = false;
        
        QuickResult qr = quickDFS(spList, v_type, DIAG_TIMEOUT_SECONDS);
        
        diagResults.push_back({cons, qr});
        
        string status;
        if (qr.failed) status = "FAILED";
        else if (qr.timedOut && qr.solutionsFound == 0) status = "TIMEOUT";
        else if (qr.timedOut) status = "PARTIAL";
        else status = "OK";
        
        string costStr = qr.bestCost >= 0 ? to_string(qr.bestCost) : "N/A";
        
        // Console output
        if (qr.solutionsFound > 0 && !qr.timedOut) {
            cout << "\033[32m" << status << "\033[0m"; // Green for success
        } else if (qr.solutionsFound > 0) {
            cout << "\033[33m" << status << "\033[0m"; // Yellow for partial
        } else if (qr.failed) {
            cout << "\033[31m" << status << "\033[0m"; // Red for failed
        } else {
            cout << status;
        }
        cout << " (" << qr.solutionsFound << " sol, " << fixed << setprecision(2) << qr.elapsedSeconds << "s";
        if (qr.bestCost >= 0) cout << ", cost=" << qr.bestCost;
        cout << ")" << endl;
        
        // File output
        outputFile << left << setw(22) << cons.name 
                   << setw(50) << cons.description
                   << setw(12) << qr.solutionsFound
                   << setw(10) << fixed << setprecision(2) << qr.elapsedSeconds
                   << setw(10) << costStr
                   << setw(12) << status << endl;
    }
    
    // === SUMMARY ===
    outputFile << endl;
    outputFile << "--- DIAGNOSTIC SUMMARY ---" << endl;
    
    // Categorize results
    vector<DiagResult> unblocking;   // disabling this constraint unblocks the problem
    vector<DiagResult> helping;      // disabling this finds partial solutions
    vector<DiagResult> noChange;     // still timeout
    vector<DiagResult> breaking;     // makes the problem infeasible
    
    for (const auto& dr : diagResults) {
        if (dr.result.failed) {
            breaking.push_back(dr);
        } else if (!dr.result.timedOut && dr.result.solutionsFound >= MAX_SOLUTIONS) {
            unblocking.push_back(dr);
        } else if (dr.result.solutionsFound > 0) {
            helping.push_back(dr);
        } else {
            noChange.push_back(dr);
        }
    }
    
    // Sort unblocking by time (fastest first)
    sort(unblocking.begin(), unblocking.end(), 
         [](const DiagResult& a, const DiagResult& b) { return a.result.elapsedSeconds < b.result.elapsedSeconds; });
    
    cout << "\n--- DIAGNOSTIC SUMMARY for " << testName << " ---" << endl;
    
    if (!unblocking.empty()) {
        string header = "UNBLOCKING constraints (disabling solves the problem):";
        cout << "\033[32m" << header << "\033[0m" << endl;
        outputFile << header << endl;
        for (const auto& dr : unblocking) {
            string line = "  * " + dr.constraint.name + " (" + dr.constraint.description + ") => " 
                        + to_string(dr.result.solutionsFound) + " solutions in " 
                        + to_string(dr.result.elapsedSeconds).substr(0, 5) + "s, cost=" + to_string(dr.result.bestCost);
            cout << "  \033[32m*\033[0m " << dr.constraint.name << " (" << dr.constraint.description << ") => " 
                 << dr.result.solutionsFound << " solutions in " << fixed << setprecision(2) << dr.result.elapsedSeconds 
                 << "s, cost=" << dr.result.bestCost << endl;
            outputFile << line << endl;
        }
    }
    
    if (!helping.empty()) {
        string header = "HELPING constraints (disabling finds some solutions but still slow):";
        cout << "\033[33m" << header << "\033[0m" << endl;
        outputFile << header << endl;
        for (const auto& dr : helping) {
            cout << "  \033[33m~\033[0m " << dr.constraint.name << " => " 
                 << dr.result.solutionsFound << " solutions in " << fixed << setprecision(2) 
                 << dr.result.elapsedSeconds << "s" << endl;
            outputFile << "  ~ " << dr.constraint.name << " => " << dr.result.solutionsFound 
                      << " solutions in " << fixed << setprecision(2) << dr.result.elapsedSeconds << "s" << endl;
        }
    }
    
    if (!breaking.empty()) {
        string header = "BREAKING constraints (disabling makes problem infeasible):";
        cout << "\033[31m" << header << "\033[0m" << endl;
        outputFile << header << endl;
        for (const auto& dr : breaking) {
            cout << "  \033[31mX\033[0m " << dr.constraint.name << " (" << dr.constraint.description << ")" << endl;
            outputFile << "  X " << dr.constraint.name << " (" << dr.constraint.description << ")" << endl;
        }
    }
    
    if (!noChange.empty()) {
        cout << "NO EFFECT constraints (still timeout when disabled):" << endl;
        outputFile << "NO EFFECT constraints (still timeout when disabled):" << endl;
        for (const auto& dr : noChange) {
            cout << "  - " << dr.constraint.name << endl;
            outputFile << "  - " << dr.constraint.name << endl;
        }
    }
    
    outputFile << endl;
}

// =============================================================================
// Screening: quickly find which combinations timeout
// =============================================================================

struct ScreenResult {
    string name;
    vector<Species> spList;
    bool needsDiag;
    string baselineStatus;
};

vector<ScreenResult> screenTimeouts() {
    // All combinations with 2+ third species that were timing out
    vector<pair<string, vector<Species>>> toScreen = {
        {"4v_1sp_3sp_3sp",   {FIRST_SPECIES, THIRD_SPECIES, THIRD_SPECIES}},
        {"4v_2sp_3sp_3sp",   {SECOND_SPECIES, THIRD_SPECIES, THIRD_SPECIES}},
        {"4v_3sp_1sp_3sp",   {THIRD_SPECIES, FIRST_SPECIES, THIRD_SPECIES}},
        {"4v_3sp_2sp_3sp",   {THIRD_SPECIES, SECOND_SPECIES, THIRD_SPECIES}},
        {"4v_3sp_3sp_2sp",   {THIRD_SPECIES, THIRD_SPECIES, SECOND_SPECIES}},
        {"4v_3sp_3sp_3sp",   {THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES}},
    };
    
    vector<ScreenResult> results;
    
    cout << "\n=== SCREENING: Confirming which combinations timeout ===" << endl;
    cout << "Timeout: " << SCREEN_TIMEOUT_SECONDS << "s" << endl;
    
    outputFile << "\n=== SCREENING PHASE ===" << endl;
    outputFile << "Timeout: " << SCREEN_TIMEOUT_SECONDS << "s" << endl << endl;
    
    for (const auto& test : toScreen) {
        cout << "Screening " << test.first << " " << spListToString(test.second) << "... " << flush;
        
        fill(activeConstraints.begin(), activeConstraints.end(), true);
        
        QuickResult qr = quickDFS(test.second, {0, 1, 2}, SCREEN_TIMEOUT_SECONDS);
        string status;
        if (qr.failed) status = "FAILED";
        else if (qr.timedOut && qr.solutionsFound == 0) status = "TIMEOUT";
        else status = "OK (" + to_string(qr.solutionsFound) + " sol in " + to_string(qr.elapsedSeconds).substr(0,5) + "s)";
        
        ScreenResult sr;
        sr.name = test.first;
        sr.spList = test.second;
        sr.needsDiag = (qr.timedOut && qr.solutionsFound == 0) || qr.failed;
        sr.baselineStatus = status;
        
        results.push_back(sr);
        
        cout << status << endl;
        outputFile << test.first << " " << spListToString(test.second) << ": " << status << endl;
    }
    
    return results;
}

// =============================================================================
// Main
// =============================================================================

int main(int argc, char* argv[]) {
    cout << "================================================================" << endl;
    cout << " CONSTRAINT DIAGNOSTIC TEST - 4 Voice + 3rd Species" << endl;
    cout << "================================================================" << endl;
    cout << endl;
    
    string filename = "../results/diagnostic_troisieme_espece.txt";
    
    // Get date
    time_t now = time(0);
    char* dt = ctime(&now);
    
    outputFile.open(filename);
    outputFile << "================================================================" << endl;
    outputFile << " CONSTRAINT DIAGNOSTIC TEST - 4 Voice + 3rd Species" << endl;
    outputFile << "================================================================" << endl;
    outputFile << "Date: " << dt;
    outputFile << "Diagnostic timeout: " << DIAG_TIMEOUT_SECONDS << "s per constraint test" << endl;
    outputFile << "Screening timeout: " << SCREEN_TIMEOUT_SECONDS << "s" << endl;
    outputFile << "Max solutions to find: " << MAX_SOLUTIONS << endl;
    outputFile << endl;
    
    if (argc >= 2) {
        string arg = argv[1];
        
        if (arg == "all") {
            // Screen then diagnose all timeouts
            vector<ScreenResult> screenResults = screenTimeouts();
            
            int diagCount = 0;
            for (const auto& sr : screenResults) {
                if (sr.needsDiag) diagCount++;
            }
            
            cout << "\n" << diagCount << " combinations need diagnosis." << endl;
            outputFile << "\n" << diagCount << " combinations need diagnosis." << endl;
            
            for (const auto& sr : screenResults) {
                if (sr.needsDiag) {
                    runDiagnostic(sr.name, sr.spList, {0, 1, 2});
                }
            }
        }
        else if (arg == "screen") {
            screenTimeouts();
        }
        else {
            // Parse specific combo like "1_3_3" or "3_3_3"
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
                runDiagnostic(name, spList, {0, 1, 2});
            } else {
                cout << "Invalid argument: " << arg << endl;
                cout << endl;
                cout << "Usage:" << endl;
                cout << "  ./DiagnosticTest all        - Screen all timeout cases then diagnose" << endl;
                cout << "  ./DiagnosticTest screen     - Only screen (confirm timeouts)" << endl;
                cout << "  ./DiagnosticTest X_Y_Z      - Diagnose specific combo (e.g. 3_3_3, 1_3_3)" << endl;
                cout << endl;
                cout << "Examples:" << endl;
                cout << "  ./DiagnosticTest 3_3_3      - Diagnose {3sp, 3sp, 3sp}" << endl;
                cout << "  ./DiagnosticTest 1_3_3      - Diagnose {1sp, 3sp, 3sp}" << endl;
                cout << "  ./DiagnosticTest all        - Full automatic diagnosis" << endl;
                outputFile.close();
                return 1;
            }
        }
    } else {
        cout << "Usage:" << endl;
        cout << "  ./DiagnosticTest all        - Screen all timeout cases then diagnose" << endl;
        cout << "  ./DiagnosticTest screen     - Only screen (confirm timeouts)" << endl;
        cout << "  ./DiagnosticTest X_Y_Z      - Diagnose specific combo (e.g. 3_3_3, 1_3_3)" << endl;
        outputFile.close();
        return 0;
    }
    
    // Final summary
    outputFile << "\n================================================================" << endl;
    outputFile << "DIAGNOSTIC COMPLETE" << endl;
    outputFile << "================================================================" << endl;
    
    outputFile.close();
    
    cout << "\n================================================================" << endl;
    cout << "DIAGNOSTIC COMPLETE - Results saved to: " << filename << endl;
    cout << "================================================================" << endl;
    
    return 0;
}
