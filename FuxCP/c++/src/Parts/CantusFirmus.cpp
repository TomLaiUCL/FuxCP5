//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../../headers/Parts/CantusFirmus.hpp"

CantusFirmus::CantusFirmus(int size, vector<int> cf, int k) : Part(size, CANTUS_FIRMUS, cf, 0, 127, k){
    cfVector = cf;
    notes = IntVarArray(*this, size, lowerBound, upperBound);
    /// link aux variables de la part
    rel(*this, notes, IRT_EQ, cp.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),cp.size()));

    /// The cantus firmus is deterministic, so the values are assigned. It is useful to have it as an IntVarArray for > 3 voices.
    for(int i = 0; i < size; i++)
        rel(*this, notes[i], IRT_EQ, cfVector[i]);

    branch(*this, notes, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

CantusFirmus::CantusFirmus(CantusFirmus &s) : Part(s){
    cfVector = s.cfVector;
    notes = s.notes;
}

Space *CantusFirmus::copy() {
    return new CantusFirmus(*this);
}
