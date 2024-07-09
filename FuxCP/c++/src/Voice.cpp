#include "../headers/Voice.hpp"

Voice::Voice(Home home, int nMes, int lb, int ub, int v_type, int sp){
    nMeasures = nMes; 
    size = nMes*4;

    voice_type = v_type;

    lowerBound = lb;
    upperBound = ub;

    species = sp;

    notes = IntVarArray(home, size, lowerBound, upperBound);
    h_intervals = IntVarArray(home, size, UNISSON, PERFECT_OCTAVE);
    m_intervals_brut = IntVarArray(home, size-4, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    
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
    species = s.species;
    voice_type = s.voice_type;
    notes.update(home, s.notes);
    h_intervals.update(home, s.h_intervals);
    isLowest.update(home, s.isLowest);
    m_intervals_brut.update(home, s.m_intervals_brut);
    motions.update(home, s.motions);
}

Voice* Voice::clone(Home home){
    return new Voice(home, *this);
}

IntVarArgs Voice::getFirstNotes(){
    if(species==CANTUS_FIRMUS){
        return notes.slice(0, 4/4, notes.size());
    } else {
        return notes.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES),notes.size());
    }
}

BoolVar Voice::getIsLowestIdx(int idx){
    return isLowest[idx];
}

IntVarArray Voice::getMelodicIntervals(){
    return m_intervals_brut;
}

IntVarArray Voice::getHInterval(){
    return h_intervals;
}

int Voice::getSpecies(){
    return species;
}