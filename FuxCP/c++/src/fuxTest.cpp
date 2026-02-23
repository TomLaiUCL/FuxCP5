// 
// Created by Luc Cleenewerk and Diego de Patoul. 
// Modified by Tom Lai.
// This file contains the testing framework implementation.  
//
#include <iostream>
#include <fstream>  // For file operations
#include <cmath>
#include <chrono>
#include <signal.h> // For testing blocking constrains in generations
#include "../headers/fuxTest.hpp"
#include "../headers/Parts/Midi.hpp"
#include <gecode/int.hh> // Ensure you include the necessary Gecode headers


// ========== Modified by Tom ====================

FuxTest::FuxTest(char* test){
    cantusFirmus = {60,   62,   65,   64,   67,   65,   64,   62,   60};
    cfSize = cantusFirmus.size();
    melodic_params = {0, 1, 2, 576, 5, 10, 25, 40}; // Last ones augmented to avoid crazy leaps
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    borrowMode = 1;
    if(strcmp(test, "all")==0){
        cout << "Running all tests..." << endl;
        test_1H1();
        test_1H2();
        test_1H3();
        test_1H4();
        test_1H5();
        // test_1H6();
        test_1H7();
        test_1H10();
        test_1M2();

        test_1H2();
        test_2M2();
        cout << "Tests ended." << endl;
    } else if(strcmp(test, "1H1")==0){
        test_1H1();
    } else if(strcmp(test, "1H2")==0){
        test_1H2();
    } else if(strcmp(test, "1H3")==0){
        test_1H3();
    } else if(strcmp(test, "1H4")==0){
        test_1H4();
    } else if(strcmp(test, "1H5")==0){
        test_1H5();
    // } else if(strcmp(test, "1H6")==0){
    //     test_1H6();
    } else if(strcmp(test, "1H7")==0){
        test_1H7();
    } else if(strcmp(test, "1H10")==0){
        test_1H10();
    } else if(strcmp(test, "1M2")==0){
        test_1M2();
    } else if(strcmp(test, "2H2")==0){
        test_2H2();
    } else if(strcmp(test, "2M2")==0){
        test_2M2();
    } else if(strcmp(test, "figs")==0){
        test_2v_1sp_fig22();
        test_2v_1sp_fig23();
        test_2v_2sp_fig38();
        test_2v_2sp_fig39();
        test_2v_2sp_fig40();
        test_2v_2sp_fig41();
        test_2v_2sp_fig42();
        test_2v_2sp_fig43();
        test_2v_2sp_fig44();
        test_2v_2sp_fig45();
        test_2v_3sp_fig55();
        test_2v_3sp_fig56();
        test_2v_3sp_fig57();
        test_2v_3sp_fig58();
        test_2v_3sp_fig59();
        test_2v_3sp_fig60();
        test_2v_4sp_fig74();
        test_2v_4sp_fig75();
        test_2v_4sp_fig76();
        test_2v_4sp_fig77();
        test_2v_4sp_fig78();
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
        test_3v_2sp_fig125();
        test_3v_2sp_fig128();
        test_3v_2sp_fig129();
        test_3v_3sp_fig132();
        test_3v_3sp_fig133();
        test_4v_2sp_fig176();
    } else if(strcmp(test, "bryce")==0){
        test_bryce();
    } else if(strcmp(test, "bryce_2")==0){
        test_bryce_2();
    } else if(strcmp(test, "bryce_all")==0){
        test_bryce_all();
    } else if(strcmp(test, "bryce_classic")==0){
        test_bryce_classic();
    } else if(strcmp(test, "bryce_fullsp")==0){
        test_bryce_fullsp();
    } 
    else if(strcmp(test, "sacha")==0){
        test_sacha();
    }
    else {
        std::invalid_argument("Test for constraint not found!");
    }
    //     CounterpointProblem* problem;
    //     problem = dispatcher(test);
    //     BAB<CounterpointProblem> e(problem);
    //     int nb_sol = 0;
    //     while(CounterpointProblem* pb = e.next()){
    //         nb_sol++;
    //         cout << pb->to_string() << endl;
    //         delete pb;
    //         if (nb_sol >= 1)
    //             break;
    //     }
    //     if(nb_sol==0){
    //         cout << "This constraint works. It prohibits a forbidden configuration." << endl;
    //     } else {
    //         cout << "This constraint does NOT work. It allows a forbidden configuration." << endl;
    //     }
    // }

}

/*
Chromatic melodies are forbidden for two and three voice composition, and to be avoided for four voice composition.
*/
void FuxTest::test_G6() {
    cout << "Start tests G6..." << endl;
    test_G6_2v_1sp();
    cout << "End tests G6." << endl;
}

void FuxTest::test_G6_2v_1sp() {
    // TODO
    // spList = {FIRST_SPECIES};
    // v_type = {0};
    // auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    // std::vector<CounterpointProblem*> solutions = get_all_solutions(problem);
    // for (CounterpointProblem* solution: solutions) {
    //     if (solution->getSolutionArray()[0].val() % 12 != cantusFirmus[0] % 12) {
    //         std::cerr   << "/!\\ ERROR test 1H4_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
    //                     << "cantus firmus: ";
    //         printVector(cantusFirmus);
    //         std::cerr   << "solution array: ";
    //         printIntVarArray(problem->getSolutionArray());
    //         std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
    //     }
    //     if (solution->getSolutionArray()[cfSize-1].val() % 12 != cantusFirmus[0] % 12) { 
    //         std::cerr   << "/!\\ ERROR test 1H4_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
    //                     << "cantus firmus: ";
    //         printVector(cantusFirmus);
    //         std::cerr   << "solution array: ";
    //         printIntVarArray(problem->getSolutionArray());
    //         std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
    //     }
    // }            
    // delete problem;
}

/*
All notes on the downbeat are consonant with the notes (on the downbeat) of the lowest stratum. 
*/
void FuxTest::test_1H1() {
    cout << "Start tests 1H1..." << endl;
    test_1H1_2v_1sp();
    test_1H1_3v_1sp();
    test_1H1_4v_1sp();
    test_1H1_2v_2sp();
    test_1H1_3v_2sp();
    test_1H1_4v_2sp();
    test_1H1_2v_3sp();
    test_1H1_3v_3sp();
    test_1H1_4v_3sp();
    test_1H1_2v_4sp();
    test_1H1_3v_4sp();
    test_1H1_4v_4sp();
    test_1H1_2v_5sp();
    cout << "End tests 1H1." << endl;
}

void FuxTest::test_1H1_2v_1sp() {
    cout << "Start test 1H1_2v_1sp..." << endl;
    // define dissonant intervals
    int dis[] = {1, 2, 5, 6, 10, 11}; 
    // define the species
    spList = {FIRST_SPECIES};
    // define the voice types (=pitch range):
    // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    v_type = {3};
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 0; i < cfSize; i++) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[i] + 12 + interval; // note is dissonant
            rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, note); // fix the note i
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H1_2v_1sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
            }
            delete problem;
        }  
    }
    cout << "End test 1H1_2v_1sp..." << endl;
}

void FuxTest::test_1H1_3v_1sp() {
    cout << "Start test 1H1_3v_1sp..." << endl;
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    int cons[] = {3, 4, 7, 8, 9}; // conssonant intervals without 0 because 1H5
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t j = 0; j < 2; j++) { // iterate over each solution line
            for (size_t i = 0; i < cfSize; i++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[j*cfSize+i], IRT_EQ, note); // fix the note i
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 1H1_3v_1sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
                }
                delete problem;
            }  
        }    
    }
    cout << "End test 1H1_3v_1sp..." << endl;
}

void FuxTest::test_1H1_4v_1sp() {
    cout << "Start test 1H1_4v_1sp..." << endl;
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    int cons[] = {3, 4, 7, 8, 9}; // conssonant intervals without 0 because 1H5
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, 3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t j = 0; j < 2; j++) { // iterate over each solution line
            for (size_t i = 0; i < cfSize; i++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[j*cfSize+i], IRT_EQ, note); // fix the note i
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 1H1_4v_1sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
                }
                delete problem;
            }  
        }
    }
    cout << "End test 1H1_4v_1sp..." << endl;
}

void FuxTest::test_1H1_2v_2sp() {
    cout << "Start test 1H1_2v_2sp..." << endl;
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    int cons[] = {3, 4, 7, 8, 9}; // conssonant intervals without 0 because 1H5
    spList = {SECOND_SPECIES};
    v_type = {3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 1; i < cfSize; i++) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[i] + 12 + interval; // note is dissonant
            rel(problem->getHome(), problem->getSolutionArray()[i*2], IRT_EQ, note); // fix the note i (downbeat)
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H1_2v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
            }
            delete problem;
        }  
    }
    cout << "End test 1H1_2v_2sp..." << endl;
}

void FuxTest::test_1H1_3v_2sp() {
    cout << "Start test 1H1_3v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    int cons[] = {3, 4, 7, 8, 9}; // conssonant intervals without 0 because 1H5
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t j = 0; j < 2; j++) { // iterate over each solution line
            for (size_t i = 1; i < cfSize; i++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize)+(i*2)], IRT_EQ, note); // fix the note i
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 1H1_3v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
                }
                delete problem;
            }  
        }    
    }
    cout << "End test 1H1_3v_2sp..." << endl;
}

void FuxTest::test_1H1_4v_2sp() {
    cout << "Start test 1H1_4v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    int cons[] = {3, 4, 7, 8, 9}; // conssonant intervals without 0 because 1H5
    spList = {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, 3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t j = 0; j < 3; j++) { // iterate over each solution line
            for (size_t i = 1; i < cfSize; i++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize)+(i*2)], IRT_EQ, note); // fix the note i
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 1H1_4v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
                }
                delete problem;
            }  
        }    
    }
    cout << "End test 1H1_4v_2sp..." << endl;
}

void FuxTest::test_1H1_2v_3sp() {
    cout << "Start test 1H1_2v_3sp..." << endl;
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    spList = {THIRD_SPECIES};
    v_type = {3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 0; i < cfSize; i++) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[i] + 12 + interval; // note is dissonant
            rel(problem->getHome(), problem->getSolutionArray()[i*4], IRT_EQ, note); // fix the note i (downbeat)
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H1_2v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
            }
            delete problem;
        }  
    }
    cout << "End test 1H1_2v_3sp..." << endl;
}

void FuxTest::test_1H1_3v_3sp() {
    cout << "Start test 1H1_3v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {3, 3};
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 0; i < cfSize; i++) {
            for (size_t j = 0; j < 2; j++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize) + (i * 4)], IRT_EQ, note); // fix the downbeat of each mesure
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 1H1_3v_3sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the measure " << i << std::endl;
                }
                delete problem;
            }
        }
    }
    cout << "End test 1H1_3v_3sp..." << endl;
}

void FuxTest::test_1H1_4v_3sp() {
    cout << "Start test 1H1_4v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    spList = {THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    v_type = {3, 3, 3};
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 0; i < cfSize; i++) {
            for (size_t j = 0; j < 3; j++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize) + (i * 4)], IRT_EQ, note); // fix the downbeat of each mesure
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 1H1_4v_3sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the measure " << i << std::endl;
                }
                delete problem;
            }
        }
    }
    cout << "End test 1H1_4v_3sp..." << endl;
}

void FuxTest::test_1H1_2v_4sp() {
    cout << "Start test 1H1_2v_4sp..." << endl;
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    spList = {FOURTH_SPECIES};
    v_type = {3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 1; i < cfSize; i++) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            if (i == cfSize-1) { // last note
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[(i*2)-1], IRT_EQ, note); // fix the note i (downbeat)
            } else { // other notes
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[i*2], IRT_EQ, note); // fix the note i (downbeat)
            }
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H1_2v_4sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
            }
            delete problem;
        }  
    }
    cout << "End test 1H1_2v_4sp..." << endl;
}

void FuxTest::test_1H1_3v_4sp() {
    cout << "Start test 1H1_3v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    spList = {FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 1; i < cfSize; i++) {
            for (size_t j = 0; j < 2; j++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                if (i == cfSize-1) { // last note
                    int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                    rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize)+(i*2)-1], IRT_EQ, note); // fix the note i (downbeat)
                } else { // other notes
                    int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                    rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize)+(i*2)], IRT_EQ, note); // fix the note i (downbeat)
                }
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 1H1_3v_4sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
                }
                delete problem;
            }
        }  
    }
    cout << "End test 1H1_3v_4sp..." << endl;
}

void FuxTest::test_1H1_4v_4sp() {
    cout << "Start test 1H1_4v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    spList = {FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {3, 3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 1; i < cfSize; i++) {
            for (size_t j = 0; j < 3; j++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                if (i == cfSize-1) { // last note
                    int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                    rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize)+(i*2)-1], IRT_EQ, note); // fix the note i (downbeat)
                } else { // other notes
                    int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                    rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize)+(i*2)], IRT_EQ, note); // fix the note i (downbeat)
                }
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 1H1_4v_4sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
                }
                delete problem;
            }
        }  
    }
    cout << "End test 1H1_4v_4sp..." << endl;
}

void FuxTest::test_1H1_2v_5sp() {
    cout << "Start test 1H1_2v_5sp..." << endl;
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    spList = {FIFTH_SPECIES};
    v_type = {3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t i = 0; i < cfSize; i++) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[i] + 12 + interval; // note is dissonant
            rel(problem->getHome(), problem->getSolutionArray()[i*4], IRT_EQ, note); // fix the note i (downbeat)
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H1_2v_5sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
            }
            delete problem;
        }  
    }
    cout << "End test 1H1_2v_5sp..." << endl;
}

// void FuxTest::test_1H1_3v_5sp() {
//     cout << "Start test 1H1_3v_5sp..." << endl;
//     int cpSize = cfSize*2-2; // size of a counterpoint
//     int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
//     spList = {FOURTH_SPECIES, FOURTH_SPECIES};
//     v_type = {3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
//     // Test that dissonant notes are forbidden
//     for (int interval : dis) {
//         for (size_t i = 1; i < cfSize; i++) {
//             for (size_t j = 0; j < 2; j++) {
//                 CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
//                 if (i == cfSize-1) { // last note
//                     int note = cantusFirmus[i] + 12 + interval; // note is dissonant
//                     rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize)+(i*2)-1], IRT_EQ, note); // fix the note i (downbeat)
//                 } else { // other notes
//                     int note = cantusFirmus[i] + 12 + interval; // note is dissonant
//                     rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize)+(i*2)], IRT_EQ, note); // fix the note i (downbeat)
//                 }
//                 if (has_solution(problem)) {
//                     std::cerr   << "/!\\ ERROR test 1H1_3v_4sp: It exists a solution but it shouldn't with the following configuration:\n"
//                                 << "Cantus firmus: ";
//                     printVector(cantusFirmus);
//                     std::cerr   << "Solution array: ";
//                     printIntVarArray(problem->getSolutionArray());
//                     std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
//                 }
//                 delete problem;
//             }
//         }  
//     }
//     cout << "End test 1H1_3v_5sp..." << endl;
// }


/*
In two voice composition, the first harmonic interval must be a perfect consonance.
*/
void FuxTest::test_1H2() {
    cout << "Start tests 1H2..." << endl;
    test_1H2_2v_1sp();
    test_1H2_2v_2sp();
    test_1H2_2v_3sp();
    test_1H2_2v_4sp();
    cout << "End tests 1H2." << endl;
}

void FuxTest::test_1H2_2v_1sp() {
    cout << "Start test 1H2_2v_1sp..." << endl;
    int p_cons[] = {0, 7}; // perfect conssonant intervals
    int non_p_cons[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11}; // non-perfect conssonant intervals
    spList = {FIRST_SPECIES};
    v_type = {3};
    for (int interval : p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, cantusFirmus[0]+12+interval); // fix the first solution note as perfect consonant with cf
        if (!has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H2_2v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First harmonic interval should be correct" << std::endl;
        }
        delete problem;
    }
    for (int interval : non_p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, cantusFirmus[0]+12+interval); // fix the first solution note as non-perfect consonant with cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H2_2v_1sp: It exists a solution but it shouldn't with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Non perfect consonance at the first mesure" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H2_2v_1sp..." << endl;
}

void FuxTest::test_1H2_2v_2sp() {
    cout << "Start test 1H2_2v_2sp..." << endl;
    int p_cons[] = {0, 7}; // perfect conssonant intervals
    int non_p_cons[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11}; // non-perfect conssonant intervals
    spList = {SECOND_SPECIES};
    v_type = {3};
    for (int interval : p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, cantusFirmus[0]+12+interval); // fix the first solution note as perfect consonant with cf
        if (!has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H2_2v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First harmonic interval should be correct" << std::endl;
        }
        delete problem;
    }
    for (int interval : non_p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, cantusFirmus[0]+12+interval); // fix the first solution note as non-perfect consonant with cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H2_2v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Non perfect consonance at the first mesure" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H2_2v_2sp..." << endl;
}

void FuxTest::test_1H2_2v_3sp() {
    cout << "Start test 1H2_2v_3sp..." << endl;
    int p_cons[] = {0, 7}; // perfect conssonant intervals
    int non_p_cons[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11}; // non-perfect conssonant intervals
    spList = {THIRD_SPECIES};
    v_type = {3};
    // perfect conssonant are allowed
    for (int interval : p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + 12 + interval; // note is perfect consonant
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first solution note as perfect consonant with cf
        if (!has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H2_2v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First harmonic interval should be correct" << std::endl;
        }
        delete problem;
    }
    // non-perfect consonant are forbidden
    for (int interval : non_p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + 12 + interval; // note is non-perfect consonant
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first solution note as non-perfect consonant with cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H2_2v_3sp: It exists a solution but it shouldn't with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Non perfect consonance at the first mesure" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H2_2v_3sp..." << endl;
}

void FuxTest::test_1H2_2v_4sp() {
    cout << "Start test 1H2_2v_4sp..." << endl;
    int p_cons[] = {0, 7}; // perfect conssonant intervals
    int non_p_cons[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11}; // non-perfect conssonant intervals
    spList = {FOURTH_SPECIES};
    v_type = {3};
    // perfect conssonant are allowed
    for (int interval : p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + 12 + interval; // note is perfect consonant
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first solution note as perfect consonant with cf
        if (!has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H2_2v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First harmonic interval should be correct" << std::endl;
        }
        delete problem;
    }
    // non-perfect consonant are forbidden
    for (int interval : non_p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + 12 + interval; // note is non-perfect consonant
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first solution note as non-perfect consonant with cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H2_2v_4sp: It exists a solution but it shouldn't with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Non perfect consonance at the first mesure" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H2_2v_4sp..." << endl;
}


/*
In two voice composition, the last harmonic in-terval must be a perfect consonance. 
When composing for three or four voices, the last chord should be composed only of notes of the harmonic triad.
*/
void FuxTest::test_1H3() {
    cout << "Start tests 1H3..." << endl;
    test_1H3_2v_1sp();
    test_1H3_3v_1sp();
    test_1H3_4v_1sp();
    test_1H3_2v_2sp();
    test_1H3_3v_2sp();
    test_1H3_4v_2sp();
    test_1H3_2v_3sp();
    test_1H3_3v_3sp();
    test_1H3_4v_3sp();
    test_1H3_2v_4sp();
    test_1H3_3v_4sp();
    test_1H3_4v_4sp();
    cout << "End tests 1H3." << endl;
}

void FuxTest::test_1H3_2v_1sp() {
    cout << "Start test 1H3_2v_1sp..." << endl;
    int p_cons[] = {0, 7}; // perfect conssonant intervals
    int non_p_cons[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11}; // non-perfect conssonant intervals
    spList = {FIRST_SPECIES};
    v_type = {0};
    // non-perfect consonant intervals are forbidden
    for (int interval : non_p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[cfSize-1], IRT_EQ, cantusFirmus[cfSize-1]+interval); // fix the last note with a not perfect consonance
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H3_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last harmonic interval must be a perfect consonance" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H3_2v_1sp..." << endl;
}

void FuxTest::test_1H3_3v_1sp() {
    cout << "Start test 1H3_3v_1sp..." << endl;
    int triad_cons[] = {0, 3, 4, 7}; // harmonic triad intervals
    int non_triad_cons[] = {1, 2, 5, 6, 8, 9, 10, 11}; // non-harmonic triad intervals
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, 3};
    // non-perfect consonant intervals are forbidden
    for (size_t i = 0; i < 2; i++) { // iterate over each solution line
        for (int interval : non_triad_cons) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[(cfSize*i)+cfSize-1], IRT_EQ, cantusFirmus[cfSize-1] + 12 + interval); // fix the last note as a note not in harmonic triad intervals
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    // perfect consonant intervals are allowed
    for (int interval : triad_cons) {
        for (size_t i = 0; i < 2; i++) { // iterate over each solution line
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[(cfSize*i)+cfSize-1], IRT_EQ, cantusFirmus[cfSize-1] + 12 + interval); // fix the last note as a note in harmonic triad intervals
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_3v_1sp: It doesn't exist solution but it should with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H3_3v_1sp..." << endl;
}

void FuxTest::test_1H3_4v_1sp() {
    cout << "Start test 1H3_4v_1sp..." << endl;
    int triad_cons[] = {0, 3, 4, 7}; // harmonic triad intervals
    int non_triad_cons[] = {1, 2, 5, 6, 8, 9, 10, 11}; // non-harmonic triad intervals
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, 3, 3};
    // non-perfect consonant intervals are forbidden
    for (size_t i = 0; i < 3; i++) { // iterate over each solution line
        for (int interval : non_triad_cons) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[(cfSize*i)+cfSize-1], IRT_EQ, cantusFirmus[cfSize-1]+12+interval); // fix the last note as a note not in harmonic triad intervals
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_4v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    // perfect consonant intervals are allowed
    for (int interval : triad_cons) {
        for (size_t i = 0; i < 3; i++) { // iterate over each solution line
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[(cfSize*i)+cfSize-1], IRT_EQ, cantusFirmus[cfSize-1] + +12 + interval); // fix the last note as a note in harmonic triad intervals
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_4v_1sp: It doesn't exist solution but it should with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H3_4v_1sp..." << endl;
}

void FuxTest::test_1H3_2v_2sp() {
    cout << "Start test 1H3_2v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    int p_cons[] = {0, 7}; // perfect conssonant intervals
    int non_p_cons[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11}; // non-perfect conssonant intervals
    spList = {SECOND_SPECIES};
    v_type = {0};
    // non-perfect consonant intervals are forbidden
    for (int interval : non_p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[cfSize-1] + interval;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note with a not perfect consonance
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H3_2v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last harmonic interval must be a perfect consonance" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H3_2v_1sp..." << endl;
}

void FuxTest::test_1H3_3v_2sp() {
    cout << "Start test 1H3_3v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    int triad_cons[] = {0, 3, 4, 7}; // harmonic triad intervals
    int non_triad_cons[] = {1, 2, 5, 6, 8, 9, 10, 11}; // non-harmonic triad intervals
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {0, 0};
    // non-perfect consonant intervals are forbidden
    for (size_t i = 0; i < 2; i++) { // iterate over each solution line
        for (int interval : non_triad_cons) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(cpSize*i)+cpSize-1], IRT_EQ, note); // fix the last note as a note not in harmonic triad intervals
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H3_3v_2sp..." << endl;
}

void FuxTest::test_1H3_4v_2sp() {
    cout << "Start test 1H3_4v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    int triad_cons[] = {0, 3, 4, 7}; // harmonic triad intervals
    int non_triad_cons[] = {1, 2, 5, 6, 8, 9, 10, 11}; // non-harmonic triad intervals
    spList = {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    v_type = {0, 0};
    // non-perfect consonant intervals are forbidden
    for (size_t i = 0; i < 3; i++) { // iterate over each solution line
        for (int interval : non_triad_cons) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(cpSize*i)+cpSize-1], IRT_EQ, note); // fix the last note as a note not in harmonic triad intervals
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_4v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H3_4v_2sp..." << endl;
}

void FuxTest::test_1H3_2v_3sp() {
    cout << "Start test 1H3_2v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    int p_cons[] = {0, 7}; // perfect conssonant intervals
    int non_p_cons[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11}; // non-perfect conssonant intervals
    spList = {THIRD_SPECIES};
    v_type = {0};
    // non-perfect consonant intervals are forbidden
    for (int interval : non_p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[cfSize-1] + interval;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note with a not perfect consonance
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H3_2v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last harmonic interval must be a perfect consonance" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H3_2v_3sp..." << endl;
}

void FuxTest::test_1H3_3v_3sp() {
    cout << "Start test 1H3_3v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    int triad_cons[] = {0, 3, 4, 7}; // harmonic triad intervals
    int non_triad_cons[] = {1, 2, 5, 6, 8, 9, 10, 11}; // non-harmonic triad intervals
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {0, 0};
    // non-perfect consonant intervals are forbidden
    for (size_t i = 0; i < 2; i++) { // iterate over each solution line
        for (int interval : non_triad_cons) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(cpSize*i)+cpSize-1], IRT_EQ, note); // fix the last note as a note not in harmonic triad intervals
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H3_3v_3sp..." << endl;
}

void FuxTest::test_1H3_4v_3sp() {
    cout << "Start test 1H3_4v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    int triad_cons[] = {0, 3, 4, 7}; // harmonic triad intervals
    int non_triad_cons[] = {1, 2, 5, 6, 8, 9, 10, 11}; // non-harmonic triad intervals
    spList = {THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    v_type = {0, 0, 0};
    // non-perfect consonant intervals are forbidden
    for (size_t i = 0; i < 3; i++) { // iterate over each solution line
        for (int interval : non_triad_cons) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(cpSize*i)+cpSize-1], IRT_EQ, note); // fix the last note as a note not in harmonic triad intervals
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_4v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H3_4v_3sp..." << endl;
}

void FuxTest::test_1H3_2v_4sp() {
    cout << "Start test 1H3_2v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    int p_cons[] = {0, 7}; // perfect conssonant intervals
    int non_p_cons[] = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11}; // non-perfect conssonant intervals
    spList = {FOURTH_SPECIES};
    v_type = {0};
    // non-perfect consonant intervals are forbidden
    for (int interval : non_p_cons) {
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[cfSize-1] + interval;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note with a not perfect consonance
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H3_2v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last harmonic interval must be a perfect consonance" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H3_2v_4sp..." << endl;
}

void FuxTest::test_1H3_3v_4sp() {
    cout << "Start test 1H3_3v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    int triad_cons[] = {0, 3, 4, 7}; // harmonic triad intervals
    int non_triad_cons[] = {1, 2, 5, 6, 8, 9, 10, 11}; // non-harmonic triad intervals
    spList = {FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {0, 0};
    // non-perfect consonant intervals are forbidden
    for (size_t i = 0; i < 2; i++) { // iterate over each solution line
        for (int interval : non_triad_cons) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(cpSize*i)+cpSize-1], IRT_EQ, note); // fix the last note as a note not in harmonic triad intervals
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H3_3v_4sp..." << endl;
}

void FuxTest::test_1H3_4v_4sp() {
    cout << "Start test 1H3_4v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    int triad_cons[] = {0, 3, 4, 7}; // harmonic triad intervals
    int non_triad_cons[] = {1, 2, 5, 6, 8, 9, 10, 11}; // non-harmonic triad intervals
    spList = {FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {0, 0, 0};
    // non-perfect consonant intervals are forbidden
    for (size_t i = 0; i < 3; i++) { // iterate over each solution line
        for (int interval : non_triad_cons) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(cpSize*i)+cpSize-1], IRT_EQ, note); // fix the last note as a note not in harmonic triad intervals
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H3_4v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last harmonic intervals must be in harmonic triad" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H3_4v_4sp..." << endl;
}

/*The key tone is tuned according to the first note of the cantus firmus. 
In other words, the lowest stratum at the first and last notes must be the tonic.*/
void FuxTest::test_1H4() {
    cout << "Start tests 1H4..." << endl;
    test_1H4_2v_1sp();
    test_1H4_3v_1sp();
    test_1H4_4v_1sp();
    test_1H4_2v_2sp();
    test_1H4_3v_2sp();
    test_1H4_4v_2sp();
    test_1H4_2v_3sp();
    test_1H4_3v_3sp();
    test_1H4_4v_3sp();
    test_1H4_2v_4sp();
    test_1H4_3v_4sp();
    test_1H4_4v_4sp();
    cout << "End tests 1H4." << endl;
}

void FuxTest::test_1H4_2v_1sp() {
    cout << "Start test 1H4_2v_1sp..." << endl;
    spList = {FIRST_SPECIES};
    v_type = {-2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_2v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cfSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_2v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_2v_1sp..." << endl;
}

void FuxTest::test_1H4_3v_1sp() {
    cout << "Start test 1H4_3v_1sp..." << endl;
    // Lowest stratum is the first solution array
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {-2, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cfSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the second solution array
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, -2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cfSize], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[(cfSize*2) -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_3v_1sp." << endl;
}

void FuxTest::test_1H4_4v_1sp() {
    cout << "Start test 1H4_4v_1sp..." << endl;
    // Lowest stratum is the first solution array
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    v_type = {-2, 3, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cfSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the second solution array
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, -2, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cfSize], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[(cfSize*2) -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the third solution array
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, 3, -2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cfSize*2], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[(cfSize*3) -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_4v_1sp." << endl;
}

void FuxTest::test_1H4_2v_2sp() {
    cout << "Start test 1H4_2v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    spList = {SECOND_SPECIES};
    v_type = {-2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_2v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_2v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_2v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_2v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_2v_2sp..." << endl;
}

void FuxTest::test_1H4_3v_2sp() {
    cout << "Start test 1H4_3v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    // Lowest stratum is the first solution array
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {-2, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the second solution array
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, -2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_3v_2sp." << endl;
}

void FuxTest::test_1H4_4v_2sp() {
    cout << "Start test 1H4_4v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    // Lowest stratum is the first solution array
    spList = {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    v_type = {-2, 3, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the second solution array
    spList = {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, -2, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2 -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the third solution array
    spList = {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, 3, -2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[(cpSize*3) -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_4v_2sp." << endl;
}

void FuxTest::test_1H4_2v_3sp() {
    cout << "Start test 1H4_2v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    spList = {THIRD_SPECIES};
    v_type = {-2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_2v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_2v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_2v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_2v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_2v_3sp..." << endl;
}

void FuxTest::test_1H4_3v_3sp() {
    cout << "Start test 1H4_3v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    // Lowest stratum is the first solution array
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {-2, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the second solution array
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {3, -2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_3v_3sp." << endl;
}

void FuxTest::test_1H4_4v_3sp() {
    cout << "Start test 1H4_4v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    // Lowest stratum is the first solution array
    spList = {THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    v_type = {-2, 3, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the second solution array
    spList = {THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    v_type = {3, -2, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2 -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the third solution array
    spList = {THIRD_SPECIES, THIRD_SPECIES, THIRD_SPECIES};
    v_type = {3, 3, -2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[(cpSize*3) -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_4v_3sp." << endl;
}

void FuxTest::test_1H4_2v_4sp() {
    cout << "Start test 1H4_2v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    spList = {FOURTH_SPECIES};
    v_type = {-2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_2v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_2v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_2v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_2v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_2v_4sp..." << endl;
}

void FuxTest::test_1H4_3v_4sp() {
    cout << "Start test 1H4_3v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    // Lowest stratum is the first solution array
    spList = {FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {-2, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the second solution array
    spList = {FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {3, -2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_3v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_3v_4sp." << endl;
}

void FuxTest::test_1H4_4v_4sp() {
    cout << "Start test 1H4_4v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    // Lowest stratum is the first solution array
    spList = {FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {-2, 3, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize-1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the second solution array
    spList = {FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {3, -2, 3};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2 -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    // Lowest stratum is the third solution array
    spList = {FOURTH_SPECIES, FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {3, 3, -2};
    for (int i = -18; i < 1; i++) { //iterate over every note that can take solution array
        // fix the first note of the lowest stratum 
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        int note = cantusFirmus[0] + i;
        rel(problem->getHome(), problem->getSolutionArray()[cpSize*2], IRT_EQ, note); // fix the first note of the lowest stratum
        if (note % 12 == cantusFirmus[0] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> First note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[0] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> First note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
        // fix the last note of the lowest stratum 
        problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        note = cantusFirmus[cfSize-1] + i;
        rel(problem->getHome(), problem->getSolutionArray()[(cpSize*3) -1], IRT_EQ, note); // fix the last note of the lowest stratum
        if (note % 12 == cantusFirmus[cfSize-1] % 12 && !has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "Solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Last note of lowest stratum is the tonic and must be" << std::endl;
        } else if (note % 12 != cantusFirmus[cfSize-1] % 12 && has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H4_4v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Last note of lowest stratum is not the tonic" << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H4_4v_4sp." << endl;
}

/*
The voices cannot play the same note at the same time except in the first and last measure.
*/
void FuxTest::test_1H5() {
    cout << "Start tests 1H5..." << endl;
    test_1H5_2v_1sp();
    test_1H5_3v_1sp();
    test_1H5_2v_2sp();
    test_1H5_3v_2sp();
    test_1H5_2v_3sp();
    test_1H5_3v_3sp();
    test_1H5_2v_4sp();
    test_1H5_3v_4sp();
    cout << "End tests 1H5." << endl;
}

void FuxTest::test_1H5_2v_1sp() {
    cout << "Start test_1H5_2v_1sp ..." << endl;
    spList = {FIRST_SPECIES};
    v_type = {0};
    for (size_t i = 1; i < cfSize-1; i++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H5_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Same note played at the mesure: " << i << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H5_2v_1sp" << endl;
}

void FuxTest::test_1H5_3v_1sp() {
    cout << "Start test 1H5_3v_1sp..." << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {0, 0};
    // // test solution lines != cf
    for (size_t j = 0; j < 2; j++) { // iterate over each solution line
        for (size_t i = 1; i < cfSize-1; i++) {
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[(j*cfSize) + i], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H5_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Same note played at the mesure: " << i << std::endl;
            }
            delete problem;
        }
    }
    // test notes of solutions lines are different
    for (size_t i = 1; i < cfSize-1; i++) {
        for (int j = -6; j < 13; j++) {
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[0] + j;
            rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, note); // fix the note of the first solution line as the same note as the note of the second solution line 
            rel(problem->getHome(), problem->getSolutionArray()[cfSize+i], IRT_EQ, note); // fix the note of the first solution line as the same note as the note of the second solution line 
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H5_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Same note played at the mesure: " << i << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H5_3v_1sp." << endl;
}

void FuxTest::test_1H5_2v_2sp() {
    cout << "Start test 1H5_2v_2sp ..." << endl;
    spList = {SECOND_SPECIES};
    v_type = {0};
    for (size_t i = 1; i < cfSize-1; i++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[i*2], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H5_2v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Same note played at the mesure: " << i << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H5_2v_2sp" << endl;
}

void FuxTest::test_1H5_3v_2sp() {
    cout << "Start test 1H5_3v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {0, 0};
    // // test solution lines != cf
    for (size_t j = 0; j < 2; j++) { // iterate over each solution line
        for (size_t i = 1; i < cfSize-1; i++) { // iterate over each note of the cantus firmus
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize) + i*2], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H5_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Same note played at the mesure: " << i << std::endl;
            }
            delete problem;
        }
    }
    // test notes of solutions lines are different
    for (size_t i = 1; i < cfSize-1; i++) {
        for (int j = -6; j < 13; j++) { // iterate over each possible note of solution lines
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[0] + j;
            rel(problem->getHome(), problem->getSolutionArray()[i*2], IRT_EQ, note); // fix the note of the first solution line as the same note as the note of the second solution line 
            rel(problem->getHome(), problem->getSolutionArray()[cpSize+i*2], IRT_EQ, note); // fix the note of the first solution line as the same note as the note of the second solution line 
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H5_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Same note played at the mesure: " << i << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H5_3v_2sp." << endl;
}

void FuxTest::test_1H5_2v_3sp() {
    cout << "Start test 1H5_2v_3sp ..." << endl;
    spList = {THIRD_SPECIES};
    v_type = {0};
    for (size_t i = 1; i < cfSize-1; i++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[i*4], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H5_2v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Same note played at the mesure: " << i << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H5_2v_3sp" << endl;
}

void FuxTest::test_1H5_3v_3sp() {
    cout << "Start test 1H5_3v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {0, 0};
    // // test solution lines != cf
    for (size_t j = 0; j < 2; j++) { // iterate over each solution line
        for (size_t i = 1; i < cfSize-1; i++) { // iterate over each note of the cantus firmus
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize) + i*4], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H5_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Same note played at the mesure: " << i << std::endl;
            }
            delete problem;
        }
    }
    // test notes of solutions lines are different
    for (size_t i = 1; i < cfSize-1; i++) {
        for (int j = -6; j < 13; j++) { // iterate over each possible note of solution lines
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[0] + j;
            rel(problem->getHome(), problem->getSolutionArray()[i*4], IRT_EQ, note); // fix the note of the first solution line as the same note as the note of the second solution line 
            rel(problem->getHome(), problem->getSolutionArray()[cpSize+i*4], IRT_EQ, note); // fix the note of the first solution line as the same note as the note of the second solution line 
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H5_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Same note played at the mesure: " << i << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H5_3v_3sp." << endl;
}

void FuxTest::test_1H5_2v_4sp() {
    cout << "Start test 1H5_2v_4sp ..." << endl;
    spList = {FOURTH_SPECIES};
    v_type = {0};
    for (size_t i = 1; i < cfSize-1; i++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[i*2-1], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H5_2v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Same note played at the mesure: " << i << std::endl;
        }
        delete problem;
    }
    cout << "End test 1H5_2v_4sp" << endl;
}

void FuxTest::test_1H5_3v_4sp() {
    cout << "Start test 1H5_3v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    spList = {FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {0, 0};
    // // test solution lines != cf
    for (size_t j = 0; j < 2; j++) { // iterate over each solution line
        for (size_t i = 1; i < cfSize-1; i++) { // iterate over each note of the cantus firmus
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[(j*cpSize) + (i*2)-2], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H5_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Same note played at the mesure: " << i << std::endl;
            }
            delete problem;
        }
    }
    // test notes of solutions lines are different
    for (size_t i = 1; i < cfSize-1; i++) {
        for (int j = -6; j < 13; j++) { // iterate over each possible note of solution lines
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[0] + j;
            rel(problem->getHome(), problem->getSolutionArray()[i*2-2], IRT_EQ, note); // fix the note of the first solution line as the same note as the note of the second solution line 
            rel(problem->getHome(), problem->getSolutionArray()[cpSize+i*2-2], IRT_EQ, note); // fix the note of the first solution line as the same note as the note of the second solution line 
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H5_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Same note played at the mesure: " << i << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H5_3v_4sp." << endl;
}

// /*
// For thesis notes, imperfect consonances are preferred to perfect consonances, and fifths are preferred to octaves.
// */
// void FuxTest::test_1H6() {
//     cout << "Start test 1H6..." << endl;
//     test_1H6_2v_1sp();
//     cout << "End test 1H6." << endl;
// }

// void FuxTest::test_1H6_2v_1sp() {
//     cout << "test_1H6_2v_1sp" << endl; 
//     spList = {FIRST_SPECIES};
//     v_type = {0};
//     auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
//     rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
//     rel(problem->getHome(), problem->getSolutionArray()[1], IRT_EQ, 54);
//     std::vector<CounterpointProblem*> solutions = get_all_solutions(problem);
//     for (CounterpointProblem* solution : solutions) {
//         cout << ">> Solution: " << solution->getSolutionArray() << endl;
//         cout << ">> Costs: " << solution->cost() << endl;
//     }
//     delete problem;
// }


/*
In two voice composition, the harmonic interval of the penultimate note must be
a major sixth or a minor third depending on whether or not the cantus firmus is the lowest stratum.
In three voice composition, that harmonic interval must be either a minor third, a perfect fifth, a major sixth or an octave.
*/
void FuxTest::test_1H7() {
    cout << "Start test 1H7..." << endl;
    test_1H7_2v_1sp();
    test_1H7_3v_1sp();
    test_1H7_2v_2sp();
    test_1H7_3v_2sp();
    test_1H7_2v_3sp();
    test_1H7_3v_3sp();
    cout << "End test 1H7." << endl;
}

void FuxTest::test_1H7_2v_1sp() {
    cout << "Start test 1H7_2v_1sp..." << endl;
    // Test with cf as the lowest stratum
    spList = {FIRST_SPECIES};
    v_type = {3};
    for (size_t interval = 0; interval < 12; interval++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        if (interval == 9) { // correct interval
            rel(problem->getHome(), problem->getSolutionArray()[cfSize-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as the major sixth with the cf
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The penultimate note should be correct" << std::endl;
            }
        } else {
            rel(problem->getHome(), problem->getSolutionArray()[cfSize-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be a major sixth" << std::endl;
            }
        }
        delete problem;
    }
    // Test with cf as the highest stratum
    v_type = {-2};
    for (size_t interval = 0; interval < 12; interval++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        if (interval == 3) { // correct interval
            rel(problem->getHome(), problem->getSolutionArray()[cfSize-2], IRT_EQ, cantusFirmus[cfSize-2]-interval); // fix the penultimate solution note as the major sixth with the cf
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_1sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The penultimate note should be correct" << std::endl;
            }
        } else {
            rel(problem->getHome(), problem->getSolutionArray()[cfSize-2], IRT_EQ, cantusFirmus[cfSize-2]-interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be a minor third" << std::endl;
            }
        }
        delete problem;
    }
    cout << "End test 1H7_2v_1sp." << endl;
}

void FuxTest::test_1H7_3v_1sp() {
    cout << "Start test 1H7_3v_1sp..." << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, 3};
    vector<int> intervals = {1, 2, 4, 5, 6, 8, 10, 11}; // forbidden intervals
    for (size_t i = 1; i < 3; i++) {
        for (int interval : intervals) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), problem->getSolutionArray()[cfSize*i-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as a wrong interval with the cf
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1H7_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                    << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> The harmonic interval of the penultimate note must be either a minor third, a perfect fifth, a major sixth or an octave. " << std::endl;
        }
        delete problem;
        }
    }
    cout << "End test 1H7_3v_1sp." << endl;
}

void FuxTest::test_1H7_2v_2sp() {
    cout << "Start test 1H7_2v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    // Test with cf as the lowest stratum
    spList = {SECOND_SPECIES};
    v_type = {3};
    for (size_t interval = 0; interval < 12; interval++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        if (interval == 9) { // correct interval
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as the major sixth with the cf
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The penultimate note should be correct" << std::endl;
            }
        } else {
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be a major sixth" << std::endl;
            }
        }
        delete problem;
    }
    // Test with cf as the highest stratum
    v_type = {-2};
    for (size_t interval = 0; interval < 12; interval++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        if (interval == 3) { // correct interval
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]-interval); // fix the penultimate solution note as the major sixth with the cf
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_2sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The penultimate note should be correct" << std::endl;
            }
        } else {
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]-interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be a minor third" << std::endl;
            }
        }
        delete problem;
    }
    cout << "End test 1H7_2v_2sp." << endl;
}

void FuxTest::test_1H7_3v_2sp() {
    cout << "Start test 1H7_3v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, 3};
    vector<int> intervals = {1, 2, 4, 5, 6, 8, 10, 11}; // forbidden intervals
    for (size_t i = 1; i < 3; i++) {
        for (int interval : intervals) {
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[cpSize*i-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be either a minor third, a perfect fifth, a major sixth or an octave. " << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H7_3v_2sp." << endl;
}

void FuxTest::test_1H7_2v_3sp() {
    cout << "Start test 1H7_2v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    // Test with cf as the lowest stratum
    spList = {THIRD_SPECIES};
    v_type = {3};
    for (size_t interval = 0; interval < 12; interval++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        if (interval == 9) { // correct interval
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as the major sixth with the cf
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The penultimate note should be correct" << std::endl;
            }
        } else {
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be a major sixth" << std::endl;
            }
        }
        delete problem;
    }
    // Test with cf as the highest stratum
    v_type = {-2};
    for (size_t interval = 0; interval < 12; interval++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        if (interval == 3) { // correct interval
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]-interval); // fix the penultimate solution note as the major sixth with the cf
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_3sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The penultimate note should be correct" << std::endl;
            }
        } else {
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]-interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be a minor third" << std::endl;
            }
        }
        delete problem;
    }
    cout << "End test 1H7_2v_3sp." << endl;
}

void FuxTest::test_1H7_3v_3sp() {
    cout << "Start test 1H7_3v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {3, 3};
    vector<int> intervals = {1, 2, 4, 5, 6, 8, 10, 11}; // forbidden intervals
    for (size_t i = 1; i < 3; i++) {
        for (int interval : intervals) {
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[cpSize*i-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be either a minor third, a perfect fifth, a major sixth or an octave. " << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H7_3v_3sp." << endl;
}

void FuxTest::test_1H7_2v_4sp() {
    cout << "Start test 1H7_2v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    // Test with cf as the lowest stratum
    spList = {FOURTH_SPECIES};
    v_type = {3};
    for (size_t interval = 0; interval < 12; interval++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        if (interval == 9) { // correct interval
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as the major sixth with the cf
            if (!has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_4sp: It doesn't exist a solution but it should with the following configuration:\n"
                        << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The penultimate note should be correct" << std::endl;
            }
        } else {
            rel(problem->getHome(), problem->getSolutionArray()[cpSize-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_2v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be a major sixth" << std::endl;
            }
        }
        delete problem;
    }
    cout << "End test 1H7_2v_4sp." << endl;
}

void FuxTest::test_1H7_3v_4sp() {
    cout << "Start test 1H7_3v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {3, 3};
    vector<int> intervals = {1, 2, 4, 5, 6, 8, 10, 11}; // forbidden intervals
    for (size_t i = 1; i < 3; i++) {
        for (int interval : intervals) {
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            rel(problem->getHome(), problem->getSolutionArray()[cpSize*i-2], IRT_EQ, cantusFirmus[cfSize-2]+12+interval); // fix the penultimate solution note as a wrong interval with the cf
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H7_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> The harmonic interval of the penultimate note must be either a minor third, a perfect fifth, a major sixth or an octave. " << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test 1H7_3v_4sp." << endl;
}

// In three voice composition, tenths are prohibited in the last chord.
void FuxTest::test_1H10(){
    cout << "Start test 1H10..." << endl;
    test_1H10_3v_1sp();
    test_1H10_3v_2sp();
    test_1H10_3v_3sp();
    test_1H10_3v_4sp();
    cout << "End test 1H10." << endl;
}

void FuxTest::test_1H10_3v_1sp(){
    cout << "Start test_1H10_3v_1sp..." << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {3, 3};
    int intervals[] = {3, 4}; // forbidden intervals
    for (size_t i = 1; i < 3; i++) { // iterate over each counter point
        for (int interval: intervals) { // interval tested
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + 12 + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(i*cfSize)-1], IRT_EQ, note); // fix the last note as a thens
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H10_3v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> A thenth is played in the last chord" << std::endl;
            }
            delete problem;

        }
    }
    cout << "End test 1H10_3v_1sp" << endl;
}

void FuxTest::test_1H10_3v_2sp() {
    cout << "Start test_1H10_3v_2sp..." << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, 3};
    int intervals[] = {3, 4}; // forbidden intervals
    for (size_t i = 1; i < 3; i++) { // iterate over each counter point
        for (int interval: intervals) { // interval tested
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + 12 + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(i*cpSize)-1], IRT_EQ, note); // fix the last note as a thens
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H10_3v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> A thenth is played in the last chord" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test_1H10_3v_2sp" << endl;
}

void FuxTest::test_1H10_3v_3sp() {
    cout << "Start test_1H10_3v_3sp..." << endl;
    int cpSize = cfSize*4-3; // size of a counterpoint
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {3, 3};
    int intervals[] = {3, 4}; // forbidden intervals
    for (size_t i = 1; i < 3; i++) { // iterate over each counter point
        for (int interval: intervals) { // interval tested
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + 12 + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(i*cpSize)-1], IRT_EQ, note); // fix the last note as a thens
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H10_3v_3sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> A thenth is played in the last chord" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test_1H10_3v_3sp" << endl;
}

void FuxTest::test_1H10_3v_4sp() {
    cout << "Start test_1H10_3v_4sp..." << endl;
    int cpSize = cfSize*2-2; // size of a counterpoint
    spList = {FOURTH_SPECIES, FOURTH_SPECIES};
    v_type = {3, 3};
    int intervals[] = {3, 4}; // forbidden intervals
    for (size_t i = 1; i < 3; i++) { // iterate over each counter point
        for (int interval: intervals) { // interval tested
            auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[cfSize-1] + 12 + interval;
            rel(problem->getHome(), problem->getSolutionArray()[(i*cpSize)-1], IRT_EQ, note); // fix the last note as a thens
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 1H10_3v_4sp: It exists solution but it shouldn't with the following configuration:\n"
                            << "cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> A thenth is played in the last chord" << std::endl;
            }
            delete problem;
        }
    }
    cout << "End test_1H10_3v_4sp" << endl;
}

// Melodic intervals cannot exceed a minor sixth interval, but octave leaps are allowed in three and four voices.
void FuxTest::test_1M2() {
    cout << "Start test 1M2..." << endl;
    test_1M2_2v_1sp();
    test_1M2_3v_1sp();
    cout << "End test 1M2." << endl;
}

void FuxTest::test_1M2_2v_1sp() {
    cout << "Start test 1M2_2v_1sp" << endl;
    spList = {FIRST_SPECIES};
    v_type = {0};
    for (size_t i = 1; i < cfSize; i++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        rel(problem->getHome(), expr(problem->getHome(), abs(problem->getSolutionArray()[i] - problem->getSolutionArray()[i-1])), IRT_GR, 8); // fix the melodic interval between note i and i-1 greater than 8 
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1M2_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Same note played at the mesure: " << i << std::endl;
        }
        delete problem;
    }
    cout << "End test 1M2_2v_1sp" << endl;
}

void FuxTest::test_1M2_3v_1sp() {
    cout << "Start test 1M2_3v_1sp..." << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {2, 2};
    // Add the constraint that the interval between note i and i-1 is greater than 8 and different from 12
    for (size_t i = 1; i < cfSize; i++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        cout << "Add constraint" << endl;
        IntVar absDiff = expr(problem->getHome(), abs(problem->getSolutionArray()[i] - problem->getSolutionArray()[i-1]));
        rel(problem->getHome(), absDiff, IRT_GR, 8);
        rel(problem->getHome(), absDiff, IRT_NQ, 12);
        cout << "Constraint added" << endl;
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 1M2_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "> Same note played at the mesure: " << i << std::endl;
        }
        delete problem;
    }
    cout << "End test 1M2_3v_1sp" << endl;
}

// void FuxTest::test_1P1_2v_1sp() {
//     cout << "Start test 1P1_2v_1sp..." << endl;
//     spList = {FIRST_SPECIES};
//     v_type = {0};
//     int cons_intervals[] = {0, 3, 4, 7, 8, 9}; // consonant intervals
//     for (size_t i = 1; i < cfSize; i++) {
//         auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
//         // Add the constraint that the note at index i is in cons_intervals
//         Gecode::IntSet consSet(cons_intervals, sizeof(cons_intervals) / sizeof(cons_intervals[0]));
//         dom(problem->getHome(), problem->getSolutionArray()[i], consSet);

//         BoolVar bothUp = expr(problem->getHome(), part1->getMelodicIntervals()[i] > 0 && part2->getMelodicIntervals()[i] > 0);
//         BoolVar bothDown = expr(home, part1->getMelodicIntervals()[i] < 0 && part2->getMelodicIntervals()[i] < 0);
//         BoolVar sameDirection = expr(home, bothUp || bothDown);

//         // Constrain the motion to be direct
//         rel(home, sameDirection == 1);

//         rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, cantusFirmus[i]); // fix the solution note as the same note as the cf
//         if (has_solution(problem)) {
//             std::cerr   << "/!\\ ERROR test 1P1_2v_1sp: It exists solution but it shouldn't with the following configuration:\n"
//                         << "cantus firmus: ";
//             printVector(cantusFirmus);
//             std::cerr   << "solution array: ";
//             printIntVarArray(problem->getSolutionArray());
//             std::cerr   << "> Same note played at the mesure: " << i << std::endl;
//         }
//         problem->getSolutionArray().
//         delete problem;
//     }
//     cout << "End test 1P1_2v_1sp" << endl;
// }


void FuxTest::test_2H2() {
    cout << "Start test 2H2..." << endl;
    test_2H2_2v_2sp();
    test_2H2_3v_2sp();
    test_2H2_4v_2sp();
    cout << "End test 2H2." << endl;
}

void FuxTest::test_2H2_2v_2sp() {
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    int cons[] = {3, 4, 7, 8, 9}; // conssonant intervals without 0 because 1H5
    spList = {SECOND_SPECIES};
    v_type = {3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {  
        //test first note
        CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[0] + 12 + interval; // note is dissonant
            rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, note); // fix the first note
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 2H2_2v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << 0 << std::endl;
            }
            delete problem;      
        // test next notes
        for (size_t i = 1; i < cfSize; i++) {
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[i] + 12 + interval; // note is dissonant
            rel(problem->getHome(), problem->getSolutionArray()[i*2], IRT_EQ, note); // fix the note i
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 2H2_2v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << i << std::endl;
            }
            delete problem;
        }  
    }
}

void FuxTest::test_2H2_3v_2sp() {
    cout << "Start test 2H2_3v_2sp..." << endl;
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    int cons[] = {3, 4, 7, 8, 9}; // consonant intervals without 0 because 1H5
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t j = 0; j < 2; j++) { // iterate over each solution line
            //test first note
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[0] + 12 + interval; // note is dissonant
            rel(problem->getHome(), problem->getSolutionArray()[j*((cfSize*2)-1)], IRT_EQ, note); // fix the first note
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 2H2_3v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << 0 << std::endl;
            }
            delete problem;      
            // test next notes
            for (size_t i = 0; i < cfSize; i++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[j*cfSize+i], IRT_EQ, note); // fix the note i
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 2H2_3v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the measure " << i << std::endl;
                }
                delete problem;
            }  
        }
    }
}

void FuxTest::test_2H2_4v_2sp() {
    cout << "Start test 2H2_4v_2sp..." << endl;
    int dis[] = {1, 2, 5, 6, 10, 11}; // dissonant intervals
    int cons[] = {3, 4, 7, 8, 9}; // consonant intervals without 0 because 1H5
    spList = {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    v_type = {3, 3, 3}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}

    // Test that dissonant notes are forbidden
    for (int interval : dis) {
        for (size_t j = 0; j < 3; j++) { // iterate over each solution line
            //test first note
            CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
            int note = cantusFirmus[0] + 12 + interval; // note is dissonant
            rel(problem->getHome(), problem->getSolutionArray()[j*((cfSize*2)-1)], IRT_EQ, note); // fix the first note
            if (has_solution(problem)) {
                std::cerr   << "/!\\ ERROR test 2H2_4v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                            << "Cantus firmus: ";
                printVector(cantusFirmus);
                std::cerr   << "Solution array: ";
                printIntVarArray(problem->getSolutionArray());
                std::cerr   << "> Dissonant harmonic at the mesure " << 0 << std::endl;
            }
            delete problem;      
            // test next notes
            for (size_t i = 0; i < cfSize; i++) {
                CounterpointProblem* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                int note = cantusFirmus[i] + 12 + interval; // note is dissonant
                rel(problem->getHome(), problem->getSolutionArray()[j*cfSize+i], IRT_EQ, note); // fix the note i
                if (has_solution(problem)) {
                    std::cerr   << "/!\\ ERROR test 2H2_4v_2sp: It exists a solution but it shouldn't with the following configuration:\n"
                                << "Cantus firmus: ";
                    printVector(cantusFirmus);
                    std::cerr   << "Solution array: ";
                    printIntVarArray(problem->getSolutionArray());
                    std::cerr   << "> Dissonant harmonic at the measure " << i << std::endl;
                }
                delete problem;
            }  
        }
    }
}

void FuxTest::test_2M2() {
    cout << "Start test 2M2..." << endl;
    test_2M2_2v_2sp();
    cout << "End test 2M2." << endl;
}

void FuxTest::test_2M2_2v_2sp() {
    cout << "Start test_2M2_2v_2sp" << endl;
    int cpSize = cfSize*2-1; // size of a counterpoint
    spList = {SECOND_SPECIES};
    v_type = {1}; // {(6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0]}
    for (size_t i = 1; i < cpSize; i++) {
        auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
        cout << "1" << endl;
        rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, problem->getSolutionArray()[i-1]); // fix the note i a the same than i-1
        cout << "2" << endl;
        if (has_solution(problem)) {
            std::cerr   << "/!\\ ERROR test 2M2_2v_2sp: It exists solution but it shouldn't with the following configuration:\n"
                        << "cantus firmus: ";
            printVector(cantusFirmus);
            std::cerr   << "solution array: ";
            printIntVarArray(problem->getSolutionArray());
            std::cerr   << "Same note is played consecutively" << std::endl;
        }
        cout << "3" << endl;
        delete problem;
        cout << "4" << endl;
    }
    cout << "End test 2M2_2v_2sp" << endl;
}

// ----- Figures tests -----

void FuxTest::test_2v_1sp_fig22() {
    cout << "Start test_2v_1sp_fig22" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,64,67,65,64,72,69,71,71,69,68,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_1sp_fig22." << endl;
}

void FuxTest::test_2v_1sp_fig23(){
    cout << "Start test_2v_1sp_fig23" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,69,67,65,64,64,62,60,67,69,68,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_1sp_fig23." << endl;
}

void FuxTest::test_2v_2sp_fig38(){
    cout << "Start test_2v_2sp_fig38" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {-1,65,64,62,60,58,57,55,53,57,60,58,57,69,67,64,65,67,69,65,62,64,65};
    v_type = {0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig38." << endl;
}

void FuxTest::test_2v_2sp_fig39(){
    cout << "Start test_2v_2sp_fig39" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {-1,53,52,48,53,52,50,48,46,58,55,60,57,53,52,48,53,41,45,50,48,52,53};
    v_type = {-2};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig39." << endl;
}

void FuxTest::test_2v_2sp_fig40(){
    cout << "Start test_2v_2sp_fig40" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55}; 
    cp =           {-1, 67,64,65,67,69,71,69,67,72,71,72,74,72,71,69,67,65,64,72,71,69,67,62,64,66,67};
    v_type = {2};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig40." << endl;
}

void FuxTest::test_2v_2sp_fig41(){
    cout << "Start test_2v_2sp_fig41" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55}; 
    cp =           {-1,67,64,65,67,65,64,62,60,64,60,72,71,69,67,71,72,71,69,67,66,62,67,59,62,66,67};
    v_type = {2};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig41." << endl;
}

void FuxTest::test_2v_2sp_fig42(){
    cout << "Start test_2v_2sp_fig42" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,60,64,65,64,62,60,59,57}; 
    cp =           {-1,69,64,65,67,62,64,76,72,71,69,65,67,71,74,69,72,64,66,68,69};
    v_type = {2};   
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig42." << endl;
}

void FuxTest::test_2v_2sp_fig43(){
    cout << "Start test_2v_2sp_fig43" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {-1,45,57,52,55,52,50,53,57,59,60,48,50,45,48,52,53,55,57,45,52,56,57};
    v_type = {-1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig43." << endl;
}

void FuxTest::test_2v_2sp_fig44(){
    cout << "Start test_2v_2sp_fig44" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {60,64,65,67,64,69,67,64,65,64,62,60}; 
    cp =           {-1,67,72,71,69,74,71,69,67,71,72,74,76,74,72,71,69,71,72,67,69,71,72};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig44." << endl;
}

void FuxTest::test_2v_2sp_fig45(){
    cout << "Start test_2v_2sp_fig45" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {60,64,65,67,64,69,67,64,65,64,62,60}; 
    cp =           {-1,60,72,71,69,74,71,67,72,71,69,72,76,74,72,69,74,69,72,60,67,71,72};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig45." << endl;
}

void FuxTest::test_2v_3sp_fig55() {
    cout << "Start test_2v_3sp_fig55" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {62,64,65,67,69,71,72,74,76,74,71,72,74,72,70,69,70,72,74,76,77,65,69,71,72,69,70,72,70,69,67,71,69,62,64,65,67,69,71,73,74};
    v_type = {2};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_3sp_fig55." << endl;
}

void FuxTest::test_2v_3sp_fig56(){
    cout << "Start test_2v_3sp_fig56" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {62,64,65,67,69,62,69,71,72,71,67,69,71,69,67,65,64,76,71,72,74,69,62,64,65,67,69,71,72,74,76,72,74,69,62,74,73,69,71,72,74};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_3sp_fig56." << endl;
}

void FuxTest::test_2v_3sp_fig57(){
    cout << "Start test_2v_3sp_fig57" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {71,67,69,71,72,71,69,67,65,67,69,71,72,64,65,67,69,72,76,74,72,71,69,72,71,74,71,69,67,71,72,71,69,71,72,74,76};
    v_type = {2};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_3sp_fig57." << endl;
}

void FuxTest::test_2v_3sp_fig58(){
    cout << "Start test_2v_3sp_fig58" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64}; 
    cp =           {64,65,67,64,69,67,65,64,62,64,65,67,69,64,69,67,65,64,62,64,65,67,69,71,72,74,76,74,72,60,72,71,69,74,69,74,76};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_3sp_fig58." << endl;
}   

void FuxTest::test_2v_3sp_fig59() {
    cout << "Start test_2v_3sp_fig59" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {65,64,62,60,59,62,67,65,64,62,60,58,57,60,62,64,65,62,64,65,67,64,65,67,69,67,65,69,67,65,64,62,60,64,60,64,65,64,62,60,58,60,62,64,65};
    v_type = {-1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_3sp_fig59." << endl;
}


void FuxTest::test_2v_3sp_fig60() {
    cout << "Start test_2v_3sp_fig60" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {62,74,72,71,72,69,71,72,74,72,71,69,71,69,66,65,66,74,78,77,76,69,81,78,77,76,74,71,72,69,71,72,74,72,71,69,71,72,74,71,72,69,71,72,74};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_3sp_fig60." << endl;
}

void FuxTest::test_2v_4sp_fig74() {
    cout << "Start test_2v_4sp_fig74" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =           {62,62,74,74,72,72,71,71,76,76,74,74,77,77,76,76,74,74,73,74};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_4sp_fig74." << endl;
}

void FuxTest::test_2v_4sp_fig75() {
    cout << "Start test_2v_4sp_fig75" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {76,76,72,72,71,72,64,64,65,65,72,72,71,71,76,76,74,76};
    v_type = {2};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_4sp_fig75." << endl;
}

void FuxTest::test_2v_4sp_fig76() {
    cout << "Start test_2v_4sp_fig76" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {64,64,69,69,67,67,65,65,62,62,74,74,72,72,76,76,74,76};
    v_type = {0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_4sp_fig76." << endl;
}

void FuxTest::test_2v_4sp_fig77() {
    cout << "Start test_2v_4sp_fig77" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {65,65,64,64,60,60,65,65,69,69,67,67,65,65,64,64,69,69,65,65,64,65};
    v_type = {0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_4sp_fig77." << endl;
}

void FuxTest::test_2v_4sp_fig78() {
    cout << "Start test_2v_4sp_fig78" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =           {74,74,72,72,71,71,66,66,64,64,76,76,74,74,72,72,71,71,74,74,72,74};
    v_type = {0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_4sp_fig78." << endl;
}

void FuxTest::test_3v_1sp_fig108(){
    cout << "Start test_3v_1sp_fig108" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {67,64,65,69,72,72,76,72,69,68,
                    64,69,62,65,65,65,72,72,74,76};
    v_type = {1, 0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig108." << endl;
}

void FuxTest::test_3v_1sp_fig109(){
    cout << "Start test_3v_1sp_fig109" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {52,48,50,48,45,57,55,52,53,52};
    cp =           {67,64,65,64,64,64,67,67,62,64,
                    71,72,69,69,72,72,71,71,69,71};
    v_type = {2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig109." << endl;
}

void FuxTest::test_3v_1sp_fig110(){
    cout << "Start test_3v_1sp_fig110" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {60,60,60,62,58,59,57,69,65,62,64,65,
                    53,52,53,50,55,55,53,45,50,50,48,41};
    v_type = {-1, -3};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig110." << endl;
}

void FuxTest::test_3v_1sp_fig111(){
    cout << "Start test_3v_1sp_fig111" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {69,72,72,74,74,70,69,67,65,77,76,77,
                    77,76,77,74,70,67,65,64,65,62,60,65};
    v_type = {1, 2};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig111." << endl;
}

void FuxTest::test_3v_1sp_fig112(){
    cout << "Start test_3v_1sp_fig112" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {69,70,72,69,70,72,69,67,72,69,70,69,
                    65,62,60,60,65,67,60,64,64,65,64,65};
    v_type = {1, 0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig112." << endl;
}   

void FuxTest::test_3v_1sp_fig113(){
    cout << "Start test_3v_1sp_fig113" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55};
    cp =           {71,67,67,71,76,72,71,67,67,69,66,67,66,67,
                    55,52,52,52,48,48,55,52,48,45,47,43,50,43};
    v_type = {2, 0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig113." << endl;
}   

void FuxTest::test_3v_1sp_fig114(){
    cout << "Start test_3v_1sp_fig114" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55};
    cp =           {71,67,67,71,67,69,71,71,72,64,66,67,66,67,
                    55,52,52,52,52,48,55,52,48,48,47,43,50,43};
    v_type = {3, 0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig114." << endl;
}  

void FuxTest::test_3v_1sp_fig115(){
    cout << "Start test_3v_1sp_fig115" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {69,72,71,74,72,76,77,76,74,72,71,69};
    cp =           {60,64,67,65,64,67,69,67,65,69,68,69,
                    69,69,76,74,69,67,65,72,74,69,76,69};
    v_type = {-1, 0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig115." << endl;
}         

void FuxTest::test_3v_1sp_fig116(){
    cout << "Start test_3v_1sp_fig116" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57};
    cp =           {64,64,67,65,64,72,69,72,71,69,68,69,
                    45,45,52,50,57,57,50,48,55,57,52,45};
    v_type = {1, -1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig116." << endl;
}     

void FuxTest::test_3v_1sp_fig117(){
    cout << "Start test_3v_1sp_fig117" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {45,48,47,50,48,52,53,52,50,48,47,45};
    cp =           {60,64,62,65,64,60,60,60,62,57,56,57,
                    69,69,71,71,72,67,69,67,65,64,62,64};
    v_type = {2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig117." << endl;
}

void FuxTest::test_3v_1sp_fig118(){
    cout << "Start test test_3v_1sp_fig118" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {60, 64, 65, 67, 64, 69, 67, 64, 65, 64, 62, 60};
    cp =           {67, 72, 69, 67, 72, 72, 76, 72, 71, 72, 71, 72,
                    48, 48, 50, 52, 48, 53, 52, 57, 50, 48, 55, 48};
    v_type = {1, -1};
    borrowMode = 1;
   test_configuration();
    cout << "End test_3v_1sp_fig118." << endl;
}

void FuxTest::test_3v_1sp_fig119(){
    cout << "Start test 3v_1sp_fig119" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {60, 64, 65, 67, 64, 69, 67, 64, 65, 64, 62, 60};
    cp =           {64, 67, 69, 71, 72, 72, 76, 72, 69, 72, 71, 72,
                    72, 72, 69, 67, 69, 65, 72, 72, 74, 72, 67, 72};
    v_type = {1, 1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig119." << endl;
}


void FuxTest::test_3v_2sp_fig125(){
    cout << "Start test_3v_2sp_fig125" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus = {52,   48,   50,   48,   45,   57,   55,   52,   53,   52};
    cp =           {64,   64,   65,   64,   65,   64,   67,   67,   69,   68,
                    -1,71,72,71,69,65,67,64,69,71,72,69,71,74,76,74,72,69,71};
    v_type = {2 ,3};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_2sp_fig125." << endl;
}

void FuxTest::test_3v_2sp_fig128(){
    cout << "Start test_3v_2sp_fig128" << endl;
    spList = {SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,   67,   69,   65,   62,   64,   65,   72,   69,   65,   67,   65};
    cp =           {-1,77,76,74,72,71,69,72,70,69,67,72,69,81,79,76,72,69,74,77,77,76,77,
                    65,   60,   65,   65,   67,   72,   74,   76,   77,   74,   72,   65};
    v_type = {1 ,0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_2sp_fig128." << endl;
}

void FuxTest::test_3v_2sp_fig129(){
    cout << "Start test_3v_2sp_fig129" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus =  {65,   67,   69,   65,   62,   64,   65,   72,   69,   65,   67,   65}; //1sp 2v cf
    cp =            {60,   60,   60,   62,   65,   67,   69,   67,   60,   62,   64,   65,
                     -1,53,52,48,53,52,50,48,46,45,43,48,41,53,52,48,53,52,50,46,43,48,41};
    v_type = {-1, -3};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_2sp_fig129." << endl;
}

void FuxTest::test_3v_3sp_fig132(){
    cout << "Start test_3v_3sp_fig132" << endl;
    spList = {FIRST_SPECIES, THIRD_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; //1sp 2v cf
    cp =            {69,69,72,71,71,74,72,76,74,73,74,
                     -1,62,65,64,62,64,65,67,69,67,64,65,67,62,67,65,64,67,65,64,62,74,62,64,65,67,69,71,72,76,74,72,74,62,65,67,69,67,69,57,62};
    v_type = {1, 0};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_3sp_fig132." << endl;
}

void FuxTest::test_3v_3sp_fig133(){
    cout << "Start test_3v_3sp_fig133" << endl;
    spList = {THIRD_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; //1sp 2v cf
    cp =            {65,62,65,67,69,65,69,71,72,64,67,69,71,74,71,69,67,69,71,73,74,76,77,74,72,69,72,74,76,74,72,71,69,74,69,71,73,69,71,72,74,
                     50,50,48,55,52,50,53,48,50,57,50};
    v_type = {0, -3};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_3sp_fig133." << endl;
}

void FuxTest::test_3v_4sp_fig150(){
    cout << "Start test_3v_4sp_fig150" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =            {69,71,72,69,65,67,69,67,72,69,70,69,
                    65,65,64,64,60,60,57,57,62,62,60,60,65,65,64,64,60,60,65,65,64,65};
    v_type = {0, -3};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_4sp_fig150." << endl;
}

void FuxTest::test_3v_4sp_fig151(){
    cout << "Start test_3v_4sp_fig151" << endl;
    spList = {FIRST_SPECIES, FOURTH_SPECIES};
    cantusFirmus =  {65,67,69,65,62,64,65,72,69,65,67,65}; //1sp 2v cf
    cp =            {53,60,65,62,58,55,62,64,65,62,58,57,
                    53,53,52,52,50,50,46,46,43,-1,48,48,46,46,45,45,50,50,53,53,52,53};
    v_type = {0, -3};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_4sp_fig151." << endl;
}

void FuxTest::test_4v_1sp_fig171(){
    cout << "Start test_4v_1sp_fig171" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =            {69,67,65,65,65,67,65,64,65,65,64,65,
                     60,60,60,62,62,58,57,57,53,57,60,57,
                     53,52,53,50,46,43,50,45,50,50,48,41};
    v_type = {3, 2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test test_4v_1sp_fig171." << endl;
}

void FuxTest::test_4v_1sp_fig172(){
    cout << "Start test_4v_1sp_fig172" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {65,67,69,65,62,64,65,72,69,65,67,65}; 
    cp =            {72,71,72,69,69,72,72,72,72,69,70,65,
                     69,67,64,65,65,67,69,67,65,65,64,65,
                     65,62,60,60,62,60,60,64,60,62,58,60};
    v_type = {3, 2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test test_4v_1sp_fig172." << endl;
}

void FuxTest::test_4v_2sp_fig175(){
    cout << "Start test_4v_2sp_fig175" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =            {74,74,72,74,76,77,77,76,74,73,74,
                     69,69,69,71,71,74,72,72,69,69,69,
                     -1,74,62,65,69,72,67,65,64,67,62,64,65,69,72,60,62,65,69,57,62};
    v_type = {3, 2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test test_4v_2sp_fig175." << endl;
}


void FuxTest::test_4v_2sp_fig176(){
    cout << "Start test_4v_2sp_fig176" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {50,   53,   52,   50,   55,   53,   57,   55,   53,   52,   50}; 
    cp =            {65,   69,   72,   74,   70,   69,   69,   71,   74,   73,   74,
                     -1,62,60,62,64,67,65,64,62,64,65,69,64,65,67,62,65,69,67,64,66,
                     69,   69,   67,   69,   70,   72,   72,   74,   69,   76,   69};
    v_type = {3, 2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test test_4v_2sp_fig176." << endl;
}

void FuxTest::test_4v_3sp_fig184(){
    cout << "Start test_4v_3sp_fig184" << endl;
    spList = {FIRST_SPECIES, THIRD_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {76,72,74,72,69,81,79,76,77,76}; 
    cp =            {68,71,64,67,69,72,69,67,65,62,64,65,67,65,64,62,60,72,69,67,65,67,69,71,72,74,76,71,72,69,71,72,69,74,69,71,68,
                     59,57,57,55,57,60,64,60,62,59,
                     64,65,62,64,65,65,64,69,62,64};
    v_type = {3, 2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test test_4v_3sp_fig184." << endl;
}

void FuxTest::test_4v_3sp_fig186(){
    cout << "Start test_4v_3sp_fig186" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES, THIRD_SPECIES};
    cantusFirmus =  {64,60,62,60,57,69,67,64,65,64}; 
    cp =            {71,76,77,76,77,77,76,76,69,68,
                     68,69,69,72,72,72,72,72,74,71,
                     76,74,72,71,69,67,65,64,62,64,65,62,69,72,69,67,65,67,69,67,65,67,69,71,72,64,65,67,69,67,65,64,62,65,64,62,64};
    v_type = {3, 2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test test_4v_3sp_fig186." << endl;
}

void FuxTest::test_4v_4sp_fig193(){
    cout << "Start test_4v_4sp_fig193" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =            {69,69,74,74,72,72,71,71,74,74,69,69,77,77,76,76,74,74,73,74,
                     69,69,69,62,74,74,72,72,69,69,69,
                     50,50,45,47,43,50,53,48,50,45,50};
    v_type = {3, 2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test test_4v_4sp_fig193." << endl;
}

void FuxTest::test_4v_4sp_fig196(){
    cout << "Start test_4v_4sp_fig196" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus =  {62,65,64,62,67,65,69,67,65,64,62}; 
    cp =            {57,57,62,62,60,60,59,59,62,62,57,57,65,65,64,64,62,62,61,62,
                     65,69,69,62,74,74,72,72,69,69,69,
                     50,50,45,47,43,50,53,48,50,45,50};
    v_type = {3, 2, 3};
    borrowMode = 1;
    test_configuration();
    cout << "End test test_4v_4sp_fig196." << endl;
}



// ----- Util functions -----

void printVector(const std::vector<int>& array) {
    for (size_t i = 0; i < array.size(); ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

void printIntVarArray(const IntVarArray& array) {
    for (int i = 0; i < array.size(); ++i) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
}

std::vector<CounterpointProblem*> get_all_solutions(CounterpointProblem* problem) {
    std::vector<CounterpointProblem*> solutions;
    BAB<CounterpointProblem> e(problem);
    while (CounterpointProblem* pb = e.next()) {
        solutions.push_back(pb);
    }
    return solutions;
}

void FuxTest::test_configuration() {
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
    for (size_t i = 0; i < cp.size(); i++) { 
        int note = cp[i];
        if (note >  0) {
            rel(problem->getHome(), problem->getSolutionArray()[i], IRT_EQ, note);
        }
    }
    if (!has_solution(problem)) {
        std::cerr   << "/!\\ ERROR: It doesn't exist a solution but it should with the following configuration:" << endl;
    } else {
        cout << "\t Test passed with the following configuration:" << endl;
    }
    cout << "\t Cantus firmus: ";
    printVector(cantusFirmus);
    cout << "\t Solution array: ";
    printIntVarArray(problem->getSolutionArray());
    delete problem;
}

int FuxTest::get_motions(CounterpointProblem* problem, int i) {
    if (cp[i] > cp[i-1]) {
        if (problem->getSolutionArray()[i].val() > problem->getSolutionArray()[i-1].val()) {
            return 2;
        } else if (problem->getSolutionArray()[i].val() < problem->getSolutionArray()[i-1].val()) {
            return 0;
        } else {
            return 1;
        }
    }
    if (cp[i] < cp[i-1]) {
        if (problem->getSolutionArray()[i].val() > problem->getSolutionArray()[i-1].val()) {
            return 0;
        } else if (problem->getSolutionArray()[i].val() < problem->getSolutionArray()[i-1].val()) {
            return 2;
        } else {
            return 1;
        }
    }
    if (cp[i] == cp[i-1]) {
        if (problem->getSolutionArray()[i].val() == problem->getSolutionArray()[i-1].val()) {
            return 2;
        } else if (problem->getSolutionArray()[i].val() < problem->getSolutionArray()[i-1].val()) {
            return 0;
        } else {
            return 1;
        }
    }
    return -1;
}

// ===============================================

CounterpointProblem* FuxTest::dispatcher(char* test){
    if(strcmp(test, "1H1")==0){
        return test_1sp_H1();
    } else if(strcmp(test, "1H2")==0){
        return test_1sp_H2();
    } else if(strcmp(test, "1H32")==0){
        return test_1sp_H3();
    } else if(strcmp(test, "1H33")==0){
        return test_1sp_H3_2();
    } else if(strcmp(test, "1H41")==0){
        return test_1sp_H4_1();
    } else if(strcmp(test, "1H42")==0){
        return test_1sp_H4_2();
    } else if(strcmp(test, "1H5")==0){
        return test_1sp_H5();
    } else if(strcmp(test, "1H7")==0){
        return test_1sp_H7();
    } else if(strcmp(test, "1H72")==0){
        return test_1sp_H7_2();
    } else if(strcmp(test, "2H2")==0){
        return test_2sp_H2();
    } else if(strcmp(test, "3H1")==0){
        return test_3sp_H1();
    } else if(strcmp(test, "4H2")==0){
        return test_4sp_H2();
    } else {
        std::invalid_argument("Test for constraint not found!");
        return nullptr;
    }
}

FuxTest::FuxTest(int testNumber): FuxTest(testNumber, 0){
    idx = cp.size();
}

FuxTest::FuxTest(int testNumber, int i){
    if(testNumber==22){
        test_2v_1sp_fig22_setter(i);
    } else if(testNumber==23){
        test_2v_1sp_fig23_setter(i);
    } else if(testNumber==38){
        test_2v_2sp_fig38_setter(i);
    } else if(testNumber==39){
        test_2v_2sp_fig39_setter(i);
    } else if(testNumber==40){
        test_2v_2sp_fig40_setter(i);
    } else if(testNumber==55){
        test_2v_3sp_fig55_setter(i);
    } else if(testNumber==74){
        test_2v_4sp_fig74_setter(i);
    }else if(testNumber==82){
        test_2v_5sp_fig82_setter(i);
    }else if(testNumber==108){
        test_3v_1sp_fig108_setter(i);
    } else if(testNumber==109){
        test_3v_1sp_fig109_setter(i);
    } else if(testNumber==110){
        test_3v_1sp_fig110_setter(i);
    } else if(testNumber==111){
        test_3v_1sp_fig111_setter(i);
    } else if(testNumber==125){
        test_3v_2sp_fig125_setter(i);
    } else if(testNumber==146){
        test_3v_4sp_fig146_setter(i);
    } else if(testNumber==166){
        test_4v_1sp_fig166_setter(i);
    } else if(testNumber==167){
        test_4v_1sp_fig167_setter(i);
    } else if(testNumber==176){
        test_4v_2sp_fig176_setter(i);
    }
    else {
        throw std::invalid_argument("This test number has not been implemented (yet)");
    }
}

CounterpointProblem* FuxTest::test_1sp_H1(){
    spList = {FIRST_SPECIES};
    v_type = {2};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[1], IRT_EQ, 72); //does not work since it is not a consonant
    return problem;
}

CounterpointProblem* FuxTest::test_1sp_H2(){
    spList = {FIRST_SPECIES};
    v_type = {2};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 64); 
    return problem;
}

CounterpointProblem* FuxTest::test_1sp_H3(){
    spList = {FIRST_SPECIES};
    v_type = {2};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[problem->getSize()-1], IRT_EQ, 64); 
    return problem;
}

CounterpointProblem* FuxTest::test_1sp_H3_2(){
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {2, 1};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[(problem->getSize()/2)-1], IRT_EQ, 67); 
    rel(problem->getHome(), problem->getSolutionArray()[(problem->getSize())-1], IRT_EQ, 65); 
    return problem;
}

CounterpointProblem* FuxTest::test_1sp_H4_1(){
    spList = {FIRST_SPECIES};
    v_type = {1};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 67); 
    return problem;
}

CounterpointProblem* FuxTest::test_1sp_H4_2(){
    spList = {FIRST_SPECIES};
    v_type = {1};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[problem->getSize()-1], IRT_EQ, 67); 
    return problem;
}

CounterpointProblem* FuxTest::test_1sp_H5(){
    spList = {FIRST_SPECIES};
    v_type = {0};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[1], IRT_EQ, 62); 
    return problem;
}

CounterpointProblem* FuxTest::test_1sp_H7(){
    spList = {FIRST_SPECIES};
    v_type = {0};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[problem->getSize()-2], IRT_EQ, 69); 
    return problem;
}

CounterpointProblem* FuxTest::test_1sp_H7_2(){
    spList = {FIRST_SPECIES};
    v_type = {-1};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[problem->getSize()-2], IRT_EQ, 55); 
    return problem;
}

CounterpointProblem* FuxTest::test_2sp_H2(){
    spList = {SECOND_SPECIES};
    v_type = {2};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    //these three lines are an example of the constraint allowing a dissonance
    //rel(problem->getHome(), problem->getSolutionArray()[2], IRT_EQ, 78); 
    //rel(problem->getHome(), problem->getSolutionArray()[3], IRT_EQ, 79); 
    //rel(problem->getHome(), problem->getSolutionArray()[4], IRT_EQ, 81); 
    //these three lines are an example of the constraint blocking a dissonance
    rel(problem->getHome(), problem->getSolutionArray()[2], IRT_EQ, 74); 
    rel(problem->getHome(), problem->getSolutionArray()[3], IRT_EQ, 79); 
    rel(problem->getHome(), problem->getSolutionArray()[4], IRT_EQ, 81); 
    return problem;
}

CounterpointProblem* FuxTest::test_3sp_H1(){
    spList = {THIRD_SPECIES};
    v_type = {2};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[4], IRT_EQ, 66);
    rel(problem->getHome(), problem->getSolutionArray()[5], IRT_EQ, 66);
    rel(problem->getHome(), problem->getSolutionArray()[6], IRT_EQ, 67);
    rel(problem->getHome(), problem->getSolutionArray()[7], IRT_EQ, 69);
    rel(problem->getHome(), problem->getSolutionArray()[8], IRT_EQ, 69);
    return problem;
}

CounterpointProblem* FuxTest::test_4sp_H2(){
    spList = {FOURTH_SPECIES};
    v_type = {-1};
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
    rel(problem->getHome(), problem->getSolutionArray()[1], IRT_EQ, 52);
    rel(problem->getHome(), problem->getSolutionArray()[2], IRT_EQ, 52);
    return problem;
}

vector<Species> FuxTest::getSpList(){
    return spList;
}

vector<int> FuxTest::getCf(){
    return cantusFirmus;
}

vector<int> FuxTest::getVType(){
    return v_type;
}

int FuxTest::getBMode(){
    return borrowMode;
}

vector<int> FuxTest::getCp(){
    return cp;
}

int FuxTest::getIdx(){
    return idx;
}

void FuxTest::test_2v_1sp_fig22_setter(int i){
    cout << "Fig. 22" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; //1sp 2v cf
    cp =           {69,64,67,65,64,72,69,71,71,69,68,69};
    v_type = {1};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_2v_1sp_fig23_setter(int i){
    cout << "Fig. 23" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; //1sp 2v cf
    cp =           {57,57,55,53,52,52,50,48,55,57,56,57};
    v_type = {-1};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_2v_2sp_fig38_setter(int i){
    cout << "Fig. 38" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; //1sp 2v cf
    cp =           {0,65,
                    64,62,
                    60,58,
                    57,55,
                    53,57,
                    60,58,
                    57,69,
                    67,64,
                    65,67,
                    69,65,
                    62,64,
                    65};
    v_type = {1};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_2v_2sp_fig39_setter(int i){
    cout << "Fig. 39 (the voice types as defined previously don't work here)" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {53,55,57,53,50,52,53,60,57,53,55,53}; //1sp 2v cf
    cp =           {0,53,52,48,53,52,50,48,46,58,55,60,57,53,52,48,53,41,45,50,48,52,53};
    v_type = {-1};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_2v_2sp_fig40_setter(int i){
    cout << "Fig. 40" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {55,60,59,55,60,64,62,67,64,60,62,59,57,55}; //1sp 2v cf
    cp =           {0,67,64,65,67,69,71,69,67,72,71,72,74,72,71,69,67,65,64,72,71,69,67,62,64,66,67};
    v_type = {2};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_2v_3sp_fig55_setter(int i){
    cout << "Fig. 55" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {62,65,64,62,67,65,69,67,65,64,62}; //1sp 2v cf
    cp =           {62,64,65,67,//ok
                    69,71,72,74,//ok
                    76,74,71,72,//ok
                    74,72,70,69,//ok
                    70,72,74,76,//ok
                    77,65,69,71,//ok, but check for maybe bemol on last note
                    72,69,70,72,//ok
                    70,69,67,70,//HERE FIRST NOTE ERROR
                    69,62,64,65,
                    67,69,71,73,
                    74};
    cout << "CP size : " << endl;
    cout << cp.size() << endl;
    v_type = {1};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_2v_4sp_fig74_setter(int i){
    cout << "Fig. 74" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {62,   65,   64,   62,   67,   65,   69,   67,   65,   64,   62}; //1sp 2v cf
    cp =           {   50,50,62,62,60,60,59,59,64,64,62,62,65,65,64,64,62,62,61,62};
    cout << cp.size() << endl;
    v_type = {-1};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_2v_5sp_fig82_setter(int i){
    cout << "Fig. 82" << endl;
    spList = {FIFTH_SPECIES};
    cantusFirmus = {62,   65,   64,   62,   67,   65,   69,   67,   65,   64,   62}; //1sp 2v cf
    cp =           {0,0,69,69,69,62,64,65,67,65,64,67,65,62, 74,74,74,72,70,67,69,71,72,72,72,72, 77,77,77,76,76,76,76,69, 74,74,74,74,73,73,74};
    cout << cp.size() << endl;
    v_type = {1};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_3v_1sp_fig108_setter(int i){
    cout << "Fig. 108" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {67,64,65,69,72,72,76,72,69,68,
                    52,57,50,53,53,53,60,60,62,64};
    v_type = {1,-2};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_3v_1sp_fig109_setter(int i){
    cout << "Fig. 109" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {52,48,50,48,45,57,55,52,53,52};
    cp =           {67,64,65,64,64,64,67,67,62,64,
                    59,60,57,57,60,60,59,59,57,59};
    v_type = {2,0};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_3v_1sp_fig110_setter(int i){
    cout << "Fig. 110" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {60,60,60,62,58,59,57,69,65,62,64,65,
                    53,52,53,50,55,55,53,45,50,50,48,41};
    v_type = {-1 ,-3};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_3v_1sp_fig111_setter(int i){
    cout << "Fig. 111" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65};
    cp =           {69,72,72,74,74,70,69,67,65,77,76,77,
                    65,64,65,62,58,55,53,52,53,50,48,53};
    v_type = {1 ,-2};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_3v_2sp_fig125_setter(int i){
    cout << "Fig. 125" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    cantusFirmus = {52,   48,   50,   48,   45,   57,   55,   52,   53,   52};
    cp =           {64,   64,   65,   64,   65,   64,   67,   67,   69,   68,
                    00,59,60,59,57,53,55,52,57,59,60,57,59,62,64,62,60,57,59};
    v_type = {2 ,1};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_3v_4sp_fig146_setter(int i){
    cout << "Fig. 146" << endl;
    spList = {FOURTH_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,   60,   62,   60,   57,   69,   67,   64,   65,   64};
    cp =           {   76,76,72,72,71,71,69,69,72,72,74,74,72,72,69,69,71,68,
                    52,   57,   55,   57,   53,   53,   52,   48,   50,   52};
    v_type = {1 ,-2};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_4v_1sp_fig166_setter(int i){
    cout << "Fig. 166" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {59,57,57,57,60,60,64,60,62,59,
                    56,57,53,52,52,53,60,60,57,56,
                    52,53,50,45,45,41,40,45,50,52};
    v_type = {-1 ,-1, -3};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_4v_1sp_fig167_setter(int i){
    cout << "Fig. 167" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {64,60,62,60,57,69,67,64,65,64};
    cp =           {68,69,65,64,64,65,72,72,69,68,
                    59,57,57,57,60,60,64,60,62,59,
                    52,53,50,45,45,53,52,57,50,52};
    v_type = {0 ,-1, -3};
    idx = i;
    borrowMode = 1;
}

void FuxTest::test_4v_2sp_fig176_setter(int i){
    cout << "Fig. 173" << endl;
    spList = {SECOND_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    cantusFirmus = {50,   53,   52,   50,   55,   53,   57,   55,   53,   52,   50};
    cp =           {00,62,60,62,64,67,65,64,62,64,65,69,64,65,67,62,65,69,67,64,66,
                    65,   69,   72,   74,   70,   69,   69,   71,   74,   73,   74,
                    57,   57,   55,   57,   58,   60,   60,   62,   57,   64,   57};
    v_type = {2, 3, 1};
    idx = i;
    borrowMode = 1;
}


//=======================================================================================
//=======================================================================================

void FuxTest::test_bryce_2v_1sp(){
    cout << "===== test_bryce 2v 1sp =====" << endl;
    spList = {FIRST_SPECIES};
    v_type = {0};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

void FuxTest::test_bryce_2v_2sp(){
    cout << "===== test_bryce 2v 2sp =====" << endl;
    spList = {SECOND_SPECIES};
    v_type = {1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
    

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%10 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }
    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }
    
    delete problem;
}

void FuxTest::test_bryce_2v_3sp(){
    cout << "===== test_bryce 2v 3sp =====" << endl;
    spList = {THIRD_SPECIES};
    v_type = {1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
    

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%10 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }
    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }
    
    delete problem;
}

void FuxTest::test_bryce_2v_4sp(){
    cout << "===== test_bryce 2v 4sp =====" << endl;
    spList = {FOURTH_SPECIES};
    v_type = {0};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    auto home = problem->getHome();
    

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%10 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }
    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }
    
    delete problem;
}

void FuxTest::test_bryce_3v_1sp(){
    cout << "===== test_bryce 3v 1sp =====" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES};
    v_type = {0, 1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%100 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

void FuxTest::test_bryce_3v_2sp(){
    cout << "===== test_bryce 3v 2sp =====" << endl;
    spList = {FIRST_SPECIES, SECOND_SPECIES};
    v_type = {0, 1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%100 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

void FuxTest::test_bryce_3v_2sp_2sp(){
    cout << "===== test_bryce 3v 2sp 2sp =====" << endl;
    spList = {SECOND_SPECIES, SECOND_SPECIES};
    v_type = {0, 1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%100 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

void FuxTest::test_bryce_3v_3sp(){
    cout << "===== test_bryce 3v 3sp =====" << endl;
    spList = {FIRST_SPECIES, THIRD_SPECIES};
    v_type = {0, 1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%1000 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

void FuxTest::test_bryce_3v_3sp_3sp(){
    cout << "===== test_bryce 3v 3sp 3sp =====" << endl;
    spList = {THIRD_SPECIES, THIRD_SPECIES};
    v_type = {0, 1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%100 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }   
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

void FuxTest::test_bryce_4v_1sp(){
    cout << "===== test_bryce 4v 1sp =====" << endl; 
    spList = {FIRST_SPECIES, FIRST_SPECIES, FIRST_SPECIES};
    v_type = {0, 1, 2};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%1000 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

void FuxTest::test_bryce_4v_2sp(){
    cout << "===== test_bryce 4v 2sp =====" << endl;
    spList = {FIRST_SPECIES, FIRST_SPECIES, SECOND_SPECIES};
    v_type = {0, 1, 2};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%10000== 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

void FuxTest::test_bryce_4v_2sp_2sp_2sp(){
    cout << "===== test_bryce 4v 2sp =====" << endl;
    spList = {SECOND_SPECIES, SECOND_SPECIES, SECOND_SPECIES};
    v_type = {0, 1, 2};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    // Additional and necessary Constrains
    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);
    auto home = problem->getHome();
 

    // BAB
    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount%1000 == 0){
            std::cout << "BAB " << myCount << " : " << s->getSolutionArray() << std::endl;
        }   
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }

    delete problem;
}

//=======================================================================================

void FuxTest::test_bryce(){
    cout << "===== test_bryce =====" << endl; 
    cantusFirmus = {60,   62,   65,   64,   67,   65,   64,   62,   60};
    cfSize = cantusFirmus.size();
    melodic_params = {0, 1, 2, 576, 5, 10, 25, 40};
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    borrowMode = 1;

    spList = {FIRST_SPECIES};
    v_type = {1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    //activeConstraints[SP1_1H6] = true;
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

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
            std::cout << "==========\nTEST NUMBER " << myCount << std::endl;
            std::cout << "=== BAB:\n" << s->to_string() << std::endl;
            std::cout << "=== END TEST " << myCount << std::endl;
            //std::cout << "H INTERVALS:\n" << s->getCounterpoint_1()->getHIntervals() << std::endl;
        }
        delete best;      // keep only the best-so-far
        best = s;
        myCount++;
    }

    if (best) {
        std::cout << "BEST (" << myCount << "): " << best->getSolutionArray() << std::endl;
        delete best;
    }
    
    delete problem;

}

static std::vector<int> extract_voice_notes(CounterpointProblem* best,
                                            int n_voices,
                                            int cfSize) {
    auto arr = best->getSolutionArray();
    std::vector<int> out;

    int start = 0;
    if (n_voices == 3) start = cfSize;
    else if (n_voices == 4) start = 2 * cfSize;

    out.reserve(arr.size() - start);
    for (int i = start; i < arr.size(); ++i) out.push_back(arr[i].val());
    return out;
}

void test_bryce_gen_BAB_bench(CounterpointProblem *problem, std::vector<int> v_type, Species species, int n_voices, std::vector<int> cantusFirmus, int timeout_value){
    Search::Options opt;
    Search::TimeStop ts(timeout_value);
    opt.stop = &ts;

    auto t0 = std::chrono::steady_clock::now();
    auto t_first = t0;
    auto t_last = t0;
    double ms_first = 0.0;
    double ms_last = 0.0;

    BAB<CounterpointProblem> e(problem, opt);
    CounterpointProblem* best = nullptr;
    int myCount = 0;
    while (CounterpointProblem* s = e.next()) {
        if (myCount == 0) {
            t_first = std::chrono::steady_clock::now();
            ms_first = std::chrono::duration<double, std::milli>(t_first - t0).count();
        }
        else if (myCount % 100000 == 0){
            cout << best->getSolutionArray() << std::endl;
        }
        
        delete best; // keep only the best-so-far
        best = s;

        myCount++;
    }
    t_last = std::chrono::steady_clock::now();
    ms_last = std::chrono::duration<double, std::milli>(t_last - t0).count();

    if (best) { // there is a solution !
        auto solutionArray = best->getSolutionArray();
        std::vector<int> solVec;
        int cfSize = cantusFirmus.size();
        switch(n_voices) {
            case 4:
                for (int i = 2*cfSize; i < solutionArray.size(); ++i) {
                    solVec.push_back(solutionArray[i].val());
                }
                break;
            case 3:
                for (int i = cfSize; i < solutionArray.size(); ++i) {
                    solVec.push_back(solutionArray[i].val());
                }
                break;
            default: // 2 voices
                for (int i = 0; i < solutionArray.size(); ++i) {
                    solVec.push_back(solutionArray[i].val());
                }
                break;;
        }
        std::vector<int> cfVec;
        for (int i = 0; i < cfSize; ++i) {
            cfVec.push_back(cantusFirmus[i]);
        }
        
        std::string filename = "midi/bryce_"
            + std::to_string(n_voices) + "v_"
            + std::to_string((int)species+1) + "sp_";
        for (int vt : v_type) {
            filename += std::to_string(vt) + "_";
        }
        filename += ".mid";
        saveMidi(filename, cfVec, extract_voice_notes(best, n_voices, cfSize), species);
        // std::cout << "BEST (" << myCount << "): " << solutionArray << std::endl;
        delete best;

        std::cout
            << "best found = " << !e.stopped()
            << ",  stored in = " << filename
            << "\nbest solution = " << solutionArray
            << "\nfirst_ms = " << ms_first
            << ",  total_ms = " << ms_last
            << "\n";
    }
    else {
        std::cout
            << "  no solution found !!"
            << "  Time wasted =" << ms_last
            << "\n";

    }
}

void FuxTest::test_bryce_midi_gen(Species species, int n_voices, vector<int> v_cases, vector<int> v_cases_1sp){
    test_bryce_midi_gen(species, n_voices, v_cases, v_cases_1sp, 600); // 10min
}

void FuxTest::test_bryce_midi_gen(Species species, int n_voices, vector<int> v_cases, vector<int> v_cases_1sp, int timeout_s){
    cout << "===== test_bryce midi gen " << n_voices << "v " << species+1 << "sp " << " =====" << endl;
    cantusFirmus = {60,   62,   65,   64,   67,   65,   64,   62,   60};
    cfSize = cantusFirmus.size();
    melodic_params = {0, 1, 2, 576, 5, 10, 25, 40};
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    borrowMode = 1;

    // Voices & Constrains definition
    spList = {species};
    int n_cases_4v = 1;
    int n_cases_3v = 1;
    if (n_voices == 4) {
        spList = {FIRST_SPECIES, FIRST_SPECIES, species};
        n_cases_3v = v_cases_1sp.size();
        n_cases_4v = v_cases_1sp.size();
    }
    if (n_voices == 3) {
            spList = {FIRST_SPECIES, species};
            n_cases_3v = v_cases_1sp.size();
    }

    int timeout_value = timeout_s*1000; // ms
    for (int vidx_1sp_2 = 0; vidx_1sp_2 < n_cases_4v; vidx_1sp_2++){
        for (int vidx_1sp_1 = 0; vidx_1sp_1 < n_cases_3v; vidx_1sp_1++){
            for (int vidx_species = 0; vidx_species < v_cases.size(); vidx_species++){
                switch(n_voices) {
                    case 4:
                        v_type = {v_cases_1sp.at(vidx_1sp_2), v_cases_1sp.at(vidx_1sp_1), v_cases.at(vidx_species)};
                        break;
                    case 3:
                        v_type = {v_cases_1sp.at(vidx_1sp_1), v_cases.at(vidx_species)};
                        break;
                    default: // 2 voices
                        v_type = {v_cases.at(vidx_species)};
                        break;;
                }
                string v_type_str = "\n=== v_type = ";
                for (size_t k = 0; k < v_type.size(); ++k) {
                    v_type_str += v_type[k];
                    v_type_str += (k+1<v_type.size()? ",":"");
                }
                std::cout << v_type_str << " ===\n";


                std::fill(activeConstraints.begin(), activeConstraints.end(), true);
                cout << "=== Testing with all constrains" << endl;
                
                auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                test_bryce_gen_BAB_bench(problem, v_type, species, n_voices, cantusFirmus, timeout_value);
                delete problem;
            }
        }
    }
}

void FuxTest::test_bryce_constrains_check(Species species, int n_voices, vector<int> v_cases, vector<int> v_cases_1sp){
    cout << "===== test_bryce constrains check " << n_voices << "v " << species+1 << "sp " << " =====" << endl;
    spList = {species};
    int n_cases_4v = 1;
    int n_cases_3v = 1;
    switch(n_voices) {
        case 4:
            spList = {FIRST_SPECIES, FIRST_SPECIES, species};
            n_cases_3v = v_cases_1sp.size();
            n_cases_4v = v_cases_1sp.size();
            break;
        case 3:
            spList = {FIRST_SPECIES, species};
            n_cases_3v = v_cases_1sp.size();
            break;
        default: // 2 voices
            break;;
    }

    int const_from = 52;
    int const_to = 61;
    switch(species) {
        case 3:
            const_from = 62;
            const_to = 76;
            break;
        case 4:
            const_from = 77;
            const_to = 91;
            break;
        case 5:
            const_from = 92;
            const_to = 113;
            break;
        default: // 2d species
            break;;
    }

    int timeout_value = 300000; // ms --> 5min
    for (int vidx_1sp_2 = 0; vidx_1sp_2 < n_cases_4v; vidx_1sp_2++){
        for (int vidx_1sp_1 = 0; vidx_1sp_1 < n_cases_3v; vidx_1sp_1++){
            for (int vidx_species = 0; vidx_species < v_cases.size(); vidx_species++){
                switch(n_voices) {
                    case 4:
                        v_type = {v_cases_1sp.at(vidx_1sp_2), v_cases_1sp.at(vidx_1sp_1), v_cases.at(vidx_species)};
                        cout << "===== Testing v_type = " << v_cases_1sp.at(vidx_1sp_2) << "," << v_cases_1sp.at(vidx_1sp_1) << "," << v_cases.at(vidx_species) << endl;
                        break;
                    case 3:
                        v_type = {v_cases_1sp.at(vidx_1sp_1), v_cases.at(vidx_species)};
                        cout << "===== Testing v_type = " << v_cases_1sp.at(vidx_1sp_1) << "," << v_cases.at(vidx_species) << endl;
                        break;
                    default: // 2 voices
                        v_type = {v_cases.at(vidx_species)};
                        cout << "===== Testing v_type = " << v_cases.at(vidx_species) << endl;
                        break;;
                }

                std::fill(activeConstraints.begin(), activeConstraints.end(), true);
                cout << "=== Testing with all constrains" << endl;
                
                auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                
                Search::Options opt;
                Search::TimeStop ts(timeout_value);
                opt.stop = &ts;
                BAB<CounterpointProblem> e(problem, opt);
                CounterpointProblem* best = nullptr;

                while (CounterpointProblem* s = e.next()) {
                    cout << "BAB ok" << std::endl;
                    
                    delete best; // keep only the best-so-far
                    best = s;

                    break;
                }
                delete problem;
                if (best) continue; // If already ok, don't test constrains

                for(int constrain=const_from; constrain <= const_to; constrain++){
                    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
                    activeConstraints[constrain] = false;
                    cout << "=== Testing without constrain " << constrain << endl;
                    
                    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);
                    
                    Search::Options opt;
                    Search::TimeStop ts(timeout_value);
                    opt.stop = &ts;
                    BAB<CounterpointProblem> e(problem, opt);
                    CounterpointProblem* best = nullptr;

                    while (CounterpointProblem* s = e.next()) {
                        cout << "BAB ok" << std::endl;
                        
                        delete best; // keep only the best-so-far
                        best = s;

                        break;
                    }

                    delete problem;
                }
            }
        }
    }
}

void FuxTest::test_bryce_2(){ // To target specific tests if needed
    test_bryce_midi_gen(THIRD_SPECIES, 2, {-2,0,2}, {});
    test_bryce_midi_gen(FOURTH_SPECIES, 2, {-2,0,2}, {});
    test_bryce_midi_gen(FIFTH_SPECIES, 2, {-2,0,2}, {});
    test_bryce_midi_gen(THIRD_SPECIES, 3, {-2,0,2}, {});
    test_bryce_midi_gen(FOURTH_SPECIES, 3, {-2,0,2}, {});
    test_bryce_midi_gen(FIFTH_SPECIES, 3, {-2,0,2}, {});
    test_bryce_midi_gen(THIRD_SPECIES, 4, {-2,0,2}, {});
    test_bryce_midi_gen(FOURTH_SPECIES, 4, {-2,0,2}, {});
    test_bryce_midi_gen(FIFTH_SPECIES, 4, {-2,0,2}, {});
    //test_bryce_constrains_check(FIFTH_SPECIES, 3, {-2,0,2}, {-2,0,2});
    //test_bryce_constrains_check(FOURTH_SPECIES, 3, {-2,0,2}, {-2,0,2});
}

void FuxTest::test_bryce_classic(){
    cout << "===== test_bryce classic =====" << endl; 
    test_bryce_2v_1sp();
    test_bryce_2v_2sp(); // super long à partir de ~30 itérations
    test_bryce_2v_3sp(); // super long à partir de ~30 itérations
    test_bryce_2v_4sp(); // 
    test_bryce_3v_1sp(); // super long à partir de ~100 itérations
    test_bryce_3v_2sp(); // super long à partir de ~700 itérations
    test_bryce_3v_3sp(); // super long à partir de ~5000 itérations
    test_bryce_4v_1sp(); // super long à partir de ~15000 itérations
    test_bryce_4v_2sp(); // super long à partir de ~50000 itérations
}

void FuxTest::test_bryce_fullsp(){
    cout << "===== test_bryce full sp =====" << endl;
    test_bryce_3v_2sp_2sp(); // super long à partir de ~1200 itérations
    test_bryce_3v_3sp_3sp(); // super long à partir de ~5500 itérations
    test_bryce_4v_2sp_2sp_2sp(); // super long à partir de ~20000 itérations
}

void FuxTest::test_bryce_all(){
    cout << "===== test_bryce all =====" << endl; 
    test_bryce_classic();
    test_bryce_fullsp();
}

void FuxTest::test_sacha(){
    cout << "===== sacha 1v 1sp =====" << endl; 
    cantusFirmus = {60,   62,   65,   64,   67,   65,   64,   62,   60};
    cfSize = cantusFirmus.size();
    melodic_params = {0, 1, 2, 576, 5, 10, 25, 40};
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
    borrowMode = 1;

    spList = {FIRST_SPECIES};
    v_type = {1};

    std::fill(activeConstraints.begin(), activeConstraints.end(), true);
    auto* problem = create_problem(cantusFirmus, spList, v_type, melodic_params, general_params, specific_params, importance, borrowMode);

    cout << "- Problem defined" << endl; 

    rel(problem->getHome(), problem->getSolutionArray()[0], IRT_EQ, 60);

    cout << "- Getting solutions" << endl; 

    BAB<CounterpointProblem> e(problem);
    CounterpointProblem* best = nullptr;
    while (CounterpointProblem* s = e.next()) {
        delete best;      // keep only the best-so-far
        best = s;
    }

    if (best) {
        auto solutionArray = best->getSolutionArray();
        std::vector<int> solVec;
        for (int i = 0; i < solutionArray.size(); ++i) {
            solVec.push_back(solutionArray[i].val());
        } 
        std::vector<int> cfVec;
        for (int i = 0; i < cantusFirmus.size(); ++i) {
            cfVec.push_back(cantusFirmus[i]);
        }

        // 4. Appeler saveMidi avec les vecteurs
        saveMidi("test_sacha.mid", cfVec, solVec, FIRST_SPECIES);
        std::cout << "BEST:\n" << best->to_string() << std::endl;
        delete best;
    }

    delete problem;
}


