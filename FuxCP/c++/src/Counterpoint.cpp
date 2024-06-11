//
// Created by Damien Sprockeels on 11/06/2024.
//

#include "../headers/Counterpoint.hpp"

Counterpoint::Counterpoint(int sp, vector<int> cf, int lb, int ub, int k) {
    nMeasures = cf.size();
    species = sp;
    key = k;

    lowerBound = lb;/// rule G5 from Thibault
    upperBound = ub;

    cantusFirmus = cf;
    /// compute melodic intervals
    for (int i = 0; i < nMeasures-1; i++)
        melodicIntervalsCf.push_back(cantusFirmus[i+1] - cantusFirmus[i]);
}

/// rule G6 from Thibault
void Counterpoint::noChromaticMelodies() {
    for(int i = 0; i < mIntervalsCp.size()-1; i++){
        rel(*this, expr(*this, mIntervalsCp[i] == 1), BOT_IMP, expr(*this, mIntervalsCp[i+1] != mIntervalsCp[i]), true); /// mCp[i] == 1 => mCp[i+1] != mCp[i]
        rel(*this, expr(*this, mIntervalsCp[i] == - 1), BOT_IMP, expr(*this, mIntervalsCp[i+1] != mIntervalsCp[i]), true); /// mCp[i] == -1 => mCp[i+1] != mCp[i]
    }
}

string Counterpoint::to_string(){
    return intVarArray_to_string(cp);
}

Counterpoint::Counterpoint(Counterpoint& s) : Space(s){
    nMeasures = s.nMeasures;
    species = s.species;
    key = s.key;

    lowerBound = s.lowerBound;
    upperBound = s.upperBound;

    cp.update(*this, s.cp);
    hIntervalsCpCf.update(*this, s.hIntervalsCpCf);
    mIntervalsCp.update(*this, s.mIntervalsCp);

    cantusFirmus = s.cantusFirmus;
    melodicIntervalsCf = s.melodicIntervalsCf;
}

Space* Counterpoint::copy() {
    return new Counterpoint(*this);
}
