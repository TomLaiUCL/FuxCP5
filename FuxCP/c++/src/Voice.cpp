#include "../headers/Voice.hpp"

Voice::Voice(Home home, int nMes, int lb, int ub){
    nMeasures = nMes; 
    size = nMes*4;

    lowerBound = lb;
    upperBound = ub;

    notes = IntVarArray(home, size, lowerBound, upperBound);
    h_intervals = IntVarArray(home, size, UNISSON, PERFECT_OCTAVE);
    // m_intervals_brut = IntVarArray(); TODO initialize correctly

    isLowest = BoolVarArray(home, nMeasures);
}


string Voice::to_string() const {
    string text = "voice : ";
    text += "VOICE TOSTRING NOT IMPLEMENTED YET";
    text += "\n";
    return text;
}


// clone constructor
Voice::Voice(Home home, Voice &s){
    nMeasures = s.nMeasures;
    size = s.size;
    lowerBound = s.lowerBound;
    upperBound = s.upperBound;
    notes.update(home, s.notes);
    h_intervals.update(home, s.h_intervals);
    // m_intervals_brut.update(home, s.m_intervals_brut);

    isLowest.update(home, s.isLowest);
}

Voice* Voice::clone(Home home){
    return new Voice(home, *this);
}