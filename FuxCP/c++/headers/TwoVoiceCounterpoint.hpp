//
// Created by Damien Sprockeels on 12/06/2024.
//

#ifndef MYPROJECT_TWOVOICECOUNTERPOINT_HPP
#define MYPROJECT_TWOVOICECOUNTERPOINT_HPP

#include "Parts/Part.hpp"
#include "Parts/FirstSpeciesCounterpoint.hpp"
#include "Parts/CantusFirmus.hpp"
#include "Parts/SecondSpeciesCounterpoint.hpp"
#include "Utilities.hpp"


class TwoVoiceCounterpoint : public Space{
protected:
    int species;
    vector<int> cantusFirmusValues;
    int lowerBound;
    int upperBound;
    int key;

    Part* cp;
    Part* cf;
public:
    TwoVoiceCounterpoint(int sp, vector<int> cfv, int lb, int ub, int k);

    TwoVoiceCounterpoint(TwoVoiceCounterpoint &s);

    Space *copy() override;

    //todo add destructor for this that calls the part destructor
};


#endif //MYPROJECT_TWOVOICECOUNTERPOINT_HPP
