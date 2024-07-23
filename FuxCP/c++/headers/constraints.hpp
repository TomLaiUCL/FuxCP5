#ifndef CONSTRAINTS_HPP
#define CONSTRAINTS_HPP

#include "Parts/Part.hpp"

using namespace std;
using namespace Gecode;

/**
 * G6 : no chromatic melodies
 */
void G6_no_chromatic_melodies(Home home, Part part);

/**
 * G7 : melodic intervals should be small
 */
void G7_melodic_intervals_should_be_small(Home home, Part part);

#endif