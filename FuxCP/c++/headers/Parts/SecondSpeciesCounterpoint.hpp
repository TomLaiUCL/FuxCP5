//
// Created by Damien Sprockeels on 12/06/2024.
//

#ifndef MYPROJECT_SECONDSPECIESCOUNTERPOINT_HPP
#define MYPROJECT_SECONDSPECIESCOUNTERPOINT_HPP


#include "FirstSpeciesCounterpoint.hpp"
#include "../Utilities.hpp"

///This class extends the first species class because most rules of the first species are also valid for the second species
/**
 * This class represents a counterpoint of the first species. It inherits from the FirstSpeciesCounterpoint class.
 * todo modify it so it also works for 3 and 4 voices. Add the appropriate constraints by making a constructor that takes the number of voices as a parameter
 * todo maybe it should take a Stratum (object or just IntVarArray) for the lowest voice or something like that depending on the formalization
 */
class SecondSpeciesCounterpoint : public FirstSpeciesCounterpoint {
protected:
    IntVarArray secondSpeciesNotesCp;               /// The notes of the counterpoint that have to follow the rules for the 2nd species
    IntVarArray secondSpeciesHarmonicIntervals;     /// The harmonic intervals between the notes that have to follow the 2nd species rules and the cantus firmus
    IntVarArray secondSpeciesMelodicIntervals;      /// The melodic intervals between the notes that have to follow the 2nd species rules
public:
    /**
     * 
     * @param size
     * @param cf
     * @param lb
     * @param ub
     * @param k
     */
    SecondSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type);

    string to_string() const override;

    // SecondSpeciesCounterpoint(SecondSpeciesCounterpoint &s);    No longer a copy, now a clone (since it is not a space, and copy constructor are for spaces, while cloning functions are just used to create a deep copy of an object)

    // virtual Space *copy() override;

    SecondSpeciesCounterpoint(SecondSpeciesCounterpoint &s); // clone constructor
    SecondSpeciesCounterpoint* copy() override; 

};


#endif //MYPROJECT_SECONDSPECIESCOUNTERPOINT_HPP