//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../headers/TwoVoiceCounterpoint.hpp"

TwoVoiceCounterpoint::TwoVoiceCounterpoint(int sp, vector<int> cfv, int lb, int ub, int k){
    species = sp;
    cantusFirmusValues = cfv;
    lowerBound = lb;
    upperBound = ub;
    key = k;

    /// Create the cantus firmus
    cf = new CantusFirmus(cantusFirmusValues.size(), cantusFirmusValues, key);
    /// Create the counterpoint
    cp = new FirstSpeciesCounterpoint(cantusFirmusValues.size(), cantusFirmusValues, lowerBound, upperBound, key, species);

    /// post branchings here so they can be adapted based on species and number of voices. It is more free than
    /// branch on cf first because it is determined
}

TwoVoiceCounterpoint::TwoVoiceCounterpoint(TwoVoiceCounterpoint &s){
species = s.species;
    cantusFirmusValues = s.cantusFirmusValues;
    lowerBound = s.lowerBound;
    upperBound = s.upperBound;
    key = s.key;

    cf = (Part*) cf->copy();
    cp = (Part*) cp->copy();
}

Space* TwoVoiceCounterpoint::copy() {
    return new TwoVoiceCounterpoint(*this);
}