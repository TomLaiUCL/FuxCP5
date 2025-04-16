#ifndef FIGURE_TESTS_HPP
#define FIGURE_TESTS_HPP

#include "fuxTest.hpp"

class FigureTests {
private:
    std::vector<Species> spList;
    std::vector<int> cantusFirmus;
    std::vector<int> cp;
    std::vector<int> v_type;
    std::vector<int> melodic_params;
    std::vector<int> general_params;
    std::vector<int> specific_params;
    std::vector<int> importance;
    int borrowMode;
    void test_configuration();
    std::vector<CounterpointProblem*> get_all_solutions();
    bool is_unsat(const set<int>& cons_set);
    set<int> minimize (const std::set<int>& activeCons);
    void findAllMUSes();

public:
    FigureTests();
    
    // Two voice first species figures
    void test_2v_1sp_fig22();
    void test_2v_1sp_fig23();
    
    // Two voice second species figures
    void test_2v_2sp_fig38();
    void test_2v_2sp_fig39();
    void test_2v_2sp_fig40();
    void test_2v_2sp_fig41();
    void test_2v_2sp_fig42();
    void test_2v_2sp_fig43();
    void test_2v_2sp_fig44();
    void test_2v_2sp_fig45();
    
    // Two voice third species figures
    void test_2v_3sp_fig55();
    void test_2v_3sp_fig56();
    void test_2v_3sp_fig57();
    void test_2v_3sp_fig58();
    void test_2v_3sp_fig59();
    void test_2v_3sp_fig60();
    
    // Two voice fourth species figures
    void test_2v_4sp_fig74();
    void test_2v_4sp_fig75();
    void test_2v_4sp_fig76();
    void test_2v_4sp_fig77();
    void test_2v_4sp_fig78();

    // Two voice fifth species figures
    void test_2v_5sp_fig86_1();
    
    // Three voice first species figures
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
    
    // Three voice second species figures
    void test_3v_2sp_fig125();
    void test_3v_2sp_fig126();
    void test_3v_2sp_fig127();
    void test_3v_2sp_fig128();
    void test_3v_2sp_fig129();

    // Three voice third species figures
    void test_3v_3sp_fig130();
    void test_3v_3sp_fig131();
    void test_3v_3sp_fig132();
    void test_3v_3sp_fig133();    

    // Three voice fourth species figures
    void test_3v_4sp_fig146();
    void test_3v_4sp_fig147();
    void test_3v_4sp_fig148();
    void test_3v_4sp_fig149();
    void test_3v_4sp_fig150();
    void test_3v_4sp_fig151();
    
    // Four voice first species figures
    void test_4v_1sp_fig166();
    void test_4v_1sp_fig167();
    void test_4v_1sp_fig168();
    void test_4v_1sp_fig169();
    void test_4v_1sp_fig170();
    void test_4v_1sp_fig171();
    void test_4v_1sp_fig172();
    
    // Four voice second species figures
    void test_4v_2sp_fig173();
    void test_4v_2sp_fig174();
    void test_4v_2sp_fig175();
    void test_4v_2sp_fig176();

    // Four voice third species figures
    void test_4v_3sp_fig183();
    void test_4v_3sp_fig184();
    void test_4v_3sp_fig185();
    void test_4v_3sp_fig186();

    // Four voice fourth species figures
    void test_4v_4sp_fig196();

    // Run all figure tests
    void run_all_tests();

    void run_twoVoice_tests();
    void run_threeVoice_tests();
    void run_fourVoice_tests();
    void run_fourthSpecies_tests();

    void MUSTest();
};

#endif // FIGURE_TESTS_HPP 