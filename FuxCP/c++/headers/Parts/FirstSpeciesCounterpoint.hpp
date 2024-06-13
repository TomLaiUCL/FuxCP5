//
// Created by Damien Sprockeels on 11/06/2024.
//

#ifndef FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP
#define FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP

#include "Part.hpp"
#include "../Utilities.hpp"

class FirstSpeciesCounterpoint : public Part{
    protected:
        int motherSpecies;
        IntVarArray firstSpeciesNotesCp;
        IntVarArray firstSpeciesHarmonicIntervals;
        IntVarArray firstSpeciesMelodicIntervals;
    public:
        /// This constructor is for when the first species is called by another species
        FirstSpeciesCounterpoint(int size, vector<int> cf,int lb, int ub, int k, int mSpecies);
        ///This is the constructor for the first species on its own
        FirstSpeciesCounterpoint(int size, vector<int> cf,int lb, int ub, int k);

        string to_string() override;

        FirstSpeciesCounterpoint(FirstSpeciesCounterpoint &s);

        virtual Space *copy() override;
};


#endif //FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP
