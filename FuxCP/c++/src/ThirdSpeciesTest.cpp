// 
// Created by Sacha
// This file tests 4-voice counterpoint with various combinations including THIRD_SPECIES
// to identify which combinations cause infinite search times.
//

#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <ctime>
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

const int TIMEOUT_SECONDS = 30;           // Timeout par test (30 secondes)
const int MAX_SOLUTIONS_TO_FIND = 10;     // Nombre de solutions à trouver
const int PROGRESS_INTERVAL = 1000;       // Afficher le progrès toutes les N itérations

// Cantus firmus par défaut
vector<int> cantusFirmus = {60, 62, 65, 64, 67, 65, 64, 62, 60};

// Paramètres par défaut (fixes pour tous les tests)
vector<int> melodic_params = {0, 1, 2, 576, 5, 10, 25, 40};
vector<int> general_params = {4, 1, 1, 2, 2, 2, 8, 1};
vector<int> specific_params = {8, 4, 0, 2, 1, 8, 50};
vector<int> importance = {8, 7, 5, 2, 9, 3, 14, 12, 6, 11, 4, 10, 1, 13};
int borrowMode = 1;

// Fichier de sortie
ofstream outputFile;

// =============================================================================
// Structure pour stocker les résultats
// =============================================================================

struct TestResult {
    string testName;
    string searchMethod;        // "BAB" ou "DFS"
    vector<Species> spList;
    int solutionsFound;
    int iterationsCount;
    double elapsedSeconds;
    bool timedOut;
    string status;              // "OK", "SLOW", "TIMEOUT", "NO_SOLUTION", "FAILED"
    int bestCost;               // Meilleur coût trouvé (-1 si aucun)
    int problemSize;            // Nombre de variables
    bool problemFailed;         // Si le problème est infaisable dès le départ
};

// =============================================================================
// Fonctions utilitaires
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

string vectorToString(const vector<int>& vec) {
    string result = "{";
    for (size_t i = 0; i < vec.size(); i++) {
        result += to_string(vec[i]);
        if (i < vec.size() - 1) result += ", ";
    }
    result += "}";
    return result;
}

void printSeparator() {
    cout << "========================================================================" << endl;
}

void printTestHeader(const string& testName, const vector<Species>& spList, const string& method) {
    cout << "\n";
    printSeparator();
    cout << "TEST: " << testName << " [" << method << "]" << endl;
    cout << "spList = " << spListToString(spList) << endl;
    cout << "Timeout: " << TIMEOUT_SECONDS << "s" << endl;
    printSeparator();
}

void logToFile(const string& message) {
    if (outputFile.is_open()) {
        outputFile << message << endl;
    }
    cout << message << endl;
}

void writeResultToFile(const TestResult& result) {
    if (!outputFile.is_open()) return;
    
    outputFile << "---" << endl;
    outputFile << "Test: " << result.testName << endl;
    outputFile << "Method: " << result.searchMethod << endl;
    outputFile << "spList: " << spListToString(result.spList) << endl;
    outputFile << "Problem Size: " << result.problemSize << " variables" << endl;
    outputFile << "Problem Failed at Creation: " << (result.problemFailed ? "YES" : "NO") << endl;
    outputFile << "Solutions Found: " << result.solutionsFound << endl;
    outputFile << "Iterations: " << result.iterationsCount << endl;
    outputFile << "Time: " << fixed << setprecision(2) << result.elapsedSeconds << "s" << endl;
    outputFile << "Best Cost: " << (result.bestCost >= 0 ? to_string(result.bestCost) : "N/A") << endl;
    outputFile << "Status: " << result.status << endl;
    outputFile << "Timed Out: " << (result.timedOut ? "YES" : "NO") << endl;
    outputFile << endl;
}

// =============================================================================
// Fonction principale de test - BAB (Branch and Bound)
// =============================================================================

TestResult runTestBAB(const string& testName, const vector<Species>& spList, const vector<int>& v_type) {
    TestResult result;
    result.testName = testName;
    result.searchMethod = "BAB";
    result.spList = spList;
    result.solutionsFound = 0;
    result.iterationsCount = 0;
    result.timedOut = false;
    result.bestCost = -1;
    result.problemFailed = false;
    
    printTestHeader(testName, spList, "BAB");
    
    // Activer toutes les contraintes
    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    
    // Créer le problème
    auto* problem = create_problem(cantusFirmus, spList, v_type, 
                                   melodic_params, general_params, 
                                   specific_params, importance, borrowMode);
    
    if (!problem) {
        cout << "ERROR: Failed to create problem" << endl;
        result.status = "ERROR";
        result.elapsedSeconds = 0;
        result.problemSize = 0;
        return result;
    }
    
    result.problemSize = problem->getSize();
    
    // Vérifier si le problème est faisable
    if (problem->getHome().failed()) {
        cout << "PROBLEM FAILED at creation - infeasible!" << endl;
        result.problemFailed = true;
        result.status = "FAILED";
        result.elapsedSeconds = 0;
        delete problem;
        return result;
    }
    
    cout << "Problem size: " << result.problemSize << " variables" << endl;
    
    // Configuration avec timeout
    Search::Options opts;
    Search::TimeStop stop(TIMEOUT_SECONDS * 1000);
    opts.stop = &stop;
    
    // Lancer la recherche BAB
    BAB<CounterpointProblem> engine(problem, opts);
    
    auto startTime = std::chrono::steady_clock::now();
    CounterpointProblem* best = nullptr;
    
    while (CounterpointProblem* s = engine.next()) {
        result.iterationsCount++;
        result.solutionsFound++;
        
        // Garder le meilleur
        delete best;
        best = s;
        
        // Calculer le coût
        int currentCost = 0;
        for (int i = 0; i < s->cost().size(); i++) {
            currentCost += s->cost()[i].val();
        }
        result.bestCost = currentCost;
        
        if (result.iterationsCount % PROGRESS_INTERVAL == 0) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            cout << "[" << elapsed << "s] Iteration " << result.iterationsCount 
                 << ", Best cost: " << result.bestCost << endl;
        }
        
        // Afficher la première solution
        if (result.solutionsFound == 1) {
            cout << "First solution found! Cost: " << result.bestCost << endl;
        }
        
        if (result.solutionsFound >= MAX_SOLUTIONS_TO_FIND) {
            cout << "Found " << MAX_SOLUTIONS_TO_FIND << " solutions, stopping." << endl;
            break;
        }
    }
    
    delete best;
    
    auto endTime = std::chrono::steady_clock::now();
    result.elapsedSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    
    // Déterminer le statut
    if (engine.stopped() && result.solutionsFound < MAX_SOLUTIONS_TO_FIND) {
        result.timedOut = true;
        result.status = (result.solutionsFound > 0) ? "TIMEOUT_PARTIAL" : "TIMEOUT";
    } else if (result.solutionsFound >= MAX_SOLUTIONS_TO_FIND) {
        result.status = "OK";
    } else if (result.solutionsFound > 0) {
        result.status = "COMPLETE";
    } else {
        result.status = "NO_SOLUTION";
    }
    
    // Afficher le résultat
    cout << "\n--- Result [BAB] ---" << endl;
    cout << "Solutions: " << result.solutionsFound << ", Iterations: " << result.iterationsCount << endl;
    cout << "Time: " << result.elapsedSeconds << "s, Best cost: " << result.bestCost << endl;
    cout << "Status: " << result.status << endl;
    
    delete problem;
    writeResultToFile(result);
    return result;
}

// =============================================================================
// Fonction principale de test - DFS (Depth First Search) avec tracking du meilleur
// =============================================================================

TestResult runTestDFS(const string& testName, const vector<Species>& spList, const vector<int>& v_type) {
    TestResult result;
    result.testName = testName;
    result.searchMethod = "DFS";
    result.spList = spList;
    result.solutionsFound = 0;
    result.iterationsCount = 0;
    result.timedOut = false;
    result.bestCost = -1;
    result.problemFailed = false;
    
    printTestHeader(testName, spList, "DFS");
    
    // Activer toutes les contraintes
    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    
    // Créer le problème
    auto* problem = create_problem(cantusFirmus, spList, v_type, 
                                   melodic_params, general_params, 
                                   specific_params, importance, borrowMode);
    
    if (!problem) {
        cout << "ERROR: Failed to create problem" << endl;
        result.status = "ERROR";
        result.elapsedSeconds = 0;
        result.problemSize = 0;
        return result;
    }
    
    result.problemSize = problem->getSize();
    
    // Vérifier si le problème est faisable
    if (problem->getHome().failed()) {
        cout << "PROBLEM FAILED at creation - infeasible!" << endl;
        result.problemFailed = true;
        result.status = "FAILED";
        result.elapsedSeconds = 0;
        delete problem;
        return result;
    }
    
    cout << "Problem size: " << result.problemSize << " variables" << endl;
    
    // Configuration avec timeout
    Search::Options opts;
    Search::TimeStop stop(TIMEOUT_SECONDS * 1000);
    opts.stop = &stop;
    
    // Lancer la recherche DFS
    DFS<CounterpointProblem> engine(problem, opts);
    
    auto startTime = std::chrono::steady_clock::now();
    int bestCostSoFar = INT_MAX;
    
    while (CounterpointProblem* s = engine.next()) {
        result.iterationsCount++;
        result.solutionsFound++;
        
        // Calculer le coût de cette solution
        int currentCost = 0;
        for (int i = 0; i < s->cost().size(); i++) {
            currentCost += s->cost()[i].val();
        }
        
        // Tracker le meilleur coût
        if (currentCost < bestCostSoFar) {
            bestCostSoFar = currentCost;
            result.bestCost = currentCost;
            cout << "New best cost: " << currentCost << " at iteration " << result.iterationsCount << endl;
        }
        
        delete s;
        
        if (result.iterationsCount % PROGRESS_INTERVAL == 0) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
            cout << "[" << elapsed << "s] Iteration " << result.iterationsCount 
                 << ", Best cost so far: " << result.bestCost << endl;
        }
        
        // Afficher info première solution
        if (result.solutionsFound == 1) {
            cout << "First solution found! Cost: " << currentCost << endl;
        }
        
        if (result.solutionsFound >= MAX_SOLUTIONS_TO_FIND) {
            cout << "Found " << MAX_SOLUTIONS_TO_FIND << " solutions, stopping." << endl;
            break;
        }
    }
    
    auto endTime = std::chrono::steady_clock::now();
    result.elapsedSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.0;
    
    // Déterminer le statut
    if (engine.stopped() && result.solutionsFound < MAX_SOLUTIONS_TO_FIND) {
        result.timedOut = true;
        result.status = (result.solutionsFound > 0) ? "TIMEOUT_PARTIAL" : "TIMEOUT";
    } else if (result.solutionsFound >= MAX_SOLUTIONS_TO_FIND) {
        result.status = "OK";
    } else if (result.solutionsFound > 0) {
        result.status = "COMPLETE";
    } else {
        result.status = "NO_SOLUTION";
    }
    
    // Afficher le résultat
    cout << "\n--- Result [DFS] ---" << endl;
    cout << "Solutions: " << result.solutionsFound << ", Iterations: " << result.iterationsCount << endl;
    cout << "Time: " << result.elapsedSeconds << "s, Best cost: " << result.bestCost << endl;
    cout << "Status: " << result.status << endl;
    
    delete problem;
    writeResultToFile(result);
    return result;
}

// =============================================================================
// Fonction pour tester une combinaison avec les deux méthodes
// =============================================================================

pair<TestResult, TestResult> runBothMethods(const string& testName, 
                                             const vector<Species>& spList, 
                                             const vector<int>& v_type) {
    TestResult babResult = runTestBAB(testName, spList, v_type);
    TestResult dfsResult = runTestDFS(testName, spList, v_type);
    return make_pair(babResult, dfsResult);
}

// =============================================================================
// Génération de toutes les combinaisons 4 voix incluant THIRD_SPECIES
// =============================================================================

struct TestCase {
    string name;
    vector<Species> spList;
    vector<int> v_type;
};

vector<TestCase> generateAllTestCases() {
    vector<TestCase> cases;
    // Seulement les espèces 1, 2 et 3 (pas 4 et 5)
    vector<Species> allSpecies = {FIRST_SPECIES, SECOND_SPECIES, THIRD_SPECIES};
    vector<int> defaultVType = {0, 1, 2};
    
    // Générer toutes les combinaisons de 3 espèces (pour 4 voix = CF + 3 contrepoints)
    for (Species s1 : allSpecies) {
        for (Species s2 : allSpecies) {
            for (Species s3 : allSpecies) {
                // On ne garde que les combinaisons incluant au moins une THIRD_SPECIES
                if (s1 == THIRD_SPECIES || s2 == THIRD_SPECIES || s3 == THIRD_SPECIES) {
                    string name = "4v_" + speciesName(s1) + "_" + speciesName(s2) + "_" + speciesName(s3);
                    cases.push_back({name, {s1, s2, s3}, defaultVType});
                }
            }
        }
    }
    
    return cases;
}

// =============================================================================
// Écriture de l'en-tête du fichier de résultats
// =============================================================================

void writeFileHeader() {
    if (!outputFile.is_open()) return;
    
    // Obtenir la date/heure actuelle
    time_t now = time(0);
    char* dt = ctime(&now);
    
    outputFile << "================================================================================" << endl;
    outputFile << "THIRD SPECIES PERFORMANCE TEST SUITE - 4 VOICE COUNTERPOINT" << endl;
    outputFile << "================================================================================" << endl;
    outputFile << "Date: " << dt;
    outputFile << "Timeout per test: " << TIMEOUT_SECONDS << " seconds" << endl;
    outputFile << "Max solutions to find: " << MAX_SOLUTIONS_TO_FIND << endl;
    outputFile << endl;
    
    outputFile << "=== PARAMETERS USED ===" << endl;
    outputFile << "Cantus Firmus: " << vectorToString(cantusFirmus) << endl;
    outputFile << "CF Length: " << cantusFirmus.size() << " measures" << endl;
    outputFile << "melodic_params: " << vectorToString(melodic_params) << endl;
    outputFile << "general_params: " << vectorToString(general_params) << endl;
    outputFile << "specific_params: " << vectorToString(specific_params) << endl;
    outputFile << "importance: " << vectorToString(importance) << endl;
    outputFile << "borrowMode: " << borrowMode << endl;
    outputFile << endl;
    
    outputFile << "=== PARAMETER IMPACT ANALYSIS ===" << endl;
    outputFile << "v_type: Defines pitch range for each voice. STRONG impact on search space." << endl;
    outputFile << "melodic_params: Melodic cost weights. MEDIUM impact (affects BAB convergence)." << endl;
    outputFile << "general_params: General constraints weights. MEDIUM impact." << endl;
    outputFile << "specific_params: Species-specific constraints. MEDIUM impact." << endl;
    outputFile << "importance: Cost evaluation order. LOW impact on feasibility." << endl;
    outputFile << endl;
    
    outputFile << "=== TEST RESULTS ===" << endl;
    outputFile << endl;
}

void writeFileSummary(const vector<pair<TestResult, TestResult>>& allResults) {
    if (!outputFile.is_open()) return;
    
    outputFile << endl;
    outputFile << "================================================================================" << endl;
    outputFile << "SUMMARY TABLE" << endl;
    outputFile << "================================================================================" << endl;
    outputFile << endl;
    
    // En-tête du tableau
    outputFile << left << setw(25) << "Test" 
               << setw(8) << "Method" 
               << setw(10) << "Solutions" 
               << setw(12) << "Iterations"
               << setw(10) << "Time(s)" 
               << setw(12) << "Best Cost"
               << setw(15) << "Status" << endl;
    outputFile << string(92, '-') << endl;
    
    int babOK = 0, babTimeout = 0, babFailed = 0;
    int dfsOK = 0, dfsTimeout = 0, dfsFailed = 0;
    
    for (const auto& pair : allResults) {
        const TestResult& bab = pair.first;
        const TestResult& dfs = pair.second;
        
        // BAB line
        outputFile << left << setw(25) << bab.testName 
                   << setw(8) << "BAB"
                   << setw(10) << bab.solutionsFound 
                   << setw(12) << bab.iterationsCount
                   << setw(10) << fixed << setprecision(2) << bab.elapsedSeconds
                   << setw(12) << (bab.bestCost >= 0 ? to_string(bab.bestCost) : "N/A")
                   << setw(15) << bab.status << endl;
        
        // DFS line
        outputFile << left << setw(25) << ""
                   << setw(8) << "DFS"
                   << setw(10) << dfs.solutionsFound 
                   << setw(12) << dfs.iterationsCount
                   << setw(10) << fixed << setprecision(2) << dfs.elapsedSeconds
                   << setw(12) << (dfs.bestCost >= 0 ? to_string(dfs.bestCost) : "N/A")
                   << setw(15) << dfs.status << endl;
        
        outputFile << string(92, '-') << endl;
        
        // Comptage
        if (bab.status == "OK" || bab.status == "COMPLETE") babOK++;
        else if (bab.status.find("TIMEOUT") != string::npos) babTimeout++;
        else babFailed++;
        
        if (dfs.status == "OK" || dfs.status == "COMPLETE") dfsOK++;
        else if (dfs.status.find("TIMEOUT") != string::npos) dfsTimeout++;
        else dfsFailed++;
    }
    
    outputFile << endl;
    outputFile << "=== STATISTICS ===" << endl;
    outputFile << "Total tests per method: " << allResults.size() << endl;
    outputFile << endl;
    outputFile << "BAB Results:" << endl;
    outputFile << "  - OK/Complete: " << babOK << endl;
    outputFile << "  - Timeout: " << babTimeout << endl;
    outputFile << "  - Failed/No Solution: " << babFailed << endl;
    outputFile << endl;
    outputFile << "DFS Results:" << endl;
    outputFile << "  - OK/Complete: " << dfsOK << endl;
    outputFile << "  - Timeout: " << dfsTimeout << endl;
    outputFile << "  - Failed/No Solution: " << dfsFailed << endl;
    outputFile << endl;
    
    outputFile << "=== CONCLUSION ===" << endl;
    if (dfsOK > babOK) {
        outputFile << "DFS found more complete solutions than BAB." << endl;
    } else if (babOK > dfsOK) {
        outputFile << "BAB found more complete solutions than DFS." << endl;
    } else {
        outputFile << "Both methods had similar success rates." << endl;
    }
}

// =============================================================================
// Fonction principale - Lancer tous les tests
// =============================================================================

void runCompleteTestSuite() {
    // Ouvrir le fichier de sortie
    string filename = "third_species_test_results.txt";
    outputFile.open(filename);
    
    if (!outputFile.is_open()) {
        cerr << "ERROR: Cannot open output file " << filename << endl;
        return;
    }
    
    cout << "Results will be saved to: " << filename << endl;
    
    // Écrire l'en-tête
    writeFileHeader();
    
    // Générer tous les cas de test
    vector<TestCase> testCases = generateAllTestCases();
    cout << "Generated " << testCases.size() << " test cases" << endl;
    
    outputFile << "Number of test cases: " << testCases.size() << endl;
    outputFile << endl;
    
    // Exécuter tous les tests
    vector<pair<TestResult, TestResult>> allResults;
    
    int testNum = 1;
    for (const auto& tc : testCases) {
        cout << "\n========== TEST " << testNum << "/" << testCases.size() << " ==========" << endl;
        outputFile << "========== TEST " << testNum << "/" << testCases.size() << " ==========" << endl;
        
        auto results = runBothMethods(tc.name, tc.spList, tc.v_type);
        allResults.push_back(results);
        
        testNum++;
    }
    
    // Écrire le résumé
    writeFileSummary(allResults);
    
    outputFile.close();
    
    cout << "\n\n========================================" << endl;
    cout << "TEST SUITE COMPLETE!" << endl;
    cout << "Results saved to: " << filename << endl;
    cout << "========================================" << endl;
}

// =============================================================================
// Fonctions de test individuelles (conservées pour compatibilité)
// =============================================================================

void test_4v_3sp_3sp_3sp() {
    vector<Species> spList = {THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_3sp_3sp", spList, v_type);
}

void test_4v_3sp_3sp_1sp() {
    vector<Species> spList = {THIRD_SPECIES, THIRD_SPECIES, FIRST_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_3sp_1sp", spList, v_type);
}

void test_4v_3sp_1sp_3sp() {
    vector<Species> spList = {THIRD_SPECIES, FIRST_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_1sp_3sp", spList, v_type);
}

void test_4v_1sp_3sp_3sp() {
    vector<Species> spList = {FIRST_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_1sp_3sp_3sp", spList, v_type);
}

void test_4v_3sp_1sp_1sp() {
    vector<Species> spList = {THIRD_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_1sp_1sp", spList, v_type);
}

void test_4v_1sp_3sp_1sp() {
    vector<Species> spList = {FIRST_SPECIES, THIRD_SPECIES, FIRST_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_1sp_3sp_1sp", spList, v_type);
}

void test_4v_1sp_1sp_3sp() {
    vector<Species> spList = {FIRST_SPECIES, FIRST_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_1sp_1sp_3sp", spList, v_type);
}

void test_4v_3sp_3sp_2sp() {
    vector<Species> spList = {THIRD_SPECIES, THIRD_SPECIES, SECOND_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_3sp_2sp", spList, v_type);
}

void test_4v_3sp_2sp_3sp() {
    vector<Species> spList = {THIRD_SPECIES, SECOND_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_2sp_3sp", spList, v_type);
}

void test_4v_2sp_3sp_3sp() {
    vector<Species> spList = {SECOND_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_2sp_3sp_3sp", spList, v_type);
}

void test_4v_3sp_2sp_2sp() {
    vector<Species> spList = {THIRD_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_2sp_2sp", spList, v_type);
}

void test_4v_2sp_3sp_2sp() {
    vector<Species> spList = {SECOND_SPECIES, THIRD_SPECIES, SECOND_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_2sp_3sp_2sp", spList, v_type);
}

void test_4v_2sp_2sp_3sp() {
    vector<Species> spList = {SECOND_SPECIES, SECOND_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_2sp_2sp_3sp", spList, v_type);
}

void test_4v_3sp_3sp_4sp() {
    vector<Species> spList = {THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_3sp_4sp", spList, v_type);
}

void test_4v_3sp_4sp_3sp() {
    vector<Species> spList = {THIRD_SPECIES, FOURTH_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_4sp_3sp", spList, v_type);
}

void test_4v_4sp_3sp_3sp() {
    vector<Species> spList = {FOURTH_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_4sp_3sp_3sp", spList, v_type);
}

void test_4v_3sp_2sp_1sp() {
    vector<Species> spList = {THIRD_SPECIES, SECOND_SPECIES, FIRST_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_2sp_1sp", spList, v_type);
}

void test_4v_3sp_1sp_2sp() {
    vector<Species> spList = {THIRD_SPECIES, FIRST_SPECIES, SECOND_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_3sp_1sp_2sp", spList, v_type);
}

void test_4v_1sp_3sp_2sp() {
    vector<Species> spList = {FIRST_SPECIES, THIRD_SPECIES, SECOND_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_1sp_3sp_2sp", spList, v_type);
}

void test_4v_2sp_3sp_1sp() {
    vector<Species> spList = {SECOND_SPECIES, THIRD_SPECIES, FIRST_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_2sp_3sp_1sp", spList, v_type);
}

void test_4v_1sp_2sp_3sp() {
    vector<Species> spList = {FIRST_SPECIES, SECOND_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_1sp_2sp_3sp", spList, v_type);
}

void test_4v_2sp_1sp_3sp() {
    vector<Species> spList = {SECOND_SPECIES, FIRST_SPECIES, THIRD_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_2sp_1sp_3sp", spList, v_type);
}

// Tests de référence
void test_4v_1sp_1sp_1sp() {
    vector<Species> spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_1sp_1sp_1sp (ref)", spList, v_type);
}

void test_4v_2sp_2sp_2sp() {
    vector<Species> spList = {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    vector<int> v_type = {0, 1, 2};
    runBothMethods("4v_2sp_2sp_2sp (ref)", spList, v_type);
}

// =============================================================================
// Menu et fonctions de lancement
// =============================================================================

void printMenu() {
    cout << "\n";
    printSeparator();
    cout << "THIRD SPECIES TEST SUITE - 4 VOICE COUNTERPOINT" << endl;
    printSeparator();
    cout << "Tests disponibles:" << endl;
    cout << "  all         - Lancer TOUS les tests (toutes combinaisons)" << endl;
    cout << "  quick       - Tests rapides (une seule 3sp)" << endl;
    cout << "  full3sp     - Tests avec plusieurs 3sp" << endl;
    cout << "  ref         - Tests de référence (sans 3sp)" << endl;
    cout << endl;
    cout << "Tests individuels (format: X_Y_Z):" << endl;
    cout << "  3_3_3, 3_3_1, 3_1_3, 1_3_3, etc." << endl;
    cout << endl;
    cout << "Timeout: " << TIMEOUT_SECONDS << " seconds per method" << endl;
    cout << "Output file: third_species_test_results.txt" << endl;
    printSeparator();
}

void runQuickTests() {
    string filename = "third_species_quick_results.txt";
    outputFile.open(filename);
    writeFileHeader();
    
    vector<pair<TestResult, TestResult>> results;
    results.push_back(runBothMethods("4v_3sp_1sp_1sp", {THIRD_SPECIES, FIRST_SPECIES, FIRST_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_1sp_3sp_1sp", {FIRST_SPECIES, THIRD_SPECIES, FIRST_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_1sp_1sp_3sp", {FIRST_SPECIES, FIRST_SPECIES, THIRD_SPECIES}, {0, 1, 2}));
    
    writeFileSummary(results);
    outputFile.close();
    cout << "Results saved to: " << filename << endl;
}

void runFull3spTests() {
    string filename = "third_species_full3sp_results.txt";
    outputFile.open(filename);
    writeFileHeader();
    
    vector<pair<TestResult, TestResult>> results;
    results.push_back(runBothMethods("4v_3sp_3sp_3sp", {THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_3sp_3sp_1sp", {THIRD_SPECIES, THIRD_SPECIES, FIRST_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_3sp_1sp_3sp", {THIRD_SPECIES, FIRST_SPECIES, THIRD_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_1sp_3sp_3sp", {FIRST_SPECIES, THIRD_SPECIES, THIRD_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_3sp_3sp_2sp", {THIRD_SPECIES, THIRD_SPECIES, SECOND_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_3sp_2sp_3sp", {THIRD_SPECIES, SECOND_SPECIES, THIRD_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_2sp_3sp_3sp", {SECOND_SPECIES, THIRD_SPECIES, THIRD_SPECIES}, {0, 1, 2}));
    
    writeFileSummary(results);
    outputFile.close();
    cout << "Results saved to: " << filename << endl;
}

void runReferenceTests() {
    string filename = "third_species_ref_results.txt";
    outputFile.open(filename);
    writeFileHeader();
    
    vector<pair<TestResult, TestResult>> results;
    results.push_back(runBothMethods("4v_1sp_1sp_1sp", {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES}, {0, 1, 2}));
    results.push_back(runBothMethods("4v_2sp_2sp_2sp", {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES}, {0, 1, 2}));
    
    writeFileSummary(results);
    outputFile.close();
    cout << "Results saved to: " << filename << endl;
}

int main(int argc, char* argv[]) {
    cout << "Third Species Performance Test Suite" << endl;
    cout << "=====================================" << endl;
    
    if (argc < 2) {
        printMenu();
        return 0;
    }
    
    string arg = argv[1];
    
    if (arg == "all") {
        runCompleteTestSuite();
    } else if (arg == "quick") {
        runQuickTests();
    } else if (arg == "full3sp") {
        runFull3spTests();
    } else if (arg == "ref") {
        runReferenceTests();
    }
    // Tests individuels
    else if (arg == "3_3_3") {
        test_4v_3sp_3sp_3sp();
    } else if (arg == "3_3_1") {
        test_4v_3sp_3sp_1sp();
    } else if (arg == "3_1_3") {
        test_4v_3sp_1sp_3sp();
    } else if (arg == "1_3_3") {
        test_4v_1sp_3sp_3sp();
    } else if (arg == "3_3_2") {
        test_4v_3sp_3sp_2sp();
    } else if (arg == "3_2_3") {
        test_4v_3sp_2sp_3sp();
    } else if (arg == "2_3_3") {
        test_4v_2sp_3sp_3sp();
    } else if (arg == "3_1_1") {
        test_4v_3sp_1sp_1sp();
    } else if (arg == "1_3_1") {
        test_4v_1sp_3sp_1sp();
    } else if (arg == "1_1_3") {
        test_4v_1sp_1sp_3sp();
    } else if (arg == "3_2_2") {
        test_4v_3sp_2sp_2sp();
    } else if (arg == "2_3_2") {
        test_4v_2sp_3sp_2sp();
    } else if (arg == "2_2_3") {
        test_4v_2sp_2sp_3sp();
    } else if (arg == "3_2_1") {
        test_4v_3sp_2sp_1sp();
    } else if (arg == "1_2_3") {
        test_4v_1sp_2sp_3sp();
    } else if (arg == "3_3_4") {
        test_4v_3sp_3sp_4sp();
    } else if (arg == "3_4_3") {
        test_4v_3sp_4sp_3sp();
    } else if (arg == "4_3_3") {
        test_4v_4sp_3sp_3sp();
    }
    else {
        cout << "Argument non reconnu: " << arg << endl;
        printMenu();
        return 1;
    }
    
    return 0;
}
