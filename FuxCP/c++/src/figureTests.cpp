#include "../headers/figureTests.hpp"
#include <iostream>
#include <queue>

using namespace std;

FigureTests::FigureTests() {
    borrowMode = 1;
    borrowMode = 1;
    melodic_params = {0, 1, 1, 576, 2, 2, 2, 1};
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8, 4, 0, 2, 1, 8, 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    notesSpeciesFor5sp = {};
}

CounterpointProblem* FigureTests::set_configuration() {
    // create a new problem
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    // set the solution array
    for (size_t i = 0; i < cp.size(); i++) { 
        int note = cp[i];
        if (note >  0) {
            rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, note);
        }
    }
    // set the species array for the 5th sepcies counterpoint
    if (spList[0] == FIFTH_SPECIES) {
        auto* part = problem->getCounterpoint_1();
        for (int i = 0; i < notesSpeciesFor5sp.size(); i++) {
            int sp = notesSpeciesFor5sp[i];
            if (sp > -1) {
                rel(problem->getHome(), part->getSpeciesArray()[i], IRT_EQ, sp);
            }
            
        }
    }
    if (spList.size() > 1 && spList[1] == FIFTH_SPECIES) {
        auto* part = problem->getCounterpoint_2();
        for (int i = 0; i < notesSpeciesFor5sp.size(); i++) {
            int sp = notesSpeciesFor5sp[i];
            if (sp > -1) {
                rel(problem->getHome(), part->getSpeciesArray()[i], IRT_EQ, sp);
            }
        }
    }
    if (spList.size() > 2 && spList[2] == FIFTH_SPECIES) {
        auto* part = problem->getCounterpoint_3();
        for (int i = 0; i < notesSpeciesFor5sp.size(); i++) {
            int sp = notesSpeciesFor5sp[i];
            if (sp > -1) {
                rel(problem->getHome(), part->getSpeciesArray()[i], IRT_EQ, sp);
            }
        }
    }
    return problem;
}

std::vector<CounterpointProblem*> FigureTests::get_all_solutions() {
    CounterpointProblem* problem = set_configuration();
    std::vector<CounterpointProblem*> solutions;
    BAB<CounterpointProblem> e(problem);
    while (CounterpointProblem* pb = e.next()) {
        cout << "Solution found" << endl;
        solutions.push_back(pb);
        cout << pb->getSolutionArray() << endl;
        cout << pb->getCounterpoint_1()-> getSpeciesArray() << endl;
        cout << "Solution added" << endl;
    }
    return solutions;
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
    CounterpointProblem* problem = set_configuration();
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
    // Initialization
    set<int> cons_set; // all constraint indices
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

void FigureTests::find_unsat_constraints() {
    // activeConstraints = std::vector<bool>(activeConstraints.size(), false);
    // cout << has_solution(set_configuration()) << endl;
    for (int i = 0; i < consSize; i++) {
        set<int> cons_set; // all constraint indices
        cons_set.insert(i);
        if (is_unsat(cons_set))  {
            cout << "\t" << get_constraint_name(i) << " is unsatisfiable." << endl;
        }
    }
}

void FigureTests::find_unsat_constraints() {
    // activeConstraints = std::vector<bool>(activeConstraints.size(), false);
    // cout << has_solution(set_configuration()) << endl;
    for (int i = 0; i < consSize; i++) {
        set<int> cons_set; // all constraint indices
        cons_set.insert(i);
        if (is_unsat(cons_set))  {
            cout << "\t" << get_constraint_name(i) << " is unsatisfiable." << endl;
        }
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
    CounterpointProblem* problem = set_configuration();
    has_solution(problem);
    cout << problem->getLowest()->getMelodicIntervals() << endl;
    cout << problem->getCounterpoint_1()->getMelodicIntervals() << endl;
    cout << problem->getCounterpoint_1()->getMelodicIntervals().size() << endl;
    cout << cantusFirmus.size() << endl;

    cout << problem->getCounterpoint_1()->getFirstMInterval() << endl;
    cout << problem->getCounterpoint_1()->getMotions() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_1sp_fig23() {
    cout << "Start test_2v_1sp_fig23" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {57,57,55,53,52,52,50,48,55,57,56,57};

    v_type = {-1};
    find_unsat_constraints();
}

void FigureTests::test_2v_2sp_fig38() {
    cout << "Start test_2v_2sp_fig38" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53};
    cp =           {-1,65,64,62,60,58,57,55,53,57,60,58,57,69,67,64,65,67,69,65,62,64,65};
    v_type = {2};
    find_unsat_constraints(); 
}

void FigureTests::test_2v_2sp_fig39() {
    cout << "Start test_2v_2sp_fig39" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cp =           {-1,53,52,48,53,52,50,48,46,58,55,60,57,53,52,48,53,41,45,50,48,52,53};
    v_type = {0};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCounterpoint_1()->getIsNotLowest() << endl;
    // cout << problem->getCounterpoint_1()->getFirstHInterval() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_2sp_fig40() {
    cout << "Start test_2v_2sp_fig40" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55}; 
    cp =           {-1, 67,64,65,67,69,71,69,67,72,71,72,74,72,71,69,67,65,64,72,71,69,67,62,64,66,67};
    v_type = {2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    find_unsat_constraints();
}

void FigureTests::test_2v_2sp_fig41() {
    cout << "Start test_2v_2sp_fig41" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55}; 
    cp =           {-1,55,52,53,55,53,52,50,48,52,48,60,59,57,55,59,60,59,57,55,54,50,55,47,50,54,55};  
    v_type = {0};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCantusFirmus()->getHIntervalSize() << endl;
    // cout << problem->getCantusFirmus()->getHInterval().size() << endl;
    // cout << problem->getCantusFirmus()->getHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getHIntervalSize() << endl;
    // cout << problem->getCounterpoint_1()->getHInterval().size() << endl;
    // cout << problem->getCounterpoint_1()->getHInterval() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_2sp_fig42() {
    cout << "Start test_2v_2sp_fig42" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,60,64,65,64,62,60,59,57}; 
    cp =           {-1,69,64,65,67,62,64,76,72,71,69,65,67,71,74,69,72,64,66,68,69};
    v_type = {2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCounterpoint_1()->getFirstHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getIsNotLowest() << endl;
    // cout << problem->getCounterpoint_1()->getSecondSpeciesMIntervals() << endl;
    // cout << problem->getCounterpoint_1()->getSecondSpeciesRealMotions() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_2sp_fig43() {
    cout << "Start test_2v_2sp_fig43" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {-1,45,57,52,55,52,50,53,57,59,60,48,50,45,48,52,53,55,57,45,52,56,57};
    v_type = {-1};   
    find_unsat_constraints();
}

void FigureTests::test_2v_2sp_fig44() {
    cout << "Start test_2v_2sp_fig44" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {60,64,65,67,64,69,67,64,65,64,62,60}; 
    cp =           {-1,67,72,71,69,74,71,69,67,71,72,74,76,74,72,71,69,71,72,67,69,71,72};
    v_type = {1};
    find_unsat_constraints();
}

void FigureTests::test_2v_2sp_fig45() {
    cout << "Start test_2v_2sp_fig45" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {60,64,65,67,64,69,67,64,65,64,62,60}; 
    cp =           {-1,48,60,59,57,62,59,55,60,59,57,60,64,62,60,57,62,57,60,48,55,59,60};
    v_type = {-1};   
    find_unsat_constraints();
}

void FigureTests::test_2v_3sp_fig55() {
    cout << "Start test_2v_3sp_fig55" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {62,64,65,67,69,71,72,74,76,74,71,72,74,72,70,69,70,72,74,76,77,65,69,71,72,69,70,72,70,69,67,71,69,62,64,65,67,69,71,73,74};
    v_type = {2};
    find_unsat_constraints();
}

void FigureTests::test_2v_3sp_fig56() {
    cout << "Start test_2v_3sp_fig56" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {50,52,53,55,57,50,57,59,60,59,55,57,59,57,55,53,52,64,59,60,62,57,50,52,53,55,57,59,60,62,64,60,62,57,50,62,61,57,59,60,62};
    v_type = {-1};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCounterpoint_1()->getIs5QNArray() << endl;
    // cout << problem->getCounterpoint_1()->getConsonance() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_3sp_fig57() {
    cout << "Start test_2v_3sp_fig57" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,67,69,71,72,71,69,67,65,67,69,71,72,64,65,67,69,72,76,74,72,71,69,72,71,74,71,69,67,71,72,71,69,71,72,74,76};
    v_type = {2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCounterpoint_1()->getIsNotLowest() << endl;
    // cout << problem->getCantusFirmus()->getHInterval() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_3sp_fig58() {
    cout << "Start test_2v_3sp_fig58" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {52,53,55,52,57,55,53,52,50,52,53,55,57,52,57,55,53,52,50,52,53,55,57,59,60,62,64,62,60,48,60,59,57,62,57,62,64};
    v_type = {-1};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCantusFirmus()->getHInterval() << endl;
    // cout << problem->getCantusFirmus()->getFirstHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getFirstHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getFirstSpeciesHIntervals() << endl;
    // cout << problem->getCounterpoint_1()->getHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getIs5QNArray() << endl;
    // cout << problem->getCounterpoint_1()->getConsonance() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_3sp_fig59() {
    cout << "Start test_2v_3sp_fig59" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cp =           {65,64,62,60,59,62,67,65,64,62,60,58,57,60,62,64,65,62,64,65,67,64,65,67,69,67,65,69,67,65,64,62,60,64,60,64,65,64,62,60,58,60,62,64,65};
    v_type = {1};
    find_unsat_constraints();
}

void FigureTests::test_2v_3sp_fig60() {
    cout << "Start test_2v_3sp_fig60" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cp =           {41,53,52,50,52,48,50,52,53,52,50,48,50,48,46,45,46,53,58,57,55,48,60,58,57,55,53,50,52,48,50,52,53,52,50,48,50,52,53,50,52,48,50,52,53};
    v_type = {0};
    find_unsat_constraints();
}

void FigureTests::test_2v_4sp_fig74() {
    cout << "Start test_2v_4sp_fig74" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {50,50,62,62,60,60,59,59,64,64,62,62,65,65,64,64,62,62,61,62};
    v_type = {-1};
    CounterpointProblem* problem = set_configuration();
    has_solution(problem);
    cout << "Lowest melodic intervals" << endl;
    cout << problem->getLowest()->getMelodicIntervals() << endl;
    cout << "Fourth Species melodic intervals" << endl;
    cout << problem->getCounterpoint_1()->getFourthSpeciesMIntervals() << endl;

    // cout << problem->getCounterpoint_1()->getFourthSpeciesMIntervals() << endl;
    // for(int i = 1; i < 11-2; i++){
    //     cout << "i: " << i << endl;
    //     cout << "isConsonance: " << problem->getCounterpoint_1()->getConsonance()[i*4] << endl;
    //     cout << "fourthSpeciesMelodicIntervals: " << problem->getCounterpoint_1()->getFourthSpeciesMIntervals()[i*2-1] << endl;
    // }
    // cout << problem->getCounterpoint_1()->getIsNotLowest() << endl;
    // cout << problem->getCounterpoint_1()->getFirstSpeciesMotions() << endl;
    // cout << problem->getCounterpoint_1()->getFourthSpeciesMIntervals() << endl;
    // cout << problem->getLowest()->getMelodicIntervals() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_4sp_fig75() {
    cout << "Start test_2v_4sp_fig75" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {76,76,72,72,71,72,64,64,65,65,72,72,71,71,76,76,74,76};
    v_type = {2};
    find_unsat_constraints();
}

void FigureTests::test_2v_4sp_fig76() {
    cout << "Start test_2v_4sp_fig76" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {52,52,57,57,55,55,53,53,50,50,62,62,60,60,64,64,62,64};
    v_type = {-2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getLowest()->getNotes() << endl;
    // cout << problem->getCounterpoint_1()->getHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getIsNotLowest() << endl;
    // cout << problem->getCounterpoint_1()->getConsonance() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_4sp_fig77() {
    cout << "Start test_2v_4sp_fig77" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53};
                   
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53};
                   
    cp =           {65,65,64,64,60,60,65,65,69,69,67,67,65,65,64,64,69,69,65,65,64,65};
    // {65,65,64,64,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    v_type = {2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getLowest()->getNotes() << endl;
    // cout << problem->getCounterpoint_1()->getHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getIsNotLowest() << endl;
    // cout << problem->getCounterpoint_1()->getFourthSpeciesMIntervals() << endl;
    // cout << problem->getCounterpoint_1()->getConsonance() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_4sp_fig78() {
    cout << "Start test_2v_4sp_fig78" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cp =           {53,53,52,52,50,50,46,46,43,43,55,55,53,53,52,52,50,50,53,53,52,53};
    v_type = {-1};
    find_unsat_constraints();
}

void FigureTests::test_2v_5sp_fig82() {
    cout << "Start test_2v_5sp_fig82" << endl;
    spList = {FIFTH_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {-1,-1,69,-1,
                    69,-1, 64,65,
                    67,65,64,67,
                    65,62,74,-1,
                    74,-1,70,67,
                    69,71,72,-1,
                    72,-1,77,-1,
                    77,-1,76,-1,
                    76,-1,74,-1,
                    74,-1,73,-1,
                    74};
    notesSpeciesFor5sp = {
                    -1, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES,-1, THIRD_SPECIES, THIRD_SPECIES,
                    THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES,
                    THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                    THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES
                };
    v_type = {2};
    find_unsat_constraints();
}

void FigureTests::test_2v_5sp_fig83() {
    cout << "Start test_2v_5sp_fig83" << endl;
    spList = {FIFTH_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {-1,-1,62,-1,
                    62,-1,57,59,
                    60,55,60,-1,
                    60,-1,59,57,
                    55,57,59,60,
                    62,57,62,-1,
                    62,-1,65,-1,
                    65,-1,64,-1,
                    64,-1,62,-1,
                    62,-1,61,-1,
                    62};
    v_type = {0};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getLowest()->getNotes() << endl;
    find_unsat_constraints();
}

void FigureTests::test_2v_5sp_fig87_1() {
    cout << "Start test_2v_5sp_fig87_1" << endl;
    spList = {FIFTH_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {-1,-1,69,-1,
                    69,-1,67,69,
                    71,67,71,-1,
                    71,-1,69,71,
                    72,67,72,-1,
                    72,-1,71,72,
                    74,69,74,-1,
                    74,-1,72,-1,
                    72,-1,71,-1,
                    71,-1,69,-1,
                    69,-1,68,-1,
                    69};
    notesSpeciesFor5sp = {
                    -1, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                    THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                    THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                    THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                    FOURTH_SPECIES
                };
    v_type = {2};
    find_unsat_constraints();
}

void FigureTests::test_3v_1sp_fig108(){
    cout << "Start test_3v_1sp_fig108" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {67,64,65,69,72,72,76,72,69,68,
                    52,57,50,53,53,53,60,60,62,64};
    v_type = {1, -2};
    find_unsat_constraints();
}

void FigureTests::test_3v_1sp_fig109(){
    cout << "Start test_3v_1sp_fig109" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {52,48,50,48,45,57,55,52,53,52};
    cp =           {67,64,65,64,64,64,67,67,62,64,
                    59,60,57,57,60,60,59,59,57,59};
    v_type = {2, 1};
    find_unsat_constraints();
}

void FigureTests::test_3v_1sp_fig110(){
    cout << "Start test_3v_1sp_fig110" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {60,60,60,62,58,59,57,69,65,62,64,65,
                    53,52,53,50,55,55,53,45,50,50,48,41};
    v_type = {-1, -3};
    find_unsat_constraints();
}

void FigureTests::test_3v_1sp_fig111(){
    cout << "Start test_3v_1sp_fig111" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {69,72,72,74,74,70,69,67,65,77,76,77,
                    65,64,65,62,58,55,53,52,53,50,48,53};
    v_type = {1, -2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_1sp_fig112(){
    cout << "Start test_3v_1sp_fig112" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53};
    cp =           {69,70,72,69,70,72,69,67,72,69,70,69,
                    65,62,60,60,65,67,60,64,64,65,64,65};
    v_type = {3, 2};
    
    find_unsat_constraints();
}   

void FigureTests::test_3v_1sp_fig113(){
    cout << "Start test_3v_1sp_fig113" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55};
    cp =           {59,55,55,59,64,60,59,55,55,57,54,55,54,55,
    cp =           {59,55,55,59,64,60,59,55,55,57,54,55,54,55,
                    55,52,52,52,48,48,55,52,48,45,47,43,50,43};
    v_type = {0, -1};
    
    find_unsat_constraints();
}   

void FigureTests::test_3v_1sp_fig114(){
    cout << "Start test_3v_1sp_fig114" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55};
    cp =           {71,67,67,71,67,69,71,71,72,64,66,67,66,67,
                    55,52,52,52,52,48,55,52,48,48,47,43,50,43};
    v_type = {2, -1};
    
    find_unsat_constraints();
}  

void FigureTests::test_3v_1sp_fig115(){
    cout << "Start test_3v_1sp_fig115" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {69,72,71,74,72,76,77,76,74,72,71,69};
    cp =           {60,64,67,65,64,67,69,67,65,69,68,69,
                    57, 57, 64, 62, 57, 55, 53, 60, 62, 57, 64, 57};
    v_type = {-1, -2};
    
    find_unsat_constraints();
}         

void FigureTests::test_3v_1sp_fig116(){
    cout << "Start test_3v_1sp_fig116" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57};
    cp =           {64,64,67,65,64,72,69,72,71,69,68,69,
                    45,45,52,50,57,57,50,48,55,57,52,45};
    v_type = {1, -1};
    
    find_unsat_constraints();
}     

void FigureTests::test_3v_1sp_fig117(){
    cout << "Start test_3v_1sp_fig117" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {45,48,47,50,48,52,53,52,50,48,47,45};
    cp =           {60,64,62,65,64,60,60,60,62,57,56,57,
                    57,57,59,59,60,55,57,55,53,52,50,52};
    v_type = {2, 1};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_1sp_fig118(){
    cout << "Start test test_3v_1sp_fig118" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {60, 64, 65, 67, 64, 69, 67, 64, 65, 64, 62, 60};
    cp =           {55, 60, 57, 55, 60, 60, 64, 60, 59, 60, 59, 60,
    cp =           {55, 60, 57, 55, 60, 60, 64, 60, 59, 60, 59, 60,
                    48, 48, 50, 52, 48, 53, 52, 57, 50, 48, 55, 48};
    v_type = {-1, -1};
    
   find_unsat_constraints();
}

void FigureTests::test_3v_1sp_fig119(){
    cout << "Start test 3v_1sp_fig119" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {60, 64, 65, 67, 64, 69, 67, 64, 65, 64, 62, 60};
    cp =           {64, 67, 69, 71, 72, 72, 76, 72, 69, 72, 71, 72,
                    60, 60, 57, 55, 57, 53, 60, 60, 62, 60, 55, 60};
    v_type = {1, -1};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_2sp_fig125(){
    cout << "Start test_3v_2sp_fig125" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus = {52,   48,   50,   48,   45,   57,   55,   52,   53,   52};
    cp =           {64,   64,   65,   64,   65,   64,   67,   67,   69,   68,
                    -1,59,60,59,57,53,55,52,57,59,60,57,59,62,64,62,60,57,59};
    v_type = {2 ,1};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_2sp_fig126(){
    cout << "Start test_3v_2sp_fig126" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {52,57,53,57,53,65,64,60,57,56,
    cp =           {52,57,53,57,53,65,64,60,57,56,
                    -1,52,53,52,50,52,53,48,50,52,53,57,60,55,57,52,52,50,52};
    v_type = {-1, -2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    
    find_unsat_constraints();
}

void FigureTests::test_3v_2sp_fig127(){
    cout << "Start test_3v_2sp_fig127" << endl;
    spList = {SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {-1,65,64,62,60,57,62,57,58,57,55,60,57,69,67,64,60,57,62,65,65,64,65,
                    53,48,53,50,46,48,50,52,53,50,48,41};
    v_type = {-1, -3};
    
    find_unsat_constraints();
}



void FigureTests::test_3v_2sp_fig128(){
    cout << "Start test_3v_2sp_fig128" << endl;
    spList = {SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,   67,   69,   65,   62,   64,   65,   72,   69,   65,   67,   65};
    cp =           {-1,77,76,74,72,71,69,72,70,69,67,72,69,81,79,76,72,69,74,77,77,76,77,
                    53,   48,   53,   53,   55,   60,   62,   64,   65,   62,   60,   53};
    v_type = {1 ,-2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_2sp_fig129(){
    cout << "Start test_3v_2sp_fig129" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus =  {53, 55, 57, 53, 50, 52, 53, 60, 57, 53, 55, 53};
    cantusFirmus =  {53, 55, 57, 53, 50, 52, 53, 60, 57, 53, 55, 53};
    cp =            {60,   60,   60,   62,   65,   67,   69,   67,   60,   62,   64,   65,
                     -1,53,52,48,53,52,50,48,46,45,43,48,41,53,52,48,53,52,50,46,43,48,41};
    v_type = {1, -1};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_3sp_fig130(){
    cout << "Start test_3v_3sp_fig130" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62};
    cp =           {69,62,65,67,69,65,69,71,72,71,67,69,71,72,74,71,76,74,71,73,74,77,76,74,72,69,72,74,76,74,72,71,69,62,69,71,73,69,71,72,74,
                    50,50,48,55,52,50,53,48,50,57,50};
    v_type = {1, -2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_3sp_fig131(){
    cout << "Start test_3v_3sp_fig131" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus = {50,53,52,50,55,53,57,55,53,52,50};
    cp =           {62,64,65,67,69,57,60,62,64,65,67,64,65,69,65,64,62,58,62,64,65,69,67,65,64,60,64,65,67,64,65,67,69,57,60,62,64,62,59,61,62,
                    57,57,61,62,58,57,60,58,57,55,53};
    v_type = {2, 1};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_3sp_fig132(){ 
    cout << "Start test_3v_3sp_fig132" << endl;
    spList = {FIRST_SPECIES, THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62};
    cp =           {69,69,72,71,71,74,72,76,74,73,74,
                    -1,50,53,52,50,52,53,55,57,55,52,53,55,50,55,53,52,55,53,52,50,62,50,52,53,55,57,59,60,64,62,60,62,50,53,55,57,55,57,45,50};
    v_type = {1, -1};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_3sp_fig133(){
    cout << "Start test_3v_3sp_fig133" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62};
    cp =            {53,50,53,55,57,53,57,59,60,52,55,57,59,62,59,57,55,57,59,61,62,64,65,62,60,57,60,62,64,62,60,59,57,62,57,59,61,57,59,60,62,
    cp =            {53,50,53,55,57,53,57,59,60,52,55,57,59,62,59,57,55,57,59,61,62,64,65,62,60,57,60,62,64,62,60,59,57,62,57,59,61,57,59,60,62,
                     50,50,48,55,52,50,53,48,50,57,50};
    v_type = {-1, -2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_4sp_fig146() {
    cout << "Start test_3v_4sp_fig146" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {76,76,72,72,71,71,69,69,72,72,74,74,72,72,69,69,71,68,
                    52,57,55,57,53,53,52,48,50,52};
    v_type = {2,-2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_4sp_fig147() {
    cout << "Start test_3v_4sp_fig147" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {64,64,60,60,59,59,57,57,62,62,60,60,59,59,57,57,59,56,
    cp =           {64,64,60,60,59,59,57,57,62,62,60,60,59,59,57,57,59,56,
                    52,57,55,57,53,53,52,48,50,52};
    v_type = {0,-2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCantusFirmus()->getNotes() << endl;
    // cout << problem->getCounterpoint_1()->getNotes() << endl;
    find_unsat_constraints();
}

void FigureTests::test_3v_4sp_fig148() {
    cout << "Start test_3v_4sp_fig148" << endl;
    spList = {FIRST_SPECIES, FOURTH_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {67,67,67,64,72,74,76,72,69,68,
                    52,52,48,48,47,47,45,45,53,53,50,50,48,48,52,52,50,52};
    v_type = {2,-2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_4sp_fig149() {
    cout << "Start test_3v_4sp_fig149" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {65,65,64,64,62,62,65,65,67,67,69,69,65,65,64,64,62,62,65,65,64,65,
                    53,48,53,50,46,48,50,57,53,50,48,41};
    v_type = {0,-2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_4sp_fig150(){
    cout << "Start test_3v_4sp_fig150" << endl;
    spList = {FIRST_SPECIES, FOURTH_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53};
    cp =           {69,71,72,69,65,67,69,67,72,69,70,69,
                    65,65,64,64,60,60,57,57,62,62,60,60,65,65,64,64,60,60,65,65,64,65};
    v_type = {3, 2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_4sp_fig151(){
    cout << "Start test_3v_4sp_fig151" << endl;
    spList = {FIRST_SPECIES, FOURTH_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {53,60,65,62,58,55,62,64,65,62,58,57,
                    53,53,52,52,50,50,46,46,43,-1,48,48,46,46,45,45,50,50,53,53,52,53};
    v_type = {-2, -2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getLowest()->getNotes() << endl;
    // cout << problem->getCantusFirmus()->getConsonance() << endl;
    // cout << problem->getCounterpoint_1()->getConsonance() << endl;
    find_unsat_constraints();
}

void FigureTests::test_3v_5sp_fig154() {
    cout << "Start test_3v_5sp_fig154" << endl;
    spList = {FIFTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {-1,-1,69,-1,
                    69,-1,69,71,
                    72,67,72,-1,
                    72,-1,71,-1,
                    71,-1,71,73,
                    74,77,76,74,
                    72,65,77,-1,
                    77,-1,76,-1,
                    76,-1,74,-1,
                    74,-1,73,-1,
                    74,

                    50,62,60,55,52,50,53,60,62,57,50};
                    50,62,60,55,52,50,53,60,62,57,50};
    notesSpeciesFor5sp = {
                        -1, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,-1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,
    };
    v_type = {2, -2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCounterpoint_1()->getNotes() << endl;
    // cout << problem->getCounterpoint_1()->getSpeciesArray() << endl;
    // for(int i = 1; i < 11-1; i++){
    //     cout << "Species " << i << ": " << problem->getCounterpoint_1()->getSpeciesArray()[i*4] << endl;
    //     cout << " Notes: " << problem->getCounterpoint_1()->getNotes()[i*4] << endl;
    //     cout << " Notes: " << problem->getCounterpoint_1()->getNotes()[(i*4)+2] << endl;
    // }
    
    find_unsat_constraints();
}

void FigureTests::test_3v_5sp_fig155() {
    cout << "Start_test_3v_5sp_fig155" << endl;
    spList = {FIFTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62};
    cp =           {-1,-1,57,-1,
                    57,-1,57,59,
                    60,55,60,-1,
                    60,-1,59,-1,
                    59,-1,59,61,
                    62,57,62,-1,
                    62,-1,65,-1,
                    65,-1,64,-1,
                    64,-1,62,-1,
                    62,-1,61,-1,
                    62,
    cp =           {-1,-1,57,-1,
                    57,-1,57,59,
                    60,55,60,-1,
                    60,-1,59,-1,
                    59,-1,59,61,
                    62,57,62,-1,
                    62,-1,65,-1,
                    65,-1,64,-1,
                    64,-1,62,-1,
                    62,-1,61,-1,
                    62,

                    50,50,48,55,52,50,53,48,50,57,50};
    notesSpeciesFor5sp = {
                        -1, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,-1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,
    };
    v_type = {0, -2};
    
    find_unsat_constraints();
}

void FigureTests::test_3v_5sp_fig156() {
    cout << "Start test_3v_5sp_fig156" << endl;
    spList = {FIRST_SPECIES, FIFTH_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {69,69,72,71,71,74,72,72,69,67,66,
                    
                    -1,-1,50,-1,
                    50,-1,53,55,
                    57,52,57,-1,
                    57,-1,55,53,
                    52,55,53,52,
                    50,52,53,-1,
                    53,-1,53,-1,
                    53,-1,52,-1,
                    52,-1,50,-1,
                    50,-1,49,-1,
                    50};
    notesSpeciesFor5sp = {
                        -1, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,-1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES
    };
    v_type = {2,-2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getLowest()->getNotes() << endl;
    // cout << problem->getCounterpoint_1()->getConsonance() << endl;
    find_unsat_constraints();
}

void FigureTests::test_3v_5sp_fig157() {
    cout << "Start test_3v_5sp_fig157" << endl;
    spList = {FIFTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {-1,-1,76,-1,
                    76,-1,72,71,
                    -1,-1,67,65,
                    -1,-1,69,-1,
                    69,-1,72,-1,
                    72,-1,77,-1,
                    77,-1,76,74,
                    72,67,72,-1,
                    72,-1,71,69,
                    68,

                    52,57,53,57,53,53,60,60,62,64};
                    52,57,53,57,53,53,60,60,62,64};
    notesSpeciesFor5sp = {
                        -1, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,-1, THIRD_SPECIES, THIRD_SPECIES,
                        -1,  -1, THIRD_SPECIES, THIRD_SPECIES,
                        -1, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                        FOURTH_SPECIES
    };
    v_type = {2,0};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_1sp_fig166() {
    cout << "Start test_4v_1sp_fig166" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {59,57,57,57,60,60,64,60,62,59,
                    56,57,53,52,52,53,60,60,57,56,
                    56,57,53,52,52,53,60,60,57,56,
                    52,53,50,45,45,41,40,45,50,52};
    v_type = {-1,-1,-3};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_1sp_fig167() {
    cout << "Start test_4v_1sp_fig167" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {68,69,65,64,64,65,72,72,69,68,
                    59,57,57,57,60,60,64,60,62,59,
                    52,53,50,45,45,53,52,57,50,52};
    v_type = {1,-1,-3};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_1sp_fig168() {
    cout << "Start test_4v_1sp_fig168" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,69,69,72,72,72,76,76,74,71,
                    68,69,65,67,69,65,72,72,69,68,
                    52,53,50,52,53,53,52,57,50,52};
    v_type = {1,1,-2};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_1sp_fig169() {
    cout << "Start test_4v_1sp_fig169" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {69,67,65,69,70,71,69,69,65,65,64,65,
                    60,60,60,62,65,67,65,64,62,57,60,57,
                    53,52,53,50,46,43,50,45,50,50,48,41};
    v_type = {0,-1,-3};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getUpper_1()->getHInterval() << endl;
    // cout << problem->getUpper_2()->getHInterval() << endl;
    // cout << problem->getUpper_3()->getHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getFirstHInterval() << endl;
    // cout << problem->getCantusFirmus()->getFirstHInterval() << endl;
    // cout << problem->getCounterpoint_2()->getFirstHInterval() << endl;
    find_unsat_constraints();
}

void FigureTests::test_4v_1sp_fig170() {
    cout << "Start test_4v_1sp_fig170" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {60,60,60,60,58,59,57,69,65,65,64,65,
                    57,55,53,57,53,55,53,52,53,57,60,57,
                    57,55,53,57,53,55,53,52,53,57,60,57,
                    53,52,53,41,46,43,50,45,50,50,48,41};
    v_type = {-1,-2,-3};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_1sp_fig171() {
    cout << "Start test_4v_1sp_fig171" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cp =           {69,67,65,65,65,67,65,64,65,65,64,65,
                    60,60,60,62,62,58,57,57,53,57,60,57,
                    53,52,53,50,46,43,50,45,50,50,48,41};
    v_type = {2,1,-1};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCounterpoint_1()->getFirstHInterval() << endl;
    // cout << problem->getCounterpoint_2()->getFirstHInterval() << endl;
    // cout << problem->getCantusFirmus()->getFirstHInterval() << endl;
    find_unsat_constraints();
}

void FigureTests::test_4v_1sp_fig172() {
    cout << "Start test_4v_1sp_fig172" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; 
    cp =           {72,71,72,69,69,72,72,72,72,69,70,65,
                    69,67,64,65,65,67,69,67,65,65,64,65,
                    65,62,60,60,62,60,60,64,60,62,58,60};
    v_type = {3,2,1};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_2sp_fig173() {
    cout << "Start test_4v_2sp_fig173" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {57,62,55,55,59,62,60,64,62,61,62,
                    -1,53,57,59,60,55,59,57,55,52,57,53,52,53,52,60,57,53,57,57,57,
                    -1,53,57,59,60,55,59,57,55,52,57,53,52,53,52,60,57,53,57,57,57,
                    50,50,48,55,52,50,45,48,50,45,50};   
    v_type = {-1,-1,-2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getLowest()->getFirstNotes() << endl;
    // cout << problem->getLowest()->getMelodicIntervals() << endl;
    // cout << problem->getCantusFirmus()->getMotions() << endl;
    // cout << problem->getCounterpoint_1()->getMotions() << endl;
    // cout << problem->getCounterpoint_2()->getMotions() << endl;
    // cout << problem->getCounterpoint_3()->getMotions() << endl;
    find_unsat_constraints();
}

void FigureTests::test_4v_2sp_fig174() {
    cout << "Start test_4v_2sp_fig174" << endl;
    spList = {SECOND_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {-1,65,69,71,72,67,71,74,76,71,74,77,76,72,76,72,69,74,73,73,74,
                    57,53,55,55,55,57,60,60,53,57,57,
                    57,53,55,55,55,57,60,60,53,57,57,
                    50,50,48,55,52,50,45,48,50,45,50};   
    v_type = {1,-1,-2};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_2sp_fig175() {
    cout << "Start test_4v_2sp_fig175" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {74,74,72,74,76,77,77,76,74,73,74,
                    69,69,69,71,71,74,72,72,69,69,69,
                    -1,62,50,53,57,60,55,53,52,55,50,52,53,57,60,48,50,53,57,45,50};   
    v_type = {2,1,-2};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_2sp_fig176() {
    cout << "Start test_4v_2sp_fig176" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {50,53,52,50,55,53,57,55,53,52,50}; 
    cp =            {65,   69,   72,   74,   70,   69,   69,   71,   74,   73,   74,
                     -1,62,60,62,64,67,65,64,62,64,65,69,64,65,67,62,65,69,67,64,66,
                     57,   57,   55,   57,   58,   60,   60,   62,   57,   64,   57};
    v_type = {3, 2, 1};
    
    find_unsat_constraints();
}
    
void FigureTests::test_4v_3sp_fig183() {
    cout << "Start test_4v_3sp_fig183" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,64,71,72,69,72,69,67,65,62,64,65,67,64,65,67,69,65,69,71,72,69,72,74,76,71,76,74,72,76,72,71,69,74,69,71,68, 
                    56,57,57,55,60,57,59,60,62,59,
                    56,57,57,55,60,57,59,60,62,59,
                    52,53,50,52,53,53,52,57,50,52};   
    v_type = {0, -1, -2};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_3sp_fig184(){
    cout << "Start test_4v_3sp_fig184" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {76,72,74,72,69,81,79,76,77,76}; 
    cp =           {68,71,64,67,69,72,69,67,65,62,64,65,67,65,64,62,60,72,69,67,65,67,69,71,72,74,76,71,72,69,71,72,69,74,69,71,68,
                    59,57,57,55,57,60,64,60,62,59,
                    52,53,50,52,53,53,52,57,50,52};
    v_type = {-2, -4, -5};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_3sp_fig185(){
    cout << "Start test_4v_3sp_fig185" << endl;
    spList = {FIRST_SPECIES, THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,69,69,72,72,72,76,69,69,68, 
                    56,59,52,55,57,60,57,55,53,57,55,53,55,52,64,62,60,59,57,55,53,55,57,59,60,62,64,62,60,57,60,59,57,50,62,60,59,  
    cp =           {71,69,69,72,72,72,76,69,69,68, 
                    56,59,52,55,57,60,57,55,53,57,55,53,55,52,64,62,60,59,57,55,53,55,57,59,60,62,64,62,60,57,60,59,57,50,62,60,59,  
                    52,53,50,52,53,53,48,48,50,52};   
    v_type = {1, -2, -2};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_3sp_fig186(){
    cout << "Start test_4v_3sp_fig186" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, THIRD_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,76,77,76,77,77,76,76,69,68,
                    68,69,69,72,72,72,72,72,74,71,
                    64, 62, 60, 59, 57, 55, 53, 52, 50, 52, 53, 50, 57, 60, 57, 55, 53, 55, 57, 55, 53, 55, 57, 59, 60, 52, 53, 55, 57, 55, 53, 52, 50, 53, 52, 50, 52};
    v_type = {1, 1, -2};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCounterpoint_3()->getIs5QNArray() << endl;
    // cout << problem->getCounterpoint_3()->getConsonance() << endl;
    find_unsat_constraints();
}

void FigureTests::test_4v_4sp_fig196(){
    cout << "Start test_4v_4sp_fig196" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =            {57,57,62,62,60,60,59,59,62,62,57,57,65,65,64,64,62,62,61,62,
                    53, 57, 57, 50, 62, 62, 60, 60, 57, 57, 57,
                    53, 57, 57, 50, 62, 62, 60, 60, 57, 57, 57,
                     50,50,45,47,43,50,53,48,50,45,50};
    v_type = {-1, -1, -3};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getUpper_1()->getHInterval() << endl;
    // cout << problem->getUpper_2()->getHInterval() << endl;
    // cout << problem->getUpper_3()->getHInterval() << endl;
    // cout << problem->getCantusFirmus()->getHInterval() << endl;
    // cout << problem->getCounterpoint_1()->getHInterval() << endl;
    // cout << problem->getCounterpoint_2()->getHInterval() << endl;
    find_unsat_constraints();
}

void FigureTests::test_4v_5sp_fig200(){
    cout << "Start test_4v_5sp_fig200" << endl;
    spList = {FIFTH_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =            {
                    -1,-1,69,-1,
                    69,-1,69,71,
                    72,67,72,-1,
                    72,-1,71,-1,
                    71,-1,71,73,
                    74,69,74,-1,
                    74,-1,77,-1,
                    77,-1,76,-1,
                    76,-1,74,-1,
                    74,-1,73,-1,
                    74,

                    57, 53, 55, 55, 55, 57, 57, 60, 57, 57, 57,
                    57, 53, 55, 55, 55, 57, 57, 60, 57, 57, 57,
                    50,50,48,55,52,50,53,48,50,45,50};
    notesSpeciesFor5sp = {
                        -1, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,-1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,
    };
    v_type = {1, -1, -2};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_5sp_fig201() {
    cout << "Start test_4v_5sp_fig201" << endl;
    spList = {FIFTH_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =            {
                    -1, -1, 57, -1,
                    57, -1, 57, 59,
                    60, 55, 60, -1, 
                    60, -1, 59, -1, 
                    59, -1, 59, 61, 
                    62, 50, 62, -1, 
                    62, -1, 65, -1, 
                    65, -1, 64, -1, 
                    64, -1, 62, -1, 
                    62, -1, 61, -1, 
                    62,

                    57, 53, 55, 55, 55, 57, 57, 60, 57, 57, 57,
                    -1, -1, 57, -1,
                    57, -1, 57, 59,
                    60, 55, 60, -1, 
                    60, -1, 59, -1, 
                    59, -1, 59, 61, 
                    62, 50, 62, -1, 
                    62, -1, 65, -1, 
                    65, -1, 64, -1, 
                    64, -1, 62, -1, 
                    62, -1, 61, -1, 
                    62,

                    57, 53, 55, 55, 55, 57, 57, 60, 57, 57, 57,
                    50,50,48,55,52,50,53,48,50,45,50};
    notesSpeciesFor5sp = {
                        -1, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,-1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, THIRD_SPECIES, THIRD_SPECIES,
                        THIRD_SPECIES, THIRD_SPECIES, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES, -1, FOURTH_SPECIES, -1,
                        FOURTH_SPECIES,
    };
    v_type = {-1, -1, -2};
    
    find_unsat_constraints();
}

void FigureTests::test_4v_Xsp_fig204() {
    cout << "Start test_4v_Xsp_fig204" << endl;
    spList = {SECOND_SPECIES, THIRD_SPECIES, FOURTH_SPECIES};
    cantusFirmus =  {50,53,52,50,55,53,57,55,53,52,50}; 
    cp = {  69,74,72,69,71,72,74,62,70,67,69,65,72,69,70,67,69,65,67,64,66, 
    cp = {  69,74,72,69,71,72,74,62,70,67,69,65,72,69,70,67,69,65,67,64,66, 
            62,64,65,67,69,67,65,69,67,65,64,67,65,62,65,64,62,60,58,62,65,67,69,65,64,62,60,57,62,60,58,55,60,59,57,62,64,52,53,55,57,
            57, 57, 62, 62, 60, 60, 58, 58, 62, 62, 60, 60, 65, 65, 64, 64, 62, 62, 61, 62,
        };
    v_type = {2,2,1};
    // CounterpointProblem* problem = set_configuration();
    // has_solution(problem);
    // cout << problem->getCantusFirmus()->getFirstSpeciesMotions() << endl;
    // cout << problem->getCounterpoint_1()->getFirstSpeciesMotions() << endl;
    // cout << problem->getCounterpoint_2()->getFirstSpeciesMotions() << endl;
    // cout << problem->getCounterpoint_3()->getFourthSpeciesMIntervals() << endl;
    // cout << problem->getCounterpoint_3()->getFirstSpeciesMotions() << endl;
    find_unsat_constraints();
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
    test_2v_5sp_fig82();
    test_2v_5sp_fig83();
    test_2v_5sp_fig87_1();
    test_2v_5sp_fig82();
    test_2v_5sp_fig83();
    test_2v_5sp_fig87_1();
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
    // Three voice fifth species figures
    test_3v_5sp_fig154();
    test_3v_5sp_fig155();
    test_3v_5sp_fig156();
    // Three voice fifth species figures
    test_3v_5sp_fig154();
    test_3v_5sp_fig155();
    test_3v_5sp_fig156();
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
    // Four voice fifth species figures
    test_4v_5sp_fig201();
}

void FigureTests:: run_thirdSpecies_tests() {
    cout << "Running third species tests..." << endl;
    // Two voice third species figures
    test_2v_3sp_fig55();
    test_2v_3sp_fig56();
    test_2v_3sp_fig57();
    test_2v_3sp_fig58();
    test_2v_3sp_fig59();
    test_2v_3sp_fig60();
    // Three voice third species figures
    test_3v_3sp_fig130();
    test_3v_3sp_fig131();
    test_3v_3sp_fig132();
    test_3v_3sp_fig133();
    // Four voice third species figures
    test_4v_3sp_fig183();
    test_4v_3sp_fig184();
    test_4v_3sp_fig185();
    test_4v_3sp_fig186();
    // Four voice fifth species figures
    test_4v_5sp_fig201();
}

void FigureTests:: run_thirdSpecies_tests() {
    cout << "Running third species tests..." << endl;
    // Two voice third species figures
    test_2v_3sp_fig55();
    test_2v_3sp_fig56();
    test_2v_3sp_fig57();
    test_2v_3sp_fig58();
    test_2v_3sp_fig59();
    test_2v_3sp_fig60();
    // Three voice third species figures
    test_3v_3sp_fig130();
    test_3v_3sp_fig131();
    test_3v_3sp_fig132();
    test_3v_3sp_fig133();
    // Four voice third species figures
    test_4v_3sp_fig183();
    test_4v_3sp_fig184();
    test_4v_3sp_fig185();
    test_4v_3sp_fig186();
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

void FigureTests::run_fifthSpecies_tests() {
    cout << "Running fifth species tests..." << endl;
    test_2v_5sp_fig82();
    test_2v_5sp_fig83();
    test_2v_5sp_fig87_1();
    test_3v_5sp_fig154();
    test_3v_5sp_fig155();
    test_3v_5sp_fig156();
    test_4v_5sp_fig201();
}

void FigureTests::run_all_tests() {
    cout << "Running all figure tests..." << endl;
    run_twoVoice_tests();
    run_threeVoice_tests();
    run_fourVoice_tests();
    test_4v_Xsp_fig204();
    cout << "All figure tests completed." << endl;
} 

// Add to the MUSTest method to include our new test
// Add to the MUSTest method to include our new test
void FigureTests::MUSTest() {
    cout << "Running MUSTest..." << endl;
    
    // Test cost summing first
    // test_cost_summing();
    
    // Then run the regular tests
    // run_all_tests();
    
    cout << "MUSTest completed." << endl;
}

void FigureTests::quickTest() {
    test_2v_1sp_fig22();
    // test_2v_2sp_fig39();
    // test_2v_2sp_fig40();
    // test_2v_2sp_fig41();
    // test_2v_2sp_fig42();
    // test_2v_2sp_fig43();
    // test_2v_2sp_fig44();
    // test_2v_2sp_fig45();
    // test_2v_3sp_fig56();
    // test_2v_3sp_fig57();
    // test_2v_3sp_fig58();
    test_2v_4sp_fig74();
    // test_2v_4sp_fig76();
    // test_2v_4sp_fig77();
    // test_2v_5sp_fig82();
    // test_2v_5sp_fig83();
    // test_3v_2sp_fig127();
    // test_3v_2sp_fig128();
    // test_3v_4sp_fig147();
    // test_3v_4sp_fig151();
    // test_3v_5sp_fig154();
    // test_3v_5sp_fig156();
    // test_4v_1sp_fig169();
    // test_4v_1sp_fig171();
    // test_4v_2sp_fig173();
    // test_4v_3sp_fig183();
    // test_4v_3sp_fig184();
    // test_4v_3sp_fig186();
    // test_4v_4sp_fig196();
    // test_4v_Xsp_fig204();
}

void FigureTests::quickTest() {
    test_2v_1sp_fig22();
    // test_2v_2sp_fig39();
    // test_2v_2sp_fig40();
    // test_2v_2sp_fig41();
    // test_2v_2sp_fig42();
    // test_2v_2sp_fig43();
    // test_2v_2sp_fig44();
    // test_2v_2sp_fig45();
    // test_2v_3sp_fig56();
    // test_2v_3sp_fig57();
    // test_2v_3sp_fig58();
    test_2v_4sp_fig74();
    // test_2v_4sp_fig76();
    // test_2v_4sp_fig77();
    // test_2v_5sp_fig82();
    // test_2v_5sp_fig83();
    // test_3v_2sp_fig127();
    // test_3v_2sp_fig128();
    // test_3v_4sp_fig147();
    // test_3v_4sp_fig151();
    // test_3v_5sp_fig154();
    // test_3v_5sp_fig156();
    // test_4v_1sp_fig169();
    // test_4v_1sp_fig171();
    // test_4v_2sp_fig173();
    // test_4v_3sp_fig183();
    // test_4v_3sp_fig184();
    // test_4v_3sp_fig186();
    // test_4v_4sp_fig196();
    // test_4v_Xsp_fig204();
}