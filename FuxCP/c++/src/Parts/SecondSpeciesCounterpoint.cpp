//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../../headers/Parts/SecondSpeciesCounterpoint.hpp"

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k):
    FirstSpeciesCounterpoint(home, size, cf, lb, ub, k, SECOND_SPECIES) /// super constructor. Applies all rules for the first species to the 1st note of each measure
{
    /// Second species notes in the counterpoint
    secondSpeciesNotesCp = IntVarArray(home, nMeasures*notesPerMeasure.at(SECOND_SPECIES), lowerBound, upperBound);
    rel(home, secondSpeciesNotesCp, IRT_EQ, cp.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),cp.size()));

    /// Harmonic intervals for the second species notes
    secondSpeciesHarmonicIntervals = IntVarArray(home, nMeasures*notesPerMeasure.at(SECOND_SPECIES), UNISSON, PERFECT_OCTAVE);
    rel(home, secondSpeciesHarmonicIntervals, IRT_EQ, hIntervalsCpCf.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),hIntervalsCpCf.size()));

    /// Melodic intervals for the second species notes
    secondSpeciesMelodicIntervals = IntVarArray(home, nMeasures*notesPerMeasure.at(SECOND_SPECIES) -1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    /// link melodic intervals
    for(int i = 0; i < secondSpeciesMelodicIntervals.size(); i++)
        rel(home, secondSpeciesMelodicIntervals[i], IRT_EQ, expr(home, secondSpeciesNotesCp[i+1] - secondSpeciesNotesCp[i]));

    /// Constraints

    /// 2.M2: Two consecutive notes cannot be the same
    rel(home, secondSpeciesMelodicIntervals, IRT_NQ, 0); /// plus efficace que de faire cp[i] /= cp[i+1]

    //todo maybe do like in the 1st species, 2 separate constructors but not done in this example
    /// branching strategy
    branch(home, secondSpeciesNotesCp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

string SecondSpeciesCounterpoint::to_string() const {
    string text = FirstSpeciesCounterpoint::to_string() + "\n Second species : \n";
    text += "Second species notes: " + intVarArray_to_string(secondSpeciesNotesCp) + "\n";
    text += "Second species harmonic intervals: " + intVarArray_to_string(secondSpeciesHarmonicIntervals) + "\n";
    text += "Second species melodic intervals: " + intVarArray_to_string(secondSpeciesMelodicIntervals) + "\n";
    return text;
}

// SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(SecondSpeciesCounterpoint &s): FirstSpeciesCounterpoint(s){     COPY CONSTRUCTOR NO LONGER NEEDED, THIS IS NO LONGER A SPACE
//     secondSpeciesNotesCp.update(*this, s.secondSpeciesNotesCp);
//     secondSpeciesHarmonicIntervals.update(*this, s.secondSpeciesHarmonicIntervals);
//     secondSpeciesMelodicIntervals.update(*this, s.secondSpeciesMelodicIntervals);

// }

// Space* SecondSpeciesCounterpoint::copy() {
//     return new SecondSpeciesCounterpoint(*this);
// }

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(Home home, SecondSpeciesCounterpoint &s): FirstSpeciesCounterpoint(home, s){  
    secondSpeciesNotesCp.update(home, s.secondSpeciesNotesCp);
    secondSpeciesHarmonicIntervals.update(home, s.secondSpeciesHarmonicIntervals);
    secondSpeciesMelodicIntervals.update(home, s.secondSpeciesMelodicIntervals);

}

SecondSpeciesCounterpoint* SecondSpeciesCounterpoint::clone(Home home){
    return new SecondSpeciesCounterpoint(home, *this);
}