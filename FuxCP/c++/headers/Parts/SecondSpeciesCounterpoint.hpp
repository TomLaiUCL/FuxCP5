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
    public:
        SecondSpeciesCounterpoint(int size, vector<int> cf,int lb, int ub, int k);

        SecondSpeciesCounterpoint(FirstSpeciesCounterpoint &s);

        virtual Space *copy();
};


#endif //MYPROJECT_SECONDSPECIESCOUNTERPOINT_HPP
