//
// Created by Damien Sprockeels on 13/06/2024.
//

#include "../../headers/CounterpointProblems/TwoVoiceCounterpoint.hpp"

/**
 * Constructor of the class.
 * @param cf a vector<int> representing the cantus firmus.
 * @param sp the species of the counterpoint. it takes values from the enum "species" in headers/Utilities.hpp
 * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
 * @param lb the lowest note possible for the counterpoint in MIDI
 * @param ub the highest note possible for the counterpoint in MIDI
 */
TwoVoiceCounterpoint::TwoVoiceCounterpoint(vector<int> cf, int sp, int k, int lb, int ub){
    cantusFirmus = cf;
    nMeasures = cf.size();
    key = k;
    lowerBound = lb;
    upperBound = ub;
    species = sp;

    switch (species) { /// call the appropriate constructor
        case FIRST_SPECIES:
            counterpoint = new FirstSpeciesCounterpoint(*this, nMeasures, cantusFirmus, lowerBound, upperBound, key);
            break;
        case SECOND_SPECIES:
            counterpoint = new SecondSpeciesCounterpoint(*this, nMeasures, cantusFirmus, lowerBound, upperBound, key);
            break;
        default:
            throw std::invalid_argument("Species not implemented");
    }

}

// COPY CONSTRUCTOR
TwoVoiceCounterpoint::TwoVoiceCounterpoint(TwoVoiceCounterpoint& s) : Space(s){
    cantusFirmus = s.cantusFirmus; 
    // counterpoint = s.counterpoint;  // just do it like this? counterpoint is a part, should we copy it?
    if (s.counterpoint) {
        counterpoint = s.counterpoint->clone(*this);
    } else {
        counterpoint = nullptr;
    }

    nMeasures = s.nMeasures; 
    key = s.key;
    lowerBound = s.lowerBound;
    upperBound = s.upperBound;
    species = s.species;
}

Space* TwoVoiceCounterpoint::copy(){   // todo use 'bool share' in copy constructor?
    return new TwoVoiceCounterpoint(*this);
}


string TwoVoiceCounterpoint::to_string() const {
    string text = counterpoint->to_string();  // segmentation fault here was because counterpoint was deleted (but why did it still work when tostring was not virtual?)
    return text;
}