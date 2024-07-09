#ifndef MYPROJECT_THREEVOICECOUNTERPOINT_HPP
#define MYPROJECT_THREEVOICECOUNTERPOINT_HPP

#include "../Utilities.hpp"
#include "CounterpointProblem.hpp"
#include "../Parts/FirstSpeciesCounterpoint.hpp"
#include "../Parts/SecondSpeciesCounterpoint.hpp"
#include "../Parts/CantusFirmus.hpp"

/**
 * This class models a counterpoint problem with 3 voices.
 */
class ThreeVoiceCounterpoint : public CounterpointProblem{
protected:
    Part* counterpoint_1;
    Part* counterpoint_2;

    Stratum* upper1;
    Stratum* upper2;

    vector<int> species;        /// the species of the counterpoints to generate
    IntVarArray solutionArray;
    IntVarArray unitedCosts;

public:
    /**
     * Constructor of the class.
     * @param cf a vector<int> representing the cantus firmus.
     * @param sp the species of the counterpoint. it takes values from the enum "species" in headers/Utilities.hpp
     * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
     * @param lb the lowest note possible for the counterpoint in MIDI
     * @param ub the highest note possible for the counterpoint in MIDI
     */
    ThreeVoiceCounterpoint(vector<int> cf, vector<int> sp, int k, int lb, int ub, vector<int> v_type, vector<int> m_costs, vector<int> g_costs);

    ThreeVoiceCounterpoint(ThreeVoiceCounterpoint& s);
    Space* copy() override; 

    string to_string() const override;

    /// Getters
    // Part* getCounterpoint(){ return counterpoint; }
    //todo add here other getters if necessary

    ///destructor
    //todo release the allocated memory (each object created must be deleted)
    ~ThreeVoiceCounterpoint(){ 
        delete counterpoint_1;
        delete counterpoint_2; 
        delete upper1;
        delete upper2;
    }  

    void uniteCounterpoints();

    void uniteCosts();
};

#endif //MYPROJECT_THREEVOICECOUNTERPOINT_HPP