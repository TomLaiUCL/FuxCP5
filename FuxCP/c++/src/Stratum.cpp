#include "../headers/Stratum.hpp"

Stratum::Stratum(Home home, int nMes, int lb, int ub, int v_type) : Voice(home, nMes, lb, ub, v_type){
    // add here any variable initialization that can't be done in Voice constructor and that is independent of the number of voices
}

// IntVarArray Stratum::getBranchingNotes(){
//     return notes;
// }


string Stratum::to_string() const {
    string text = "Stratum : ";
    text += "STRATUM TOSTRING NOT IMPLEMENTED YET";
    text += "\n";
    return text;
}


// clone constructor
Stratum::Stratum(Home home, Stratum &s) : Voice(s){
    // update/clone here all variables that are not in voice
}

Stratum* Stratum::clone(Home home){
    return new Stratum(home, *this);
}

void Stratum::setNote(Home home, int index, IntVar note){
    rel(home, notes[index*4], IRT_EQ, note);
}