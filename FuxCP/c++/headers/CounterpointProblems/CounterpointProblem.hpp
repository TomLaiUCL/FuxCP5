#ifndef MYPROJECT_COUNTERPOINTPROBLEM_HPP
#define MYPROJECT_COUNTERPOINTPROBLEM_HPP

#include "../Utilities.hpp"
#include "../Parts/FirstSpeciesCounterpoint.hpp"
#include "../Parts/SecondSpeciesCounterpoint.hpp"
#include "../Parts/CantusFirmus.hpp"

/**
 * This (abstract) class gives a general model for a counterpoint problem. 
 */
class CounterpointProblem : public Space{
protected:
    CantusFirmus* cantusFirmus;
    // put one counterpoint? or group counterpoints together in a vector<Part*> ? or just leave it like this...

    int nMeasures;      /// the number of measures in the score to generate
    int key;            /// the key of the score
    int lowerBound;     /// the lowest note possible for the counterpoints
    int upperBound;     /// the highest note possible for the counterpoints
    Stratum* lowest;
    IntVarArray successiveCostArray;
    // vector<int> species;        /// the species of the counterpoint to generate

public:
    /**
     * Constructor of the class.
     * @param cf a vector<int> representing the cantus firmus.
     * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
     * @param lb the lowest note possible for the counterpoints in MIDI
     * @param ub the highest note possible for the counterpoints in MIDI
     */
    CounterpointProblem(vector<int> cf, int k, int lb, int ub, int v_type, vector<int> m_costs, vector<int> g_costs, int nV);

    CounterpointProblem(CounterpointProblem& s);
    virtual Space* copy(); 

    virtual string to_string() const;  

    /// Getters
    //todo add here getters if necessary

    ///destructor
    //todo release the allocated memory (each object created must be deleted)
    virtual ~CounterpointProblem(){ delete cantusFirmus; delete lowest; }  
};

#endif //MYPROJECT_COUNTERPOINTPROBLEM_HPP