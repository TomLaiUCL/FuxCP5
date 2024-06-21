//
// Created by Damien Sprockeels on 13/06/2024.
//

#include "../../headers/CounterpointProblems/TwoVoiceCounterpoint.hpp"
#include "../../headers/CounterpointUtils.hpp"

/**
 * Constructor of the class.
 * @param cf a vector<int> representing the cantus firmus.
 * @param sp the species of the counterpoint. it takes values from the enum "species" in headers/Utilities.hpp
 * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
 * @param lb the lowest note possible for the counterpoint in MIDI
 * @param ub the highest note possible for the counterpoint in MIDI
 */
TwoVoiceCounterpoint::TwoVoiceCounterpoint(vector<int> cf, int sp, int k, int lb, int ub) : CounterpointProblem(cf, k, lb, ub){
    species = sp;
    counterpoint = create_counterpoint(*this, species, nMeasures, cf, lowerBound, upperBound, key);
}

// COPY CONSTRUCTOR
TwoVoiceCounterpoint::TwoVoiceCounterpoint(TwoVoiceCounterpoint& s) : CounterpointProblem(s){
    if (s.counterpoint) {
        counterpoint = s.counterpoint->clone(*this);
    } else {
        counterpoint = nullptr;
    }
    species = s.species;
}

Space* TwoVoiceCounterpoint::copy(){   // todo use 'bool share' in copy constructor?
    return new TwoVoiceCounterpoint(*this);
}


string TwoVoiceCounterpoint::to_string() const {
    string text = "";
    text += CounterpointProblem::to_string();
    text += "TwoVoiceCounterpoint problem object : ";
    text += counterpoint->to_string();  // segmentation fault here was because counterpoint was deleted (but why did it still work when tostring was not virtual?)
    return text;
}