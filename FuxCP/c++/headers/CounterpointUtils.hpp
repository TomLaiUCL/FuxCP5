#ifndef COUNTERPOINTUTILS_HPP
#define COUNTERPOINTUTILS_HPP

#include "Utilities.hpp"
#include "CounterpointProblems/TwoVoiceCounterpoint.hpp"
#include "CounterpointProblems/ThreeVoiceCounterpoint.hpp"
#include "CounterpointProblems/FourVoiceCounterpoint.hpp"
#include "Parts/FirstSpeciesCounterpoint.hpp"
#include "Parts/SecondSpeciesCounterpoint.hpp"
#include "Parts/ThirdSpeciesCounterpoint.hpp"

using namespace std;
using namespace Gecode;


/**
 * This function creates the appropriate Part given the species of the counterpoint requested. 
 * @return a pointer to a Part object
*/
Part* create_counterpoint(Home home, int species, int nMeasures, vector<int> cantusFirmus, int lowerBound, int upperBound, int key, Stratum* low,
    CantusFirmus* c, int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV);

/**
 * This function creates the appropriate counterpoint problem given the number of counterpoints (size of the species list) requested. 
 * @return a pointer to a counterpoint problem object
*/
CounterpointProblem* create_problem(vector<int> cf, vector<int> sp, int k, int lb, int ub, vector<int> v_type, vector<int> m_costs, vector<int> g_costs,
    vector<int> s_costs, int bm);

CounterpointProblem* create_problem(vector<int> cf, vector<int> sp, int k, int lb, int ub, vector<int> v_type, vector<int> m_costs, vector<int> g_costs,
    vector<int> s_costs, int bm, int i, vector<int> cp);

/**
 * Creates the lowest strata deciding which notes currently are the lowest
 */
void create_lowest(Home home, Stratum* lowest, CantusFirmus* cf, Part* cp1, Part* cp2=nullptr);

///////////////////////////////////////////////////////////////////
//////////////     CONTSTRAINTS             ///////////////////////
///////////////////////////////////////////////////////////////////



#endif