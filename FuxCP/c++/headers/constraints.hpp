#ifndef CONSTRAINTS_HPP
#define CONSTRAINTS_HPP

class Part;

#include "Parts/Part.hpp"

void initializeIsOffArray(Home home, Part* part);

void G4_counterpointMustBeInTheSameKey(Home home, Part* part);

void G6_noChromaticMelodies(Home home, Part* part, int mSpec);

void G7_melodicIntervalsShouldBeSmall(Home home, Part* part, int mSpec);

void G9_lastChordSameAsFundamental(Home home, Stratum* lowest, Part* cantusFirmus);

void H1_1_harmonicIntrvalsAreConsonances(Home home, Part* part);

void H1_3_fiveConsecutiveNotesByJointDegree(Home home, Part* part);

void H2_1_startWithPerfectConsonance(Home home, Part* part);

void H2_2_arsisHarmoniesCannotBeDisonnant(Home home, Part* part);

void H2_3_disonanceImpliesDiminution(Home home, Part* part);

void H3_1_endWithPerfectConsonance(Home home, Part* part);

void H3_2_penultimateNoteDomain(Home home, Part* part);

void H3_3_cambiataCost(Home home, Part* part);

void H4_1_keyToneIsTuned(Home home, Part* part);

void H5_1_cpAndCfDifferentNotes(Home home, Part* part, Part* cf);

void H6_1_preferImperfectConsonances(Home home, Part* part);

void H7_1_2v_penultimateSixthOrThird(Home home, Part* part);

void H8_3v_preferHarmonicTriad(Home home, Part* part, IntVarArray triadCostArray, Stratum* upper1, Stratum* upper2);

void H8_4v_preferHarmonicTriad(Home home, IntVarArray triadCostArray, Stratum* upper1, Stratum* upper2, Stratum* upper3);

void M2_1_2v_melodicIntervalsNotExceedMinorSixth(Home home, Part* part);

void M2_2_2v_twoConsecutiveNotesAreNotTheSame(Home home, Part* part);

void M2_2_3v_melodicIntervalsNotExceedMinorSixth(Home home, vector<Part*> parts, bool containsThirdSpecies);

void M4_varietyCost(Home home, vector<Part*> parts);

void P1_1_2v_noDirectMotionFromPerfectConsonance(Home home, Part* part);

void P1_1_4v_noDirectMotionFromPerfectConsonance(Home home, Part* part);

void P1_2_2v_noDirectMotionFromPerfectConsonance(Home home, Part* part);

void P1_2_3v_noDirectMotionFromPerfectConsonance(Home home, Part* part);

void P1_2_4v_noDirectMotionFromPerfectConsonance(Home home, Part* part);

void P3_1_noBattuta(Home home, Part* part);

void P3_2_noBattuta(Home home, Part* part);

void P4_successiveCost(Home home, vector<Part*> parts, int scc_cz, IntVarArray successiveCostArray, vector<Species> species);

void P6_noMoveInSameDirection(Home home, vector<Part*> parts);

void P7_noSuccessiveAscendingSixths(Home home, vector<Part*> parts);

void M2_1_3v_melodicIntervalsNotExceedMinorSixth(Home home, Part* part);

void P1_1_3v_noDirectMotionFromPerfectConsonance(Home home, Part* part);

void H7_1_4v_penultimateSixthOrThird(Home home, Part* part);

#endif