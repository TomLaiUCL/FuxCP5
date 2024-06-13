//
// Created by Damien Sprockeels on 12/06/2024.
//

#ifndef MYPROJECT_SECONDSPECIESCOUNTERPOINT_HPP
#define MYPROJECT_SECONDSPECIESCOUNTERPOINT_HPP


#include "FirstSpeciesCounterpoint.hpp"
#include "../Utilities.hpp"

///This class extends the first species class because most rules of the first species are also valid for the second species
class SecondSpeciesCounterpoint : public FirstSpeciesCounterpoint {
protected:
    IntVarArray secondSpeciesNotesCp;
    IntVarArray secondSpeciesHarmonicIntervals;
    IntVarArray secondSpeciesMelodicIntervals;
public:
    SecondSpeciesCounterpoint(int size, vector<int> cf,int lb, int ub, int k);

    string to_string() override;

    SecondSpeciesCounterpoint(SecondSpeciesCounterpoint &s);

    virtual Space *copy() override;
};


#endif //MYPROJECT_SECONDSPECIESCOUNTERPOINT_HPP