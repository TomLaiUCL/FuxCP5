//
// Created by Damien Sprockeels on 11/06/2024.
//

#ifndef FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP
#define FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP

#include "Counterpoint.hpp"
#include "Utilities.hpp"

class FirstSpeciesCounterpoint : public Counterpoint{
    protected:
        int motherSpecies;
    public:
        FirstSpeciesCounterpoint(vector<int> cf,int lb, int ub, int k, int mSpecies);

        FirstSpeciesCounterpoint(FirstSpeciesCounterpoint &s);

        virtual Space *copy();
};


#endif //FUXCP_BASE_FIRSTSPECIESCOUNTERPOINT_HPP
