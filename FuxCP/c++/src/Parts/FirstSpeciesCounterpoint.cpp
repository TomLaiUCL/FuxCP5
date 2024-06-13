//
// Created by Damien Sprockeels on 11/06/2024.
//

#include "../../headers/Parts/FirstSpeciesCounterpoint.hpp"

/**
 * General constructor. It takes the mother species as an argument and sets all the variables as well as the general
 * rules that have to be applied regardless of the species. It does not apply 1st species specific rules and does not post branching.
 * todo maybe add a Stratum object or IntVarArray for the lowest voice
 * @param nMes the number of measures in the composition
 * @param cf the cantus firmus todo maybe it should be a CantusFirmusObject
 * @param lb the lower bound for the counterpoint
 * @param ub the upper bound for the counterpoint
 * @param k the key of the composition
 * @param mSpecies the species from which this is called.
 */
FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(int nMes, vector<int> cf, int lb, int ub, int k, int mSpecies):
        Part(nMes, FIRST_SPECIES, cf, lb, ub, k) { /// super constructor
    motherSpecies = mSpecies;
    lengthCp1stSpecies = nMeasures;
    /// First species notes in the counterpoint
    firstSpeciesNotesCp = IntVarArray(*this, nMeasures * notesPerMeasure.at(FIRST_SPECIES), lowerBound, upperBound);
    rel(*this, firstSpeciesNotesCp, IRT_EQ, cp.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),cp.size()));
    /// Harmonic intervals for the first species notes
    firstSpeciesHarmonicIntervals = IntVarArray(*this, nMeasures* notesPerMeasure.at(FIRST_SPECIES), UNISSON, PERFECT_OCTAVE);
    rel(*this, firstSpeciesHarmonicIntervals, IRT_EQ, hIntervalsCpCf.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),hIntervalsCpCf.size()));
    /// Melodic intervals for the first species notes
    firstSpeciesMelodicIntervals = IntVarArray(*this, nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    ///link melodic intervals
    for(int i = 0; i < firstSpeciesMelodicIntervals.size(); i++)
        rel(*this, firstSpeciesMelodicIntervals[i], IRT_EQ, expr(*this, firstSpeciesNotesCp[i+1] - firstSpeciesNotesCp[i]));

    /// General rules
    //todo the rules hera are examples, they should be verified and changed if not accurate.

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
    rel(*this, firstSpeciesMelodicIntervals, IRT_LQ, MINOR_SIXTH);
    rel(*this, firstSpeciesMelodicIntervals, IRT_GQ, -MINOR_SIXTH);
    //todo add other melodic rules here

    /// Motion rules
    //todo add motion rules here
}

/**
 * This constructor is only used when creating a counterpoint of the first species. It calls the other constructor with
 * FIRST_SPECIES as the mother species. Additionally, it posts 1st species specific constraints as well as the branching.
 * @param nMes the number of measures in the composition
 * @param cf the cantus firmus todo maybe it should be a CantusFirmusObject
 * @param lb the lower bound for the counterpoint
 * @param ub the upper bound for the counterpoint
 * @param k the key of the composition
 */
FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(int nMes, vector<int> cf, int lb, int ub, int k) :
        FirstSpeciesCounterpoint(nMes, cf, lb, ub, k, FIRST_SPECIES) ///call the general constructor
{
    //todo add here rules that are specific to the first species, rules that are used by other species are in the general constructor

    ///branching strategy
    ///only branch on the relevant variables for this species, others from the Part are ignored
    branch(*this, firstSpeciesNotesCp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

/**
 * This function returns a string with the characteristics of the counterpoint. It calls the to_string() method from
 * the Part class and adds 1st species specific characteristics.
 * @return a string representation of the current instance of the FirstSpeciesCounterpoint class.
 */
string FirstSpeciesCounterpoint::to_string(){
    string text = Part::to_string() + "\nFirst species :\n";
    text += "First species notes: " + intVarArray_to_string(firstSpeciesNotesCp) + "\n";
    text += "First species harmonic intervals: " + intVarArray_to_string(firstSpeciesHarmonicIntervals) + "\n";
    text += "First species melodic intervals: " + intVarArray_to_string(firstSpeciesMelodicIntervals) + "\n";
    return text;
}

/// Copy constructor. This needs to copy all useful attributes and update variables. Must call the super copy constructor
FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(FirstSpeciesCounterpoint &s) : Part(s){
    motherSpecies = s.motherSpecies;
    firstSpeciesNotesCp.update(*this, s.firstSpeciesNotesCp);
    firstSpeciesHarmonicIntervals.update(*this, s.firstSpeciesHarmonicIntervals);
    firstSpeciesMelodicIntervals.update(*this, s.firstSpeciesMelodicIntervals);
}

/// Copy function
Space* FirstSpeciesCounterpoint::copy() {
    return new FirstSpeciesCounterpoint(*this);
}