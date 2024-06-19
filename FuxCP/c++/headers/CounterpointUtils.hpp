#ifndef COUNTERPOINTUTILS_HPP
#define COUNTERPOINTUTILS_HPP

#include "Utilities.hpp"
#include "Parts/FirstSpeciesCounterpoint.hpp"
#include "Parts/SecondSpeciesCounterpoint.hpp"

using namespace std;
using namespace Gecode;



Part* create_counterpoint(Home home, int species, int nMeasures, vector<int> cantusFirmus, int lowerBound, int upperBound, int key);




#endif