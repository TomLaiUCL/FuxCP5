//
// Created by Damien Sprockeels on 11/06/2024.
//

#include "../headers/Part.hpp"

/// This class represents a part, so it creates all the variables associated to that part and posts the constraints that are species independent
Part::Part(int nMes, int sp, vector<int> cf, int lb, int ub, int k) {
    nMeasures       = nMes;
    size            = nMes * 4;
    species         = sp;
    key             = k;

    lowerBound      = lb;/// rule G5 from Thibault
    upperBound      = ub;

    cantusFirmus    = cf;
    /// compute melodic intervals for the Cf
    for (int i = 0; i < nMeasures-1; i++)
        melodicIntervalsCf.push_back(cantusFirmus[i+1] - cantusFirmus[i]);

    cp                  = IntVarArray(*this, size, lowerBound, upperBound); /// rule G2 from Thibault (rule G3 is implicit)
    hIntervalsCpCf      = IntVarArray(*this, size, UNISSON, PERFECT_OCTAVE);
//    mIntervalsCp        = IntVarArray(*this, nMeasures-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
//    motionsCfCp         = IntVarArray(*this, nMeasures-1, IntSet(IntArgs({CONTRARY_MOTION, PARALLEL_MOTION, OBLIQUE_MOTION})));

    /// link harmonic intervals
    for (int i = 0; i < hIntervalsCpCf.size(); i++){
        /// i // 4 because the ctp has 4 notes per measure and the Cf 1
        rel(*this, expr(*this, abs(cp[i] - cantusFirmus[floor(i / 4)])), IRT_EQ, hIntervalsCpCf[i]); /// absolute value is rule G1 from Thibault
    }
    /// link melodic intervals
//    for(int i = 0; i < nMeasures-1; i++){
//        rel(*this, mIntervalsCp[i], IRT_EQ, expr(*this, cp[i+1] - cp[i]));
//    }
    //todo link motions using the enum in headers/Utilities.hpp

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///                                                General Constraints                                           ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// rule G4 from Thibault: The counterpoint has to be in the key of the Cantus Firmus todo move this to the parent class in a function
    dom(*this, cp, IntSet(IntArgs(get_all_notes_from_scale(key, MAJOR_SCALE)))); //@todo do it correctly, this is just an example

}

string Part::to_string(){
    string part = "Part characteristics :\n";
    part += "Cp array: " + cleanIntVarArray_to_string(cp) + "\n";
    part += "Harmonic intervals array: " + cleanIntVarArray_to_string(hIntervalsCpCf) + "\n";
    return part;
}

Part::Part(Part& s) : Space(s){
    nMeasures = s.nMeasures;
    species = s.species;
    key = s.key;

    lowerBound = s.lowerBound;
    upperBound = s.upperBound;

    cantusFirmus = s.cantusFirmus;
    melodicIntervalsCf = s.melodicIntervalsCf;

    cp.update(*this, s.cp);
    hIntervalsCpCf.update(*this, s.hIntervalsCpCf);
    mIntervalsCp.update(*this, s.mIntervalsCp);
    motionsCfCp.update(*this, s.motionsCfCp);
}

Space* Part::copy() {
    return new Part(*this);
}