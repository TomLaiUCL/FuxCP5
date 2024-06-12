//
// Created by Damien Sprockeels on 11/06/2024.
//

#include "../headers/FirstSpeciesCounterpoint.hpp"

FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(int size, vector<int> cf,int lb, int ub, int k, int mSpecies):
        Part(size, FIRST_SPECIES, cf, lb, ub, k) { /// super constructor
    motherSpecies = mSpecies;
    /// First species notes in the counterpoint
    firstSpeciesNotesCp = IntVarArray(*this, nMeasures, lowerBound, upperBound);
    rel(*this, firstSpeciesNotesCp, IRT_EQ, cp.slice(0,4/notesPerMeasure.at(species),cp.size()));
    /// Harmonic intervals for the first species notes
    firstSpeciesHarmonicIntervals = IntVarArray(*this, nMeasures, UNISSON, PERFECT_OCTAVE);
    rel(*this, firstSpeciesHarmonicIntervals, IRT_EQ, hIntervalsCpCf.slice(0,4,hIntervalsCpCf.size()));
    /// Melodic intervals for the first species notes
    firstSpeciesMelodicIntervals = IntVarArray(*this, nMeasures-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    ///link melodic intervals
    for(int i = 0; i < firstSpeciesMelodicIntervals.size(); i++)
        rel(*this, firstSpeciesMelodicIntervals[i], IRT_EQ, expr(*this, firstSpeciesNotesCp[i+1] - firstSpeciesNotesCp[i]));


    /// General rules

    /// Harmonic rules

    //todo put each rule in a function so it is easy to call them in different constructors depending on the species calling the first species
    /// H1 from Thibault: All harmonic intervals must be consonances
    dom(*this, firstSpeciesHarmonicIntervals, IntSet(IntArgs(CONSONANCES)));

    /// H2 from Thibault: The first harmonic interval must be a perfect consonance
    dom(*this, hIntervalsCpCf[0], IntSet(IntArgs(PERFECT_CONSONANCES)));

    /// H3 from Thibault: The last harmonic interval must be a perfect consonance
    dom(*this, hIntervalsCpCf[hIntervalsCpCf.size()-1], IntSet(IntArgs(PERFECT_CONSONANCES)));

    //todo add other harmonic rules here

    /// Melodic rules

    /// M2 from Thibault: Melodic intervals cannot exceed a minor sixth
    //rel(*this, mIntervalsCp, IRT_LQ, MINOR_SIXTH);
    //todo add other melodic rules here

    /// Motion rules
    //todo add motion rules here
}

FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(int size, vector<int> cf,int lb, int ub, int k) :
        FirstSpeciesCounterpoint(size, cf, lb, ub, k, FIRST_SPECIES) ///call the general constructor
{
    //todo add here rules that are specific to the first species, rules that are used by other species are in the general constructor

    //branching strategy todo maybe move it to the problem class, we'll see
    ///only branch on the relevant variables for this species, others are ignored
    branch(*this, firstSpeciesNotesCp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

string FirstSpeciesCounterpoint::to_string(){
    string text = Part::to_string();
    text += "First species notes: " + cleanIntVarArray_to_string(firstSpeciesNotesCp) + "\n";
    text += "First species harmonic intervals: " + cleanIntVarArray_to_string(firstSpeciesHarmonicIntervals) + "\n";
    text += "First species melodic intervals: " + cleanIntVarArray_to_string(firstSpeciesMelodicIntervals) + "\n";
    return text;
}



FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(FirstSpeciesCounterpoint &s) : Part(s){
    motherSpecies = s.motherSpecies;
    firstSpeciesNotesCp.update(*this, s.firstSpeciesNotesCp);
    firstSpeciesHarmonicIntervals.update(*this, s.firstSpeciesHarmonicIntervals);
    firstSpeciesMelodicIntervals.update(*this, s.firstSpeciesMelodicIntervals);
}

Space* FirstSpeciesCounterpoint::copy() {
    return new FirstSpeciesCounterpoint(*this);
}