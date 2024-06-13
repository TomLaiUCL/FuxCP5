//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../../headers/Parts/SecondSpeciesCounterpoint.hpp"

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(int size, vector<int> cf,int lb, int ub, int k):
    FirstSpeciesCounterpoint(size, cf, lb, ub, k, SECOND_SPECIES) /// super constructor
{
    IntVarArgs relevantNotesCp = cp.slice(0,4,cp.size());

    /// branching strategy
    branch(*this, relevantNotesCp, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(FirstSpeciesCounterpoint &s) : FirstSpeciesCounterpoint(s){
}

Space* SecondSpeciesCounterpoint::copy() {
    return new SecondSpeciesCounterpoint(*this);
}