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
FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, int mSpecies, Stratum* low, CantusFirmus* c,
     int v_type):
        Part(home, nMes, FIRST_SPECIES, cf, lb, ub, k, low, v_type) { /// super constructor
    motherSpecies = mSpecies;
    lengthCp1stSpecies = nMeasures;
    //cantus = c;

    for(int i = lowerBound; i <= upperBound; i++){
        cp_range.push_back(i);
    }

    extended_domain = vector_intersection(cp_range, vector_union(scale, borrowed_scale));
    off_domain = vector_difference(vector_intersection(cp_range, scale), lowerBound, upperBound);
    /// First species notes in the counterpoint
    firstSpeciesNotesCp = IntVarArray(home, nMeasures * notesPerMeasure.at(FIRST_SPECIES), IntSet(IntArgs(extended_domain)));
    //firstSpeciesNotesCp[firstSpeciesNotesCp.size()-2] = IntVar(home, 0, 127); //TODO : to be changed to correct domain

    rel(home, firstSpeciesNotesCp, IRT_EQ, notes.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),notes.size()));

    
    /// Harmonic intervals for the first species notes
    // firstSpeciesHarmonicIntervals = IntVarArray(home, nMeasures* notesPerMeasure.at(FIRST_SPECIES), UNISSON, PERFECT_OCTAVE);
    // rel(home, firstSpeciesHarmonicIntervals, IRT_EQ, hIntervalsCpCf.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),hIntervalsCpCf.size()));
    /// Melodic intervals for the first species notes
    //firstSpeciesMelodicIntervals = IntVarArray(home, nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    ///link melodic intervals
    //for(int i = 0; i < firstSpeciesMelodicIntervals.size(); i++)
    //    rel(home, firstSpeciesMelodicIntervals[i], IRT_EQ, expr(home, firstSpeciesNotesCp[i+1] - firstSpeciesNotesCp[i]));

    /// General rules
    //todo the rules hera are examples, they should be verified and changed if not accurate.

    /// Harmonic rules
    //todo put each rule in a function so it is easy to call them in different constructors depending on the species calling the first species
    /// H1 from Thibault: All harmonic intervals must be consonances
    //dom(home, firstSpeciesHarmonicIntervals, IntSet(IntArgs(CONSONANCES)));

    /// H2 from Thibault: The first harmonic interval must be a perfect consonance
    // dom(home, hIntervalsCpCf[0], IntSet(IntArgs(PERFECT_CONSONANCES)));

    /// H3 from Thibault: The last harmonic interval must be a perfect consonance
    // dom(home, hIntervalsCpCf[hIntervalsCpCf.size()-1], IntSet(IntArgs(PERFECT_CONSONANCES)));

    //todo add other harmonic rules here

    /// Melodic rules

    /// M2 from Thibault: Melodic intervals cannot exceed a minor sixth
    //rel(home, firstSpeciesMelodicIntervals, IRT_LQ, MINOR_SIXTH);
    //rel(home, firstSpeciesMelodicIntervals, IRT_GQ, -MINOR_SIXTH);
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
FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type) :
        FirstSpeciesCounterpoint(home, nMes, cf, lb, ub, k, FIRST_SPECIES, low, c, v_type) ///call the general constructor
{
    //todo add here rules that are specific to the first species, rules that are used by other species are in the general constructor

    ///branching strategy
    ///only branch on the relevant variables for this species, others from the Part are ignored
    //branch(home, firstSpeciesNotesCp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

/**
 * This function returns a string with the characteristics of the counterpoint. It calls the to_string() method from
 * the Part class and adds 1st species specific characteristics.
 * @return a string representation of the current instance of the FirstSpeciesCounterpoint class.
 */
string FirstSpeciesCounterpoint::to_string() const {
    string text = Part::to_string() + "\nFirst species :\n";
    text += "First species notes: " + intVarArray_to_string(firstSpeciesNotesCp) + "\n";
    //text += "First species harmonic intervals: " + intVarArray_to_string(firstSpeciesHarmonicIntervals) + "\n";
    //text += "First species melodic intervals: " + intVarArray_to_string(firstSpeciesMelodicIntervals) + "\n";
    return text;
}

// /// Copy constructor. This needs to copy all useful attributes and update variables. Must call the super copy constructor            NO MORE COPY CONSTRUCTOR SINCE THIS NO LONGER EXTENDS SPACE
// FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(FirstSpeciesCounterpoint &s) : Part(s){
//     motherSpecies = s.motherSpecies;
//     firstSpeciesNotesCp.update(*this, s.firstSpeciesNotesCp);
//     firstSpeciesHarmonicIntervals.update(*this, s.firstSpeciesHarmonicIntervals);
//     firstSpeciesMelodicIntervals.update(*this, s.firstSpeciesMelodicIntervals);
// }

// /// Copy function
// Space* FirstSpeciesCounterpoint::copy() {
//     return new FirstSpeciesCounterpoint(*this);
// }

// clone constructor
FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(Home home, FirstSpeciesCounterpoint &s) : Part(s){
    motherSpecies = s.motherSpecies;
    cantus = s.cantus;
    firstSpeciesNotesCp.update(home, s.firstSpeciesNotesCp);
    firstSpeciesHarmonicIntervals.update(home, s.firstSpeciesHarmonicIntervals);
    firstSpeciesMelodicIntervals.update(home, s.firstSpeciesMelodicIntervals);
}

FirstSpeciesCounterpoint* FirstSpeciesCounterpoint::clone(Home home){
    return new FirstSpeciesCounterpoint(home, *this);
}

IntVarArray FirstSpeciesCounterpoint::getBranchingNotes(){
    return firstSpeciesNotesCp;
}