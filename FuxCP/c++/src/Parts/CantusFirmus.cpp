//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../../headers/Parts/CantusFirmus.hpp"

CantusFirmus::CantusFirmus(Home hme, int size, vector<int> cf, int k, Stratum* low, int v_type) : Part(hme, size, CANTUS_FIRMUS, cf, 0, 127, k, low, v_type){
    cf_vector = cf;
    notes = IntVarArray(home, size, lowerBound, upperBound);
    /// link aux variables de la part
    //rel(home, active_notes, IRT_EQ, notes.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),notes.size()));

    /// The cantus firmus is deterministic, so the values are assigned. It is useful to have it as an IntVarArray for > 3 voices.
    for(int i = 0; i < size; i++)
        rel(home, notes[i], IRT_EQ, cf_vector[i]);

    //branch(home, notes, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}

// IntVarArray CantusFirmus::getBranchingNotes(){
//     return notes;
// }


string CantusFirmus::to_string() const {
    string text = "Cantus Firmus : ";
    text += intVarArray_to_string(notes);
    text += "\n";
    return text;
}


// clone constructor
CantusFirmus::CantusFirmus(CantusFirmus &s) : Part(s){
    notes.update(home, s.notes);
}

CantusFirmus* CantusFirmus::copy(){
    return new CantusFirmus(*this);
}

//IntVarArray CantusFirmus::getBranchingNotes(){
//    return notes;
//}