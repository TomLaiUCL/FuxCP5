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
TwoVoiceCounterpoint::TwoVoiceCounterpoint(vector<int> cf, int sp, int k, int lb, int ub, int v_type, vector<int> m_costs, vector<int> g_costs, int bm) : 
    CounterpointProblem(cf, k, lb, ub, v_type, m_costs, g_costs, TWO_VOICES){
    species = sp;
    upper = new Stratum(*this, nMeasures, 0, 127, v_type, lowest->getNotes()); 
    counterpoint = create_counterpoint(*this, species, nMeasures, cf, lowerBound, upperBound, key, lowest, cantusFirmus, v_type, m_costs, g_costs, bm, TWO_VOICES);

    /// H2 from Thibault: The first harmonic interval must be a perfect consonance
    dom(*this, counterpoint->getHInterval()[0], IntSet(IntArgs(PERFECT_CONSONANCES)));

    /// H3 from Thibault: The last harmonic interval must be a perfect consonance
    dom(*this, counterpoint->getHInterval()[counterpoint->getHInterval().size()-1], IntSet(IntArgs(PERFECT_CONSONANCES)));

    //rel(*this, counterpoint->getFirstNotes()[2], IRT_EQ, 76);
    //create_lowest(*this, lowest, cantusFirmus, counterpoint);
    lowest->setCantusPointer(cantusFirmus);
    lowest->setCpPointer(*this, counterpoint);
    lowest->setLowest(*this, upper);

    //upper->setNote(*this, 5, counterpoint->getFirstNotes()[5]);
    branch(*this, counterpoint->getBranchingNotes(), INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}
// COPY CONSTRUCTOR
TwoVoiceCounterpoint::TwoVoiceCounterpoint(TwoVoiceCounterpoint& s) : CounterpointProblem(s){
    //counterpoint = s.counterpoint;
    species = s.species;
    if (s.counterpoint) {
        counterpoint = s.counterpoint->clone(*this);
    } else {
        counterpoint = nullptr;
    }
    if (s.upper) {
        upper = s.upper->clone(*this);
    } else {
        upper = nullptr;
    }
}

Space* TwoVoiceCounterpoint::copy(){   // todo use 'bool share' in copy constructor?
    return new TwoVoiceCounterpoint(*this);
}


string TwoVoiceCounterpoint::to_string() const {
    string text = "";
    text += CounterpointProblem::to_string();
    text += "TwoVoiceCounterpoint problem object : \n";
    text += "Cantus Firmus : \n" + cantusFirmus->to_string() + "\n";
    text += "Counterpoint : \n" +counterpoint->to_string() + "\n";  // segmentation fault here was because counterpoint was deleted (but why did it still work when tostring was not virtual?)
    text += "Lowest : \n" +lowest->to_string() + "\n";
    text += "Upper : \n" +upper->to_string() + "\n";
    return text;
}