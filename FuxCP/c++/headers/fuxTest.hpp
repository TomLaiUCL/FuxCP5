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
    int cfSize;
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
    void test_1H1_3v_1sp();
    void test_1H1_4v_1sp();
    void test_1H1_2v_2sp();
    void test_1H1_3v_2sp();
    void test_1H1_4v_2sp();
    void test_1H1_2v_3sp();
    void test_1H1_3v_3sp();
    void test_1H1_4v_3sp();
    void test_1H1_2v_4sp();
    void test_1H1_3v_4sp();
    void test_1H1_4v_4sp();
    void test_1H1_2v_5sp();
    void test_1H1_3v_5sp();
    void test_1H1_4v_5sp();

    void test_1H2();
    void test_1H2_2v_1sp();
    void test_1H2_2v_2sp();
    void test_1H2_2v_3sp();
    void test_1H2_2v_4sp();

    void test_1H3();
    void test_1H3_2v_1sp();
    void test_1H3_3v_1sp();
    void test_1H3_4v_1sp();
    void test_1H3_2v_2sp();
    void test_1H3_3v_2sp();
    void test_1H3_4v_2sp();
    void test_1H3_2v_3sp();
    void test_1H3_3v_3sp();
    void test_1H3_4v_3sp();
    void test_1H3_2v_4sp();
    void test_1H3_3v_4sp();
    void test_1H3_4v_4sp();

    void test_1H4();
    void test_1H4_2v_1sp();
    void test_1H4_3v_1sp();
    void test_1H4_4v_1sp();
    void test_1H4_2v_2sp();
    void test_1H4_3v_2sp();
    void test_1H4_4v_2sp();
    void test_1H4_2v_3sp();
    void test_1H4_3v_3sp();
    void test_1H4_4v_3sp();
    void test_1H4_2v_4sp();
    void test_1H4_3v_4sp();
    void test_1H4_4v_4sp();

    void test_1H5();
    void test_1H5_2v_1sp();
    void test_1H5_3v_1sp();
    void test_1H5_2v_2sp();
    void test_1H5_3v_2sp();
    void test_1H5_2v_3sp();
    void test_1H5_3v_3sp();
    void test_1H5_2v_4sp();
    void test_1H5_3v_4sp();

    void test_1H6();
    void test_1H6_2v_1sp();

    void test_1H7();
    void test_1H7_2v_1sp();
    void test_1H7_3v_1sp();
    void test_1H7_2v_2sp();
    void test_1H7_3v_2sp();
    void test_1H7_2v_3sp();
    void test_1H7_3v_3sp();
    void test_1H7_2v_4sp();
    void test_1H7_3v_4sp();

    void test_1H10();
    void test_1H10_3v_1sp();
    void test_1H10_3v_2sp();
    void test_1H10_3v_3sp();
    void test_1H10_3v_4sp();

    void test_1M2();
    void test_1M2_2v_1sp();
    void test_1M2_3v_1sp();
    void test_1M2_4v_1sp();

    void test_1P1_2v_1sp();

    void test_2H2();
    void test_2H2_2v_2sp();
    void test_2H2_3v_2sp();
    void test_2H2_4v_2sp();

    void test_2M2();
    void test_2M2_2v_2sp();

    void test_2v_1sp_fig22();
    void test_2v_1sp_fig23();
    void test_2v_2sp_fig38();
    void test_2v_2sp_fig39();
    void test_2v_2sp_fig40();
    void test_2v_2sp_fig41();
    void test_2v_2sp_fig42();
    void test_2v_2sp_fig43();
    void test_2v_2sp_fig44();
    void test_2v_2sp_fig45();
    void test_2v_3sp_fig55();
    void test_2v_3sp_fig56();
    void test_2v_3sp_fig57();
    void test_2v_3sp_fig58();
    void test_2v_3sp_fig59();
    void test_2v_3sp_fig60();
    void test_2v_4sp_fig74();
    void test_2v_4sp_fig75();
    void test_2v_4sp_fig76();
    void test_2v_4sp_fig77();
    void test_2v_4sp_fig78();
    void test_3v_1sp_fig108();
    void test_3v_1sp_fig109();
    void test_3v_1sp_fig110();
    void test_3v_1sp_fig111();
    void test_3v_1sp_fig112();
    void test_3v_1sp_fig113();
    void test_3v_1sp_fig114();
    void test_3v_1sp_fig115();
    void test_3v_1sp_fig116();
    void test_3v_1sp_fig117();
    void test_3v_1sp_fig118();
    void test_3v_1sp_fig119();
    void test_3v_2sp_fig125();
    void test_3v_2sp_fig128();
    void test_3v_2sp_fig129();
    void test_3v_3sp_fig132();
    void test_3v_3sp_fig133();
    void test_3v_4sp_fig150();
    void test_3v_4sp_fig151();
    void test_4v_1sp_fig171();
    void test_4v_1sp_fig172();
    void test_4v_2sp_fig175();
    void test_4v_2sp_fig176();
    void test_4v_3sp_fig184();
    void test_4v_3sp_fig186();
    void test_4v_4sp_fig193();
    void test_4v_4sp_fig196();


    int get_motions(CounterpointProblem* problem, int i);

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

    void test_configuration();

    void test_bryce_2v_1sp();
    void test_bryce_2v_2sp();
    void test_bryce_2v_3sp();
    void test_bryce_2v_4sp();
    void test_bryce_3v_1sp();
    void test_bryce_3v_2sp();
    void test_bryce_3v_2sp_2sp();
    void test_bryce_3v_3sp();
    void test_bryce_3v_3sp_3sp();
    void test_bryce_4v_1sp();
    void test_bryce_4v_2sp();
    void test_bryce_4v_2sp_2sp_2sp();
    void test_sacha_4v_3sp();
    
    void test_bryce();
    void test_bryce_2();
    void test_bryce_classic();
    void test_bryce_fullsp();
    void test_bryce_all();
    void test_sacha();
};

void printVector(const std::vector<int>& array);

void printIntVarArray(const IntVarArray& array);

std::vector<CounterpointProblem*> get_all_solutions(CounterpointProblem* problem);


#endif