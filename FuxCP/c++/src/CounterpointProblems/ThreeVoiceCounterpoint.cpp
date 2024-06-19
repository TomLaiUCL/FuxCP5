#include "../../headers/CounterpointProblems/ThreeVoiceCounterpoint.hpp"
#include "../../headers/CounterpointUtils.hpp"

/**
 * Constructor of the class.
 * @param cf a vector<int> representing the cantus firmus.
 * @param sp the species of the counterpoint. it takes values from the enum "species" in headers/Utilities.hpp
 * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
 * @param lb the lowest note possible for the counterpoint in MIDI
 * @param ub the highest note possible for the counterpoint in MIDI
 */
ThreeVoiceCounterpoint::ThreeVoiceCounterpoint(vector<int> cf, vector<int> sp, int k, int lb, int ub){
    cantusFirmus = cf;
    nMeasures = cf.size();
    key = k;
    lowerBound = lb;
    upperBound = ub;
    species = sp;

    counterpoint_1 = create_counterpoint(*this, species, nMeasures, cantusFirmus, lowerBound, upperBound, key);
    counterpoint_2 = create_counterpoint(*this, species, nMeasures, cantusFirmus, lowerBound, upperBound, key);


}

// COPY CONSTRUCTOR
ThreeVoiceCounterpoint::ThreeVoiceCounterpoint(ThreeVoiceCounterpoint& s) : Space(s){
    cantusFirmus = s.cantusFirmus; 
    if (s.counterpoint_1) {
        counterpoint_1 = s.counterpoint_1->clone(*this);
    } else {
        counterpoint_1 = nullptr;
    }

    if (s.counterpoint_2) {
        counterpoint_2 = s.counterpoint_2->clone(*this);
    } else {
        counterpoint_2 = nullptr;
    }

    nMeasures = s.nMeasures; 
    key = s.key;
    lowerBound = s.lowerBound;
    upperBound = s.upperBound;
    species = s.species;
}

Space* ThreeVoiceCounterpoint::copy(){  
    return new ThreeVoiceCounterpoint(*this);
}


string ThreeVoiceCounterpoint::to_string() const {
    string text = "counterpoint 1 : \n";
    text += counterpoint_1->to_string();  
    text += "counterpoint 2 : \n";
    text += counterpoint_2->to_string();  
    return text;
}