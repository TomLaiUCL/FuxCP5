// 
// Created by Luc Cleenewerk and Diego de Patoul. 
// This file is the header file of the testing framework implementation.  
// 

#ifndef FUX_TESTS_HPP
#define FUX_TESTS_HPP

#include "Utilities.hpp"
#include "Parts/Part.hpp"
#include "CounterpointUtils.hpp"
#include "CounterpointProblems/CounterpointProblem.hpp"
#include "Parts/CantusFirmus.hpp"

using namespace Gecode;
using namespace std;

class FuxTest{

protected:
    vector<Species> spList;
    vector<int> cantusFirmus;
    vector<int> v_type;
    int borrowMode;
    vector<int> cp;
    int idx;
    int size;
    vector<int> melodic_params;
    vector<int> general_params;
    vector<int> specific_params;
    vector<int> importance;

public:

// ========== Modified by Tom ====================

    void test_G6();
    void test_G6_2v_1sp();

    void test_1H1();
    void test_1H1_2v_1sp();
    // void test_1H1_3v_1sp();

    void test_1H2();
    void test_1H2_2v_1sp();

    void test_1H3();
    void test_1H3_2v_1sp();
    void test_1H3_4v_1sp();

    void test_1H4();
    void test_1H4_2v_1sp();

    void test_1H5();
    void test_1H5_2v_1sp();

    void test_1H6();
    void test_1H6_2v_1sp();

    void test_1H7();
    void test_1H7_2v_1sp();

    void test_1M2();
    void test_1M2_2v_1sp();

// ===============================================

    FuxTest(char* test);

    FuxTest(int testNumber);

    FuxTest(int testNumber, int i);

    vector<Species> getSpList();
    vector<int> getCf();
    vector<int> getVType();
    int getBMode();
    vector<int> getCp();
    int getIdx();

    CounterpointProblem* test_1sp_H1();
    CounterpointProblem* test_1sp_H2();
    CounterpointProblem* test_1sp_H3();
    CounterpointProblem* test_1sp_H3_2();
    CounterpointProblem* test_1sp_H4_1();
    CounterpointProblem* test_1sp_H4_2();
    CounterpointProblem* test_1sp_H5();
    CounterpointProblem* test_1sp_H7();
    CounterpointProblem* test_1sp_H7_2();
    CounterpointProblem* test_2sp_H2();
    CounterpointProblem* test_3sp_H1();
    CounterpointProblem* test_4sp_H2();
    
    CounterpointProblem* dispatcher(char* test);

    void test_2v_1sp_fig22_setter(int i);
    void test_2v_1sp_fig23_setter(int i);

    void test_2v_2sp_fig38_setter(int i);
    void test_2v_2sp_fig39_setter(int i);
    void test_2v_2sp_fig40_setter(int i);

    void test_2v_3sp_fig55_setter(int i);

    void test_2v_4sp_fig74_setter(int i);

    void test_2v_5sp_fig82_setter(int i);

    void test_3v_1sp_fig108_setter(int i);
    void test_3v_1sp_fig109_setter(int i);
    void test_3v_1sp_fig110_setter(int i);
    void test_3v_1sp_fig111_setter(int i);

    void test_3v_2sp_fig125_setter(int i);

    void test_3v_4sp_fig146_setter(int i);

    void test_4v_1sp_fig166_setter(int i);
    void test_4v_1sp_fig167_setter(int i);

    void test_4v_2sp_fig176_setter(int i);

};

void printVector(const std::vector<int>& array);

void printIntVarArray(const IntVarArray& array);

std::vector<CounterpointProblem*> get_all_solutions(CounterpointProblem* problem);

bool has_solution(CounterpointProblem* problem);

#endif