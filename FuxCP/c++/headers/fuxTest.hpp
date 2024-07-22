#ifndef FUX_TESTS_HPP
#define FUX_TEST_HPP

#include "Parts/Part.hpp"

using namespace Gecode;
using namespace std;

class FuxTest{

protected:
    vector<int> species;
    vector<int> cantusFirmus;
    vector<int> v_type;
    int borrowMode;
    vector<int> cp;
    int idx;

public:

    FuxTest(int testNumber);

    FuxTest(int testNumber, int i);

    vector<int> getSpList();
    vector<int> getCf();
    vector<int> getVType();
    int getBMode();
    vector<int> getCp();
    int getIdx();

    void test_2v_1sp_fig22_setter(int i);
    void test_2v_1sp_fig23_setter(int i);
};

#endif