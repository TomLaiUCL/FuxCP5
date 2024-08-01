#ifndef CONSTRAINTS_HPP
#define CONSTRAINTS_HPP

class Part;

#include "Parts/Part.hpp"

void G6_noChromaticMelodies(Home home, Part* part, int mSpec);

void G7_melodicIntervalsShouldBeSmall(Home home, Part* part, int mSpec);

void H1_1_harmonicIntrvalsAreConsonances(Home home, Part* part);

void H4_1_keyToneIsTuned(Home home, Part* part);

void H5_1_cpAndCfDifferentNotes(Home home, Part* part, Part* cf);

void H6_1_preferImperfectConsonances(Home home, Part* part);

void H7_1_2v_penultimateSixthOrThird(Home home, Part* part);

void M2_1_2v_melodicIntervalsNotExceedMinorSixth(Home home, Part* part);

void P1_1_2v_noDirectMotionFromPerfectConsonance(Home home, Part* part);

void P3_1_noBattuta(Home home, Part* part);

void M2_1_3v_melodicIntervalsNotExceedMinorSixth(Home home, Part* part);

void P1_1_3v_noDirectMotionFromPerfectConsonance(Home home, Part* part);

void H7_1_4v_penultimateSixthOrThird(Home home, Part* part);

#endif