#ifndef MYPROJECT_COUNTERPOINTPROBLEM_HPP
#define MYPROJECT_COUNTERPOINTPROBLEM_HPP

#include "../Utilities.hpp"
#include "../Parts/FirstSpeciesCounterpoint.hpp"
#include "../Parts/SecondSpeciesCounterpoint.hpp"
#include "../Parts/CantusFirmus.hpp"

/**
 * This (abstract) class gives a general model for a counterpoint problem. 
 */
class CounterpointProblem : public IntLexMinimizeSpace{
protected:
    CantusFirmus* cantusFirmus;
    // put one counterpoint? or group counterpoints together in a vector<Part*> ? or just leave it like this...

    int nMeasures;      /// the number of measures in the score to generate
    int key;            /// the key of the score
    int lowerBound;     /// the lowest note possible for the counterpoints
    int upperBound;     /// the highest note possible for the counterpoints
    Stratum* lowest;
    IntVarArray successiveCostArray;
    IntVarArray triadCostArray;
    Part* counterpoint_1;
    Part* counterpoint_2;
    Part* counterpoint_3;
    IntVarArray unitedCosts;
    IntVarArray solutionArray;
    // vector<int> species;        /// the species of the counterpoint to generate

public:
    /**
     * Constructor of the class.
     * @param cf a vector<int> representing the cantus firmus.
     * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
     * @param lb the lowest note possible for the counterpoints in MIDI
     * @param ub the highest note possible for the counterpoints in MIDI
     */
    CounterpointProblem(vector<int> cf, int k, int lb, int ub, int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int nV);

    CounterpointProblem(CounterpointProblem& s);
    virtual IntLexMinimizeSpace* copy(); 

    virtual string to_string() const;  

    /**
     * Constrain method for bab search
     * @todo modify this function if you want to use branch and bound
     * @param _b a space to constrain the current instance of the Problem class with upon finding a solution
     */
    virtual void constrain(const IntMinimizeSpace& _b);

    virtual IntVarArgs cost() const;

    /// Getters
    //todo add here getters if necessary
    Part* getCounterpoint(){ return counterpoint_1; }
    ///destructor
    //todo release the allocated memory (each object created must be deleted)
    virtual ~CounterpointProblem(){ delete cantusFirmus; delete lowest; delete counterpoint_1; delete counterpoint_2; delete counterpoint_3; }

    Home getHome();

};

#endif //MYPROJECT_COUNTERPOINTPROBLEM_HPP