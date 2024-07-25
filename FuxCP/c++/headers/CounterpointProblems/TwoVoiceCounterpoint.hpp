//
// Created by Damien Sprockeels on 13/06/2024.
//

#ifndef MYPROJECT_TWOVOICECOUNTERPOINT_HPP
#define MYPROJECT_TWOVOICECOUNTERPOINT_HPP

#include "../Utilities.hpp"
#include "CounterpointProblem.hpp"
#include "../Parts/FirstSpeciesCounterpoint.hpp"
#include "../Parts/SecondSpeciesCounterpoint.hpp"
#include "../Parts/CantusFirmus.hpp"

/**
 * This class models a counterpoint problem with 2 voices.
 */
class TwoVoiceCounterpoint : public CounterpointProblem{
protected:
    Stratum* upper;

    int species;        /// the species of the counterpoint to generate

public:
    /**
     * Constructor of the class.
     * @param cf a vector<int> representing the cantus firmus.
     * @param sp the species of the counterpoint. it takes values from the enum "species" in headers/Utilities.hpp
     * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
     * @param lb the lowest note possible for the counterpoint in MIDI
     * @param ub the highest note possible for the counterpoint in MIDI
     */
    TwoVoiceCounterpoint(vector<int> cf, int sp, int k, int lb, int ub, int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, vector<int> imp, int bm);

    TwoVoiceCounterpoint(TwoVoiceCounterpoint& s);
    IntLexMinimizeSpace* copy() override; 

    string to_string() const override; 

    /// Getters
    
    //todo add here other getters if necessary

    ///destructor
    //todo release the allocated memory (each object created must be deleted) WILL THE COUNTERPOINTPROBLEM DESTRUCTOR BE AUTOMATICALLY CALLED?
    ~TwoVoiceCounterpoint() { delete upper; }  

    void uniteCosts();
};

#endif //MYPROJECT_TWOVOICECOUNTERPOINT_HPP