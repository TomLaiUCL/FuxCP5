//
// Created by Damien Sprockeels on 11/06/2024.
//

#ifndef FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP
#define FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP

#include "Part.hpp"
#include "../Utilities.hpp"
#include "CantusFirmus.hpp"

/**
 * This class represents a counterpoint of the first species. It inherits from the Part class.
 * todo modify it so it also works for 3 and 4 voices. Add the appropriate constraints by making a constructor that takes the number of voices as a parameter
 * todo maybe it should take a Stratum (object or just IntVarArray) for the lowest voice or something like that depending on the formalization
 */
class FirstSpeciesCounterpoint : public Part{
protected:
    int motherSpecies;                          /// The species from which this is called.
    int lengthCp1stSpecies;                     /// The number of notes in the counterpoint
    CantusFirmus* cantus;
    IntVarArray firstSpeciesNotesCp;            /// The notes of the counterpoint that have to follow the rules for the first species
    IntVarArray firstSpeciesHarmonicIntervals;  /// The harmonic intervals between the notes that have to follow the 1st species rules and the lowest stratum
    IntVarArray firstSpeciesMelodicIntervals;   /// The melodic intervals between the notes that have to follow the 1st species rules
public:
    /**
     * General constructor. It takes the mother species as an argument and calls the super constructor from the part class.
     * Additionally, it sets all the 1st species specific variables as well as the general rules that have to be applied
     * regardless of the species. It does not apply 1st species specific rules and does not post branching.
     * todo maybe add a Stratum object or IntVarArray for the lowest voice
     * @param nMes the number of measures in the composition
     * @param cf the cantus firmus todo maybe it should be a CantusFirmusObject
     * @param lb the lower bound for the counterpoint
     * @param ub the upper bound for the counterpoint
     * @param k the key of the composition
     * @param mSpecies the species from which this is called.
     */
    FirstSpeciesCounterpoint(Home  home, int nMes, vector<int> cf, int lb, int ub, int k, int mSpecies, Stratum* low, CantusFirmus* c,  int v_type);

    /**
     * This constructor is only used when creating a counterpoint of the first species. It calls the other constructor with
     * FIRST_SPECIES as the mother species. Additionally, it posts 1st species specific constraints as well as the branching.
     * @param nMes the number of measures in the composition
     * @param cf the cantus firmus todo maybe it should be a CantusFirmusObject
     * @param lb the lower bound for the counterpoint
     * @param ub the upper bound for the counterpoint
     * @param k the key of the composition
     */
    FirstSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, Stratum* low, CantusFirmus* c,  int v_type);

    /// Getters
    int getLengthCp1stSpecies() {return lengthCp1stSpecies;};

    /**
     * This function returns a string with the characteristics of the counterpoint. It calls the to_string() method from
     * the Part class and adds 1st species specific characteristics.
     * @return a string representation of the current instance of the FirstSpeciesCounterpoint class.
     */
    string to_string() const override;

    /// Copy constructor. This needs to copy all useful attributes and update variables. Must call the super copy constructor       NO LONGER NEEDED
    // FirstSpeciesCounterpoint(FirstSpeciesCounterpoint &s);
    /// Copy function
    // virtual Space *copy() override;

    FirstSpeciesCounterpoint(FirstSpeciesCounterpoint& s); // clone constructor
    FirstSpeciesCounterpoint* copy() override;

    virtual IntVarArray getBranchingNotes() override;

};


#endif //FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP