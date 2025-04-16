#include "../headers/figureTests.hpp"
#include <iostream>
#include <queue>

using namespace std;

FigureTests::FigureTests() {
    borrowMode = 0;
    melodic_params = {0, 1, 1, 576, 2, 2, 2, 1};
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8, 4, 0, 2, 1, 8, 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
}

std::vector<CounterpointProblem*> FigureTests::get_all_solutions() {
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    for (size_t i = 0; i < cp.size(); i++) { 
        int note = cp[i];
        if (note >  0) {
            rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, note);
        }
    }
    std::vector<CounterpointProblem*> solutions;
    DFS<CounterpointProblem> e(problem);
    while (CounterpointProblem* pb = e.next()) {
        cout << "Solution found" << endl;
        solutions.push_back(pb);
        cout << pb->getSolutionArray() << endl;
        cout << "Solution added" << endl;
    }
    return solutions;
}

void FigureTests::test_configuration() {
    activeConstraints = std::vector<bool>(activeConstraints.size(), false);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    for (size_t i = 0; i < cp.size(); i++) { 
        int note = cp[i];
        if (note >  0) {
            rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, note);
        }
    }
    if (!has_solution(problem)) {
        std::cerr   << "\t /!\\ ERROR: It doesn't exist a solution but it should with the following configuration:" << endl;
    } else {
        cout << "\t Test passed with the following configuration:" << endl;
    }
    cout << "\t Cantus firmus: ";
    printVector(cantusFirmus);
    cout << "\t Solution array: ";
    printIntVarArray(problem->getSolutionArray());
    // cout << problem->getCounterpoint_1()->getBranchingNotes() << endl;
    delete problem;
}

/*
* Check if the problem is unsatisfiable
* @return true if the problem is unsatisfiable, false otherwise
*/
bool FigureTests::is_unsat(const set<int>& cons_set) {
    // set active cons_set
    activeConstraints = std::vector<bool>(activeConstraints.size(), false);
    for (int cons : cons_set) {
        activeConstraints[cons] = true;
    }
    // create a new problem and set the solution array
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    for (size_t i = 0; i < cp.size(); i++) { 
        int note = cp[i];
        if (note >  0) {
            rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, note);
        }
    }
    // check if the problem is unsatisfiable
    bool unsat = !has_solution(problem);
    delete problem;
    return unsat;
}

/**
 * Minimize the set of constraints by removing one constraint at a time
 * until the problem becomes satisfiable.
 * @param cons_set the set of constraints to minimize
 * @return the minimized set of constraints
 */
std::set<int> FigureTests::minimize(const std::set<int>& cons_set) {
    std::set<int> mus = cons_set;
    for (auto it = mus.begin(); it != mus.end(); ) {
        set<int> trial = mus;
        trial.erase(*it);
        if (is_unsat(trial)) {
            mus = trial;
            it = mus.begin(); // restart
        } else {
            it++;
        }
    }
    return mus;
}

/**
 * Find all minimal unsatisfiable subsets (MUSes) of the active constraints
 * using a breadth-first search algorithm.
 */
void FigureTests::findAllMUSes() {
    set<int> cons_set;
    for (int i = 0; i < activeConstraints.size(); i++) {
        cons_set.insert(i);
    }
    
    set<set<int>> mus_found;
    set<set<int>> visited;
    queue<set<int>> queue;

    queue.push(cons_set);
    visited.insert(cons_set);

    while (!queue.empty()) {
        std::set<int> current = queue.front();
        queue.pop();
        // Check if the current set is a minimal unsatisfiable subset
        if (!is_unsat(current))  {
            continue;
        }
        std::set<int> mus = minimize(current);
        if (!mus_found.count(mus)) {
            mus_found.insert(mus);
            // Remove one constraint at a time and add the new set to the queue to explore further
            for (int c : mus) {
                set<int> next = current;
                next.erase(c);
                if (!visited.count(next)) {
                    visited.insert(next);
                    queue.push(next);
                }
            }
        }
    }
    // Print the MUSes found
    for (set<int> mus : mus_found) {
        cout << "\t MUS found: ";
        for (int cons : mus) {
            cout << get_constraint_name(cons) << " ";
        }
        cout << endl;
    }
}

/* ============================================================================
                            FIGURES
============================================================================ */

void FigureTests::test_2v_1sp_fig22() {
    cout << "Start test_2v_1sp_fig22" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,64,67,65,64,72,69,71,71,69,68,69};
    v_type = {1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_1sp_fig23() {
    cout << "Start test_2v_1sp_fig23" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,69,67,65,64,64,62,60,67,69,68,69};
    v_type = {1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_2sp_fig38() {
    cout << "Start test_2v_2sp_fig38" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {-1,65,64,62,60,58,57,55,53,57,60,58,57,69,67,64,65,67,69,65,62,64,65};
    v_type = {0};
    borrowMode = 1;
    findAllMUSes(); 
}

void FigureTests::test_2v_2sp_fig39() {
    cout << "Start test_2v_2sp_fig39" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {-1,53,52,48,53,52,50,48,46,58,55,60,57,53,52,48,53,41,45,50,48,52,53};
    v_type = {-2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_2sp_fig40() {
    cout << "Start test_2v_2sp_fig40" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55}; 
    cp =           {-1, 67,64,65,67,69,71,69,67,72,71,72,74,72,71,69,67,65,64,72,71,69,67,62,64,66,67};
    v_type = {2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_2sp_fig41() {
    cout << "Start test_2v_2sp_fig41" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55}; 
    cp =           {-1,67,64,65,67,65,64,62,60,64,60,72,71,69,67,71,72,71,69,67,66,62,67,59,62,66,67};  
    v_type = {2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_2sp_fig42() {
    cout << "Start test_2v_2sp_fig42" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,60,64,65,64,62,60,59,57}; 
    cp =           {-1,69,64,65,67,62,64,76,72,71,69,65,67,71,74,69,72,64,66,68,69};
    v_type = {2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_2sp_fig43() {
    cout << "Start test_2v_2sp_fig43" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {-1,45,57,52,55,52,50,53,57,59,60,48,50,45,48,52,53,55,57,45,52,56,57};
    v_type = {-1};   
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_2sp_fig44() {
    cout << "Start test_2v_2sp_fig44" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {60,64,65,67,64,69,67,64,65,64,62,60}; 
    cp =           {-1,67,72,71,69,74,71,69,67,71,72,74,76,74,72,71,69,71,72,67,69,71,72};
    v_type = {1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_2sp_fig45() {
    cout << "Start test_2v_2sp_fig45" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {60,64,65,67,64,69,67,64,65,64,62,60}; 
    cp =           {-1,60,72,71,69,74,71,67,72,71,69,72,76,74,72,69,74,69,72,60,67,71,72};
    v_type = {1};   
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_3sp_fig55() {
    cout << "Start test_2v_3sp_fig55" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {62,64,65,67,69,71,72,74,76,74,71,72,74,72,70,69,70,72,74,76,77,65,69,71,72,69,70,72,70,69,67,71,69,62,64,65,67,69,71,73,74};
    v_type = {2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_3sp_fig56() {
    cout << "Start test_2v_3sp_fig56" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {62,64,65,67,69,62,69,71,72,71,67,69,71,69,67,65,64,76,71,72,74,69,62,64,65,67,69,71,72,74,76,72,74,69,62,74,73,69,71,72,74};
    v_type = {1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_3sp_fig57() {
    cout << "Start test_2v_3sp_fig57" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,67,69,71,72,71,69,67,65,67,69,71,72,64,65,67,69,72,76,74,72,71,69,72,71,74,71,69,67,71,72,71,69,71,72,74,76};
    v_type = {2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_3sp_fig58() {
    cout << "Start test_2v_3sp_fig58" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {64,65,67,64,69,67,65,64,62,64,65,67,69,64,69,67,65,64,62,64,65,67,69,71,72,74,76,74,72,60,72,71,69,74,69,74,76};
    v_type = {1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_3sp_fig59() {
    cout << "Start test_2v_3sp_fig59" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {65,64,62,60,59,62,67,65,64,62,60,58,57,60,62,64,65,62,64,65,67,64,65,67,69,67,65,69,67,65,64,62,60,64,60,64,65,64,62,60,58,60,62,64,65};
    v_type = {-1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_3sp_fig60() {
    cout << "Start test_2v_3sp_fig60" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {62,74,72,71,72,69,71,72,74,72,71,69,71,69,66,65,66,74,78,77,76,69,81,78,77,76,74,71,72,69,71,72,74,72,71,69,71,72,74,71,72,69,71,72,74};
    v_type = {1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_4sp_fig74() {
    cout << "Start test_2v_4sp_fig74" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {62,62,74,74,72,72,71,71,76,76,74,74,77,77,76,76,74,74,73,74};
    v_type = {2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_4sp_fig75() {
    cout << "Start test_2v_4sp_fig75" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {76,76,72,72,71,72,64,64,65,65,72,72,71,71,76,76,74,76};
    v_type = {2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_4sp_fig76() {
    cout << "Start test_2v_4sp_fig76" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {64,64,69,69,67,67,65,65,62,62,74,74,72,72,76,76,74,76};
    v_type = {0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_4sp_fig77() {
    cout << "Start test_2v_4sp_fig77" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {65,65,64,64,60,60,65,65,69,69,67,67,65,65,64,64,69,69,65,65,64,65};
    v_type = {0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_4sp_fig78() {
    cout << "Start test_2v_4sp_fig78" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {53,53,52,52,50,50,46,46,43,43,55,55,53,53,52,52,50,50,53,53,52,53};
    v_type = {-3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_2v_5sp_fig86_1() {
    cout << "Start test_2v_5sp_fig86_1" << endl;
    spList = {FIFTH_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {64,62,60,58,57,55,53,52,50,48,46,45};
    v_type = {0};
    borrowMode = 1;
    std::vector<CounterpointProblem*> solution = get_all_solutions();
    for (auto& s : solution) {
        cout << "Solution found" << endl;
        cout << s->getSolutionArray() << endl;
        cout << "Solution added" << endl;
    }

}

void FigureTests::test_3v_1sp_fig108(){
    cout << "Start test_3v_1sp_fig108" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {67,64,65,69,72,72,76,72,69,68,
                    64,69,62,65,65,65,72,72,74,76};
    v_type = {1, 0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_1sp_fig109(){
    cout << "Start test_3v_1sp_fig109" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {52,48,50,48,45,57,55,52,53,52};
    cp =           {67,64,65,64,64,64,67,67,62,64,
                    71,72,69,69,72,72,71,71,69,71};
    v_type = {2, 3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_1sp_fig110(){
    cout << "Start test_3v_1sp_fig110" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {60,60,60,62,58,59,57,69,65,62,64,65,
                    53,52,53,50,55,55,53,45,50,50,48,41};
    v_type = {-1, -3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_1sp_fig111(){
    cout << "Start test_3v_1sp_fig111" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {69,72,72,74,74,70,69,67,65,77,76,77,
                    77,76,77,74,70,67,65,64,65,62,60,65};
    v_type = {1, 0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_1sp_fig112(){
    cout << "Start test_3v_1sp_fig112" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {69,70,72,69,70,72,69,67,72,69,70,69,
                    65,62,60,60,65,67,60,64,64,65,64,65};
    v_type = {1, 0};
    borrowMode = 1;
    findAllMUSes();
}   

void FigureTests::test_3v_1sp_fig113(){
    cout << "Start test_3v_1sp_fig113" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55};
    cp =           {71,67,67,71,76,72,71,67,67,69,66,67,66,67,
                    55,52,52,52,48,48,55,52,48,45,47,43,50,43};
    v_type = {2, -1};
    borrowMode = 1;
    findAllMUSes();
}   

void FigureTests::test_3v_1sp_fig114(){
    cout << "Start test_3v_1sp_fig114" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55};
    cp =           {71,67,67,71,67,69,71,71,72,64,66,67,66,67,
                    55,52,52,52,52,48,55,52,48,48,47,43,50,43};
    v_type = {2, -1};
    borrowMode = 1;
    findAllMUSes();
}  

void FigureTests::test_3v_1sp_fig115(){
    cout << "Start test_3v_1sp_fig115" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {69,72,71,74,72,76,77,76,74,72,71,69};
    cp =           {60,64,67,65,64,67,69,67,65,69,68,69,
                    69,69,76,74,69,67,65,72,74,69,76,69};
    v_type = {-1, 0};
    borrowMode = 1;
    findAllMUSes();
}         

void FigureTests::test_3v_1sp_fig116(){
    cout << "Start test_3v_1sp_fig116" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57};
    cp =           {64,64,67,65,64,72,69,72,71,69,68,69,
                    45,45,52,50,57,57,50,48,55,57,52,45};
    v_type = {1, -1};
    borrowMode = 1;
    findAllMUSes();
}     

void FigureTests::test_3v_1sp_fig117(){
    cout << "Start test_3v_1sp_fig117" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {45,48,47,50,48,52,53,52,50,48,47,45};
    cp =           {60,64,62,65,64,60,60,60,62,57,56,57,
                    69,69,71,71,72,67,69,67,65,64,62,64};
    v_type = {2, 3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_1sp_fig118(){
    cout << "Start test test_3v_1sp_fig118" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {60, 64, 65, 67, 64, 69, 67, 64, 65, 64, 62, 60};
    cp =           {67, 72, 69, 67, 72, 72, 76, 72, 71, 72, 71, 72,
                    48, 48, 50, 52, 48, 53, 52, 57, 50, 48, 55, 48};
    v_type = {1, -1};
    borrowMode = 1;
   findAllMUSes();
}

void FigureTests::test_3v_1sp_fig119(){
    cout << "Start test 3v_1sp_fig119" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {60, 64, 65, 67, 64, 69, 67, 64, 65, 64, 62, 60};
    cp =           {64, 67, 69, 71, 72, 72, 76, 72, 69, 72, 71, 72,
                    72, 72, 69, 67, 69, 65, 72, 72, 74, 72, 67, 72};
    v_type = {1, 1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_2sp_fig125(){
    cout << "Start test_3v_2sp_fig125" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus = {52,   48,   50,   48,   45,   57,   55,   52,   53,   52};
    cp =           {64,   64,   65,   64,   65,   64,   67,   67,   69,   68,
                    -1,71,72,71,69,65,67,64,69,71,72,69,71,74,76,74,72,69,71};
    v_type = {2 ,3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_2sp_fig126(){
    cout << "Start test_3v_2sp_fig126" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {64,69,65,69,65,77,76,72,69,68,
                    -1,52,53,52,50,52,53,48,50,52,53,57,60,55,57,52,52,50,52};
    v_type = {1, -2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_2sp_fig127(){
    cout << "Start test_3v_2sp_fig127" << endl;
    spList = {SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {-1,65,64,62,60,57,62,57,58,57,55,60,57,69,67,64,60,57,62,65,65,64,65,
                    53,48,53,50,46,48,50,52,53,50,48,41};
    v_type = {-1, -3};
    borrowMode = 1;
    findAllMUSes();
}



void FigureTests::test_3v_2sp_fig128(){
    cout << "Start test_3v_2sp_fig128" << endl;
    spList = {SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,   67,   69,   65,   62,   64,   65,   72,   69,   65,   67,   65};
    cp =           {-1,77,76,74,72,71,69,72,70,69,67,72,69,81,79,76,72,69,74,77,77,76,77,
                    65,   60,   65,   65,   67,   72,   74,   76,   77,   74,   72,   65};
    v_type = {1 ,0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_2sp_fig129(){
    cout << "Start test_3v_2sp_fig129" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus =  {65,   67,   69,   65,   62,   64,   65,   72,   69,   65,   67,   65};
    cp =            {60,   60,   60,   62,   65,   67,   69,   67,   60,   62,   64,   65,
                     -1,53,52,48,53,52,50,48,46,45,43,48,41,53,52,48,53,52,50,46,43,48,41};
    v_type = {-1, -3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_3sp_fig130(){
    cout << "Start test_3v_3sp_fig130" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62};
    cp =           {69,62,65,67,69,65,69,71,72,71,67,69,71,72,74,71,76,74,71,73,74,77,76,74,72,69,72,74,76,74,72,71,69,62,69,71,73,69,71,72,74,
                    50,50,48,55,52,50,53,48,50,57,50};
    v_type = {1, -2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_3sp_fig131(){
    cout << "Start test_3v_3sp_fig131" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus = {50,53,52,50,55,53,57,55,53,52,50};
    cp =           {62,64,65,67,69,57,60,62,64,65,67,64,65,69,65,64,62,58,62,64,65,69,67,65,64,60,64,65,67,64,65,67,69,57,60,62,64,62,59,61,62,
                    69,69,73,74,70,69,72,70,69,67,65};
    v_type = {2, 3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_3sp_fig132(){ 
    cout << "Start test_3v_3sp_fig132" << endl;
    spList = {FIRST_SPECIES, THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62};
    cp =           {69,69,72,71,71,74,72,76,74,73,74,
                    -1,62,65,64,62,64,65,67,69,67,64,65,67,62,67,65,64,67,65,64,62,74,62,64,65,67,69,71,72,76,74,72,74,62,65,67,69,67,69,57,62};
    v_type = {1, 1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_3sp_fig133(){
    cout << "Start test_3v_3sp_fig133" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62};
    cp =            {65,62,65,67,69,65,69,71,72,64,67,69,71,74,71,69,67,69,71,73,74,76,77,74,72,69,72,74,76,74,72,71,69,74,69,71,73,69,71,72,74,
                     50,50,48,55,52,50,53,48,50,57,50};
    v_type = {1, -2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_4sp_fig146() {
    cout << "Start test_3v_4sp_fig146" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {76,76,72,72,71,71,69,69,72,72,74,74,72,72,69,69,71,68,
                    64,69,67,69,65,65,64,60,62,64};
    v_type = {2,0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_4sp_fig147() {
    cout << "Start test_3v_4sp_fig147" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {76,76,72,72,71,71,69,69,74,74,72,72,71,71,69,69,71,68,
                    52,57,55,57,53,53,52,48,50,52};
    v_type = {2,-2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_4sp_fig148() {
    cout << "Start test_3v_4sp_fig148" << endl;
    spList = {FIRST_SPECIES, FOURTH_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {67,67,67,64,72,74,76,72,69,68,
                    52,52,48,48,47,47,45,45,53,53,50,50,48,48,52,52,50,52};
    v_type = {2,-2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_4sp_fig149() {
    cout << "Start test_3v_4sp_fig149" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {65,65,64,64,62,62,65,65,67,67,69,69,65,65,64,64,62,62,65,65,64,65,
                    53,48,53,50,46,48,50,57,53,50,48,41};
    v_type = {0,-2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_4sp_fig150(){
    cout << "Start test_3v_4sp_fig150" << endl;
    spList = {FIRST_SPECIES, FOURTH_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {69,71,72,69,65,67,69,67,72,69,70,69,
                    65,65,64,64,60,60,57,57,62,62,60,60,65,65,64,64,60,60,65,65,64,65};
    v_type = {1, 0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_3v_4sp_fig151(){
    cout << "Start test_3v_4sp_fig151" << endl;
    spList = {FIRST_SPECIES, FOURTH_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {53,60,65,62,58,55,62,64,65,62,58,57,
                    53,53,52,52,50,50,46,46,43,-1,48,48,46,46,45,45,50,50,53,53,52,53};
    v_type = {-2, -2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_1sp_fig166() {
    cout << "Start test_4v_1sp_fig166" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {59,57,57,57,60,60,64,60,62,59,
                    68,69,65,64,64,65,72,72,69,68,
                    52,53,50,45,45,41,40,45,50,52};
    v_type = {-1,1,-3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_1sp_fig167() {
    cout << "Start test_4v_1sp_fig167" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,59,67,64,65,64}; 
    cp =           {68,69,65,64,64,65,72,72,69,68,
                    71,69,69,69,72,72,76,72,74,71,
                    52,53,50,45,45,41,40,45,50,52};
    v_type = {1,1,-3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_1sp_fig168() {
    cout << "Start test_4v_1sp_fig168" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,59,67,64,65,64}; 
    cp =           {71,69,69,72,72,72,76,76,74,71,
                    68,69,65,67,69,65,72,72,69,68,
                    64,65,62,64,65,65,64,69,62,64};
    v_type = {1,1,0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_1sp_fig169() {
    cout << "Start test_4v_1sp_fig169" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {69,67,65,69,70,71,69,69,65,65,64,65,
                    60,60,60,62,65,67,65,64,62,57,60,57,
                    53,52,53,50,46,43,50,45,50,50,48,41};
    v_type = {0,-1,-3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_1sp_fig170() {
    cout << "Start test_4v_1sp_fig170" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {60,60,60,60,58,59,57,69,65,65,64,65,
                    69,67,65,69,65,67,65,64,65,69,72,69,
                    53,52,53,41,46,43,50,45,50,50,48,41};
    v_type = {-1,0,-3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_1sp_fig171() {
    cout << "Start test_4v_1sp_fig171" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {69,67,65,65,65,67,65,64,65,65,64,65,
                    60,60,60,62,62,58,57,57,53,57,60,57,
                    53,52,53,50,46,43,50,45,50,50,48,41};
    v_type = {0,-1,-3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_1sp_fig172() {
    cout << "Start test_4v_1sp_fig172" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {72,71,72,69,69,72,72,72,72,69,70,65,
                    69,67,64,65,65,67,69,67,65,65,64,65,
                    65,62,60,60,62,60,60,64,60,62,58,60};
    v_type = {1,0,-1};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_2sp_fig173() {
    cout << "Start test_4v_2sp_fig173" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {57,62,55,55,59,62,60,64,62,61,62,
                    -1,65,69,71,72,67,71,69,67,64,69,65,64,65,64,72,69,65,69,69,69,
                    50,50,48,55,52,50,45,48,50,45,50};   
    v_type = {-1,1,-2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_2sp_fig174() {
    cout << "Start test_4v_2sp_fig174" << endl;
    spList = {SECOND_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {-1,65,69,71,72,67,71,74,76,71,74,77,76,72,76,72,69,74,73,73,74,
                    69,65,67,67,67,69,72,72,65,69,69,
                    50,50,48,55,52,50,45,48,50,45,50};   
    v_type = {1,1,-2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_2sp_fig175() {
    cout << "Start test_4v_2sp_fig175" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {74,74,72,74,76,77,77,76,74,73,74,
                    69,69,69,71,71,74,72,72,69,69,69,
                    -1,74,62,65,69,72,67,65,64,67,62,64,65,69,72,60,62,65,69,57,62};   
    v_type = {2,1,0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_2sp_fig176() {
    cout << "Start test_4v_2sp_fig176" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {50,53,52,50,55,53,57,55,53,52,50}; 
    cp =            {65,   69,   72,   74,   70,   69,   69,   71,   74,   73,   74,
                     -1,62,60,62,64,67,65,64,62,64,65,69,64,65,67,62,65,69,67,64,66,
                     69,   69,   67,   69,   70,   72,   72,   74,   69,   76,   69};
    v_type = {3, 2, 3};
    borrowMode = 1;
    findAllMUSes();
}
    
void FigureTests::test_4v_3sp_fig183() {
    cout << "Start test_4v_3sp_fig183" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,64,71,72,69,72,69,67,65,62,64,65,67,64,65,67,69,65,69,71,72,69,72,74,76,71,76,74,72,76,72,71,69,74,69,71,68, 
                    68,69,69,67,72,69,71,72,74,71,
                    52,53,50,52,53,53,52,57,50,52};   
    v_type = {0, 1, -2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_3sp_fig184(){
    cout << "Start test_4v_3sp_fig184" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {76,72,74,72,69,81,79,76,77,76}; 
    cp =           {68,71,64,67,69,72,69,67,65,62,64,65,67,65,64,62,60,72,69,67,65,67,69,71,72,74,76,71,72,69,71,72,69,74,69,71,68,
                    59,57,57,55,57,60,64,60,62,59,
                    64,65,62,64,65,65,64,69,62,64};
    v_type = {-2, -4, -3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_3sp_fig185(){
    cout << "Start test_4v_3sp_fig185" << endl;
    spList = {FIRST_SPECIES, THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,69,69,72,72,72,76,65,65,68, 
                    68,71,64,67,69,72,69,67,65,69,67,65,67,64,76,74,72,71,69,67,65,67,69,71,72,74,76,74,72,69,72,71,69,62,74,72,71,  
                    52,53,50,52,53,53,48,48,50,52};   
    v_type = {1, 0, -2};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_3sp_fig186(){
    cout << "Start test_4v_3sp_fig186" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, THIRD_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,76,77,76,77,77,76,76,69,68,
                    68,69,69,72,72,72,72,72,74,71,
                    76,74,72,71,69,67,65,64,62,64,65,62,69,72,69,67,65,67,69,67,65,67,69,71,72,64,65,67,69,67,65,64,62,65,64,62,64};
    v_type = {1, 1, 0};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::test_4v_4sp_fig196(){
    cout << "Start test_4v_4sp_fig196" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =            {57,57,62,62,60,60,59,59,62,62,57,57,65,65,64,64,62,62,61,62,
                     65,69,69,62,74,74,72,72,69,69,69,
                     50,50,45,47,43,50,53,48,50,45,50};
    v_type = {-1, 1, -3};
    borrowMode = 1;
    findAllMUSes();
}

void FigureTests::run_twoVoice_tests() {
    cout << "Running two voice tests..." << endl;
    // Two voice first species figures
    test_2v_1sp_fig22();
    test_2v_1sp_fig23();
    // Two voice second species figures
    test_2v_2sp_fig38();
    test_2v_2sp_fig39();
    test_2v_2sp_fig40();
    test_2v_2sp_fig41();
    test_2v_2sp_fig42();
    test_2v_2sp_fig43();
    test_2v_2sp_fig44();
    test_2v_2sp_fig45();
    // Two voice third species figures
    test_2v_3sp_fig55();
    test_2v_3sp_fig56();
    test_2v_3sp_fig57();
    test_2v_3sp_fig58();
    test_2v_3sp_fig59();
    test_2v_3sp_fig60();
    // Two voice fourth species figures
    test_2v_4sp_fig74();
    test_2v_4sp_fig75();
    test_2v_4sp_fig76();
    test_2v_4sp_fig77();
    test_2v_4sp_fig78();
    // Two voice fifth species figures
    test_2v_5sp_fig86_1();
}

void FigureTests::run_threeVoice_tests() {
    cout << "Running three voice tests..." << endl;
    // Three voice first species figures
    test_3v_1sp_fig108();
    test_3v_1sp_fig109();
    test_3v_1sp_fig110();
    test_3v_1sp_fig111();
    test_3v_1sp_fig112();
    test_3v_1sp_fig113();
    test_3v_1sp_fig114();
    test_3v_1sp_fig115();
    test_3v_1sp_fig116();
    test_3v_1sp_fig117();
    test_3v_1sp_fig118();
    test_3v_1sp_fig119();
    // Three voice second species figures
    test_3v_2sp_fig125();
    test_3v_2sp_fig126();
    test_3v_2sp_fig127();
    test_3v_2sp_fig128();
    test_3v_2sp_fig129();
    // Three voice third species figures
    test_3v_3sp_fig130();
    test_3v_3sp_fig131();
    test_3v_3sp_fig132();
    test_3v_3sp_fig133();
    // Three voice fourth species figures
    test_3v_4sp_fig146();
    test_3v_4sp_fig147();
    test_3v_4sp_fig148();
    test_3v_4sp_fig149();
    test_3v_4sp_fig150();
    test_3v_4sp_fig151();
}

void FigureTests::run_fourVoice_tests() {
    cout << "Running four voice tests..." << endl;
    // Four voice first species figures
    test_4v_1sp_fig166();
    test_4v_1sp_fig167();   
    test_4v_1sp_fig168();
    test_4v_1sp_fig169();
    test_4v_1sp_fig170();
    test_4v_1sp_fig171();
    test_4v_1sp_fig172();
    // Four voice second species figures
    test_4v_2sp_fig173();
    test_4v_2sp_fig174();
    test_4v_2sp_fig175();
    test_4v_2sp_fig176();
    // Four voice third species figures
    test_4v_3sp_fig183();
    test_4v_3sp_fig184();
    test_4v_3sp_fig185();
    test_4v_3sp_fig186();
    // Four voice fourth species figures
    test_4v_4sp_fig196();
}

void FigureTests::run_fourthSpecies_tests() {
    cout << "Running four species tests..." << endl;
    // Two voice fourth species figures
    test_2v_4sp_fig74();
    test_2v_4sp_fig75();
    test_2v_4sp_fig76();
    test_2v_4sp_fig77();
    test_2v_4sp_fig78();
    // Three voice fourth species figures
    test_3v_4sp_fig146();
    test_3v_4sp_fig147();
    test_3v_4sp_fig148();
    test_3v_4sp_fig149();
    test_3v_4sp_fig150();
    test_3v_4sp_fig151();
    // Four voice fourth species figures
    test_4v_4sp_fig196();
}

void FigureTests::run_all_tests() {
    cout << "Running all figure tests..." << endl;
    run_twoVoice_tests();
    run_threeVoice_tests();
    run_fourVoice_tests();
    cout << "All figure tests completed." << endl;
} 

void FigureTests::MUSTest() {
    cout << "Running MUSTest..." << endl;
    run_all_tests();
    
    cout << "MUSTest completed." << endl;
}