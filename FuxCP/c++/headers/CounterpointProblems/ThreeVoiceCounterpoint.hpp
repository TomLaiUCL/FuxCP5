#ifndef MYPROJECT_THREEVOICECOUNTERPOINT_HPP
#define MYPROJECT_THREEVOICECOUNTERPOINT_HPP

#include "../Utilities.hpp"
#include "../Parts/FirstSpeciesCounterpoint.hpp"
#include "../Parts/SecondSpeciesCounterpoint.hpp"

/**
 * This class models a counterpoint problem with 3 voices.
 */
class ThreeVoiceCounterpoint : public Space{
protected:
    vector<int> cantusFirmus;
    Part* counterpoint_1;
    Part* counterpoint_2;

    int nMeasures;      /// the number of measures in the score to generate
    int key;            /// the key of the score
    int lowerBound;     /// the lowest note possible for the counterpoints
    int upperBound;     /// the highest note possible for the counterpoints
    vector<int> species;        /// the species of the counterpoints to generate

public:
    /**
     * Constructor of the class.
     * @param cf a vector<int> representing the cantus firmus.
     * @param sp the species of the counterpoint. it takes values from the enum "species" in headers/Utilities.hpp
     * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
     * @param lb the lowest note possible for the counterpoint in MIDI
     * @param ub the highest note possible for the counterpoint in MIDI
     */
    ThreeVoiceCounterpoint(vector<int> cf, vector<int> sp, int k, int lb, int ub);

    ThreeVoiceCounterpoint(ThreeVoiceCounterpoint& s);
    virtual Space* copy(); // pq virtual?

    string to_string() const;  // pq c'etait virtual?

    /// Getters
    // Part* getCounterpoint(){ return counterpoint; }
    //todo add here other getters if necessary

    ///destructor
    //todo release the allocated memory (each object created must be deleted)
    ~ThreeVoiceCounterpoint(){ 
        delete counterpoint_1;
        delete counterpoint_2; 
    }  
};

#endif //MYPROJECT_THREEVOICECOUNTERPOINT_HPP