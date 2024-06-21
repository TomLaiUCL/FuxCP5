#ifndef COUNTERPOINTUTILS_HPP
#define COUNTERPOINTUTILS_HPP

#include "Utilities.hpp"
#include "CounterpointProblems/TwoVoiceCounterpoint.hpp"
#include "CounterpointProblems/ThreeVoiceCounterpoint.hpp"
#include "Parts/FirstSpeciesCounterpoint.hpp"
#include "Parts/SecondSpeciesCounterpoint.hpp"

using namespace std;
using namespace Gecode;


/**
 * This function creates the appropriate Part given the species of the counterpoint requested. 
 * @return a pointer to a Part object
*/
Part* create_counterpoint(Home home, int species, int nMeasures, vector<int> cantusFirmus, int lowerBound, int upperBound, int key);

/**
 * This function creates the appropriate counterpoint problem given the number of counterpoints (size of the species list) requested. 
 * @return a pointer to a counterpoint problem object
*/
CounterpointProblem* create_problem(vector<int> cf, vector<int> sp, int k, int lb, int ub);

#endif