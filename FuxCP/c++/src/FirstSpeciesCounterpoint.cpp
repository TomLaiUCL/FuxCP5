//
// Created by Damien Sprockeels on 11/06/2024.
//

#include "../headers/FirstSpeciesCounterpoint.hpp"

FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(vector<int> cf,int lb, int ub, int k, int mSpecies):
                        Counterpoint(FIRST_SPECIES, cf, lb, ub, k) {
    motherSpecies = mSpecies;

    cp = IntVarArray(*this, nMeasures, lowerBound, upperBound); /// rule G2 from Thibault (rule G3 is implicit)
    hIntervalsCpCf = IntVarArray(*this, nMeasures, UNISSON, PERFECT_OCTAVE);
    mIntervalsCp = IntVarArray(*this, nMeasures-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);

    /// link harmonic intervals
    for (int i = 0; i < nMeasures; i++){
        rel(*this, expr(*this, abs(cp[i] - cantusFirmus[i])), IRT_EQ, hIntervalsCpCf[i]); /// absolute value is rule G1 from Thibault
    }
    /// link melodic intervals
    for(int i = 0; i < nMeasures-1; i++){
        rel(*this, mIntervalsCp[i], IRT_EQ, expr(*this, cp[i+1] - cp[i]));
    }
    //@todo link motions using the enum in headers/Utilities.hpp


    /// Constraints

    /// rule G4 from Thibault: The counterpoint has to be in the key of the Cantus Firmus
    dom(*this, cp, IntSet(IntArgs(get_all_notes_from_scale(key, MAJOR_SCALE)))); //@todo add borrowed notes here, see Thibault's thesis

    /// G6 from Thibault, self explanatory (comes from the parent class)
    noChromaticMelodies();

    /// H1 from Thibault: All harmonic intervals must be consonances
    dom(*this, hIntervalsCpCf, IntSet(IntArgs(CONSONANCES)));

    /// test zone
    rel(*this, cp[0], IRT_EQ, 64);

    //branching strategy
    branch(*this, cp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}



FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(FirstSpeciesCounterpoint &s) : Counterpoint(s){
    motherSpecies = s.motherSpecies;
}

Space* FirstSpeciesCounterpoint::copy() {
    return new FirstSpeciesCounterpoint(*this);
}