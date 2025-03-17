#include "../headers/figureTests.hpp"
#include <iostream>

using namespace std;

FigureTests::FigureTests() {
    borrowMode = 0;
    melodic_params = {0, 1, 1, 576, 2, 2, 2, 1};
    general_params = {4, 1, 1, 2, 2, 2, 8, 1};
    specific_params = {8, 4, 0, 2, 1, 8, 50};
    importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};
}

void FigureTests::test_configuration() {
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

void FigureTests::test_2v_1sp_fig22() {
    cout << "Start test_2v_1sp_fig22" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,64,67,65,64,72,69,71,71,69,68,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_1sp_fig22." << endl;
}

void FigureTests::test_2v_1sp_fig23() {
    cout << "Start test_2v_1sp_fig23" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,69,67,65,64,64,62,60,67,69,68,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_1sp_fig23." << endl;
}

void FigureTests::test_2v_2sp_fig38() {
    cout << "Start test_2v_2sp_fig38" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig38." << endl;
}

void FigureTests::test_2v_2sp_fig39() {
    cout << "Start test_2v_2sp_fig39" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig39." << endl;
}

void FigureTests::test_2v_2sp_fig40() {
    cout << "Start test_2v_2sp_fig40" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_2sp_fig40." << endl;
}

void FigureTests::test_2v_3sp_fig55() {
    cout << "Start test_2v_3sp_fig55" << endl;
    spList = {THIRD_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_3sp_fig55." << endl;
}

void FigureTests::test_2v_4sp_fig74() {
    cout << "Start test_2v_4sp_fig74" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_4sp_fig74." << endl;
}

void FigureTests::test_2v_5sp_fig82() {
    cout << "Start test_2v_5sp_fig82" << endl;
    spList = {FIFTH_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_2v_5sp_fig82." << endl;
}

void FigureTests::test_3v_1sp_fig108() {
    cout << "Start test_3v_1sp_fig108" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig108." << endl;
}

void FigureTests::test_3v_1sp_fig109() {
    cout << "Start test_3v_1sp_fig109" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig109." << endl;
}

void FigureTests::test_3v_1sp_fig110() {
    cout << "Start test_3v_1sp_fig110" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig110." << endl;
}

void FigureTests::test_3v_1sp_fig111() {
    cout << "Start test_3v_1sp_fig111" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_1sp_fig111." << endl;
}

void FigureTests::test_3v_2sp_fig125() {
    cout << "Start test_3v_2sp_fig125" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_2sp_fig125." << endl;
}

void FigureTests::test_3v_4sp_fig146() {
    cout << "Start test_3v_4sp_fig146" << endl;
    spList = {FOURTH_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_3v_4sp_fig146." << endl;
}

void FigureTests::test_4v_1sp_fig166() {
    cout << "Start test_4v_1sp_fig166" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_4v_1sp_fig166." << endl;
}

void FigureTests::test_4v_1sp_fig167() {
    cout << "Start test_4v_1sp_fig167" << endl;
    spList = {FIRST_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_4v_1sp_fig167." << endl;
}

void FigureTests::test_4v_2sp_fig176() {
    cout << "Start test_4v_2sp_fig176" << endl;
    spList = {SECOND_SPECIES};
    cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; 
    cp =           {69,67,64,67,65,64,72,69,71,69,67,69};
    v_type = {1,1,1};
    borrowMode = 1;
    test_configuration();
    cout << "End test_4v_2sp_fig176." << endl;
}

void FigureTests::run_all_tests() {
    cout << "Running all figure tests..." << endl;
    
    // Two voice first species figures
    test_2v_1sp_fig22();
    test_2v_1sp_fig23();
    
    // Two voice second species figures
    test_2v_2sp_fig38();
    test_2v_2sp_fig39();
    test_2v_2sp_fig40();
    
    // Two voice third species figures
    test_2v_3sp_fig55();
    
    // Two voice fourth species figures
    test_2v_4sp_fig74();
    
    // Two voice fifth species figures
    test_2v_5sp_fig82();
    
    // Three voice first species figures
    test_3v_1sp_fig108();
    test_3v_1sp_fig109();
    test_3v_1sp_fig110();
    test_3v_1sp_fig111();
    
    // Three voice second species figures
    test_3v_2sp_fig125();
    
    // Three voice fourth species figures
    test_3v_4sp_fig146();
    
    // Four voice first species figures
    test_4v_1sp_fig166();
    test_4v_1sp_fig167();
    
    // Four voice second species figures
    test_4v_2sp_fig176();
    
    cout << "All figure tests completed." << endl;
} 