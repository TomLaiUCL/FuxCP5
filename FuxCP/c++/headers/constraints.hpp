#ifndef CONSTRAINTS_HPP
#define CONSTRAINTS_HPP

#include "Utilities.hpp"

using namespace std;
using namespace Gecode;

/**
 * G6 : no chromatic melodies
 */
void no_chromatic_melodies(Home home, IntVarArray m_intervals);

#endif