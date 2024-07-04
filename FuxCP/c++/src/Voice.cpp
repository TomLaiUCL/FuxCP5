#include "../headers/Voice.hpp"

Voice::Voice(Home hme, int nMes, int lb, int ub, int v_type) : home(hme){
    home = hme;
    nMeasures = nMes; 
    size = nMes*4;

    voice_type = v_type;

    lowerBound = lb;
    upperBound = ub;

    

    notes = IntVarArray(home, size, lowerBound, upperBound);
    h_intervals = IntVarArray(home, size, UNISSON, PERFECT_OCTAVE);
    // m_intervals_brut = IntVarArray(); TODO initialize correctly

    
}


string Voice::to_string() const {
    string text = "voice : ";
    text += "VOICE TOSTRING NOT IMPLEMENTED YET";
    text += "\n";
    return text;
}


// clone constructor
Voice::Voice(Voice &s):home(s.home){
    home = s.home;
    nMeasures = s.nMeasures;
    size = s.size;
    lowerBound = s.lowerBound;
    upperBound = s.upperBound;
    voice_type = s.voice_type;
    notes.update(home, s.notes);
    h_intervals.update(home, s.h_intervals);
    // m_intervals_brut.update(home, s.m_intervals_brut);
}

Voice* Voice::copy(){
    return new Voice(*this);
}

IntVarArgs Voice::getFirstNotes(){
    return notes.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES),notes.size());
}