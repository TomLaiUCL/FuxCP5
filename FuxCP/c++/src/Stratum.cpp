#include "../headers/Stratum.hpp"

Stratum::Stratum(Home home, int nMes, int lb, int ub) : Voice(home, nMes, lb, ub){
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
Stratum::Stratum(Home home, Stratum &s) : Voice(home, s){
    // update/clone here all variables that are not in voice
}

Stratum* Stratum::clone(Home home){
    return new Stratum(home, *this);
}