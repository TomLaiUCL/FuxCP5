//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../../headers/Parts/CantusFirmus.hpp"

CantusFirmus::CantusFirmus(Home home, int size, vector<int> cf, int k) : Part(home, size, CANTUS_FIRMUS, cf, 0, 127, k){
    cf_vector = cf;
    notes = IntVarArray(home, size, lowerBound, upperBound);
    /// link aux variables de la part
    rel(home, notes, IRT_EQ, cp.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),cp.size()));

    /// The cantus firmus is deterministic, so the values are assigned. It is useful to have it as an IntVarArray for > 3 voices.
    for(int i = 0; i < size; i++)
        rel(home, notes[i], IRT_EQ, cf_vector[i]);

    //branch(home, notes, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

IntVarArray CantusFirmus::getBranchingNotes(){
    return notes;
}


string CantusFirmus::to_string() const {
    string text = "Cantus Firmus : ";
    text += int_vector_to_string(cf_vector);
    text += "\n";
    return text;
}


// clone constructor
CantusFirmus::CantusFirmus(Home home, CantusFirmus &s) : Part(home, s){
    notes.update(home, s.notes);
}

CantusFirmus* CantusFirmus::clone(Home home){
    return new CantusFirmus(home, *this);
}