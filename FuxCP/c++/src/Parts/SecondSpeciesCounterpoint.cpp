//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../../headers/Parts/SecondSpeciesCounterpoint.hpp"

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(int size, vector<int> cf,int lb, int ub, int k):
    FirstSpeciesCounterpoint(size, cf, lb, ub, k, SECOND_SPECIES) /// super constructor. Applies all rules for the first species to the 1st note of each measure
{
    /// Second species notes in the counterpoint
    secondSpeciesNotesCp = IntVarArray(*this, nMeasures*notesPerMeasure.at(SECOND_SPECIES), lowerBound, upperBound);
    rel(*this, secondSpeciesNotesCp, IRT_EQ, cp.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),cp.size()));

    /// Harmonic intervals for the second species notes
    secondSpeciesHarmonicIntervals = IntVarArray(*this, nMeasures*notesPerMeasure.at(SECOND_SPECIES), UNISSON, PERFECT_OCTAVE);
    rel(*this, secondSpeciesHarmonicIntervals, IRT_EQ, hIntervalsCpCf.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),hIntervalsCpCf.size()));

    /// Melodic intervals for the second species notes
    secondSpeciesMelodicIntervals = IntVarArray(*this, nMeasures*notesPerMeasure.at(SECOND_SPECIES) -1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    /// link melodic intervals
    for(int i = 0; i < secondSpeciesMelodicIntervals.size(); i++)
        rel(*this, secondSpeciesMelodicIntervals[i], IRT_EQ, expr(*this, secondSpeciesNotesCp[i+1] - secondSpeciesNotesCp[i]));

    /// Constraints

    /// 2.M2: Two consecutive notes cannot be the same
    rel(*this, secondSpeciesMelodicIntervals, IRT_NQ, 0); /// plus efficace que de faire cp[i] /= cp[i+1]

    //todo maybe do like in the 1st species, 2 separate constructors but not done in this example
    /// branching strategy
    branch(*this, secondSpeciesNotesCp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

string SecondSpeciesCounterpoint::to_string(){
    string text = FirstSpeciesCounterpoint::to_string() + "\n Second species : \n";
    text += "Second species notes: " + intVarArray_to_string(secondSpeciesNotesCp) + "\n";
    text += "Second species harmonic intervals: " + intVarArray_to_string(secondSpeciesHarmonicIntervals) + "\n";
    text += "Second species melodic intervals: " + intVarArray_to_string(secondSpeciesMelodicIntervals) + "\n";
    return text;
}

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(SecondSpeciesCounterpoint &s): FirstSpeciesCounterpoint(s){
    secondSpeciesNotesCp.update(*this, s.secondSpeciesNotesCp);
    secondSpeciesHarmonicIntervals.update(*this, s.secondSpeciesHarmonicIntervals);
    secondSpeciesMelodicIntervals.update(*this, s.secondSpeciesMelodicIntervals);

}

Space* SecondSpeciesCounterpoint::copy() {
    return new SecondSpeciesCounterpoint(*this);
}