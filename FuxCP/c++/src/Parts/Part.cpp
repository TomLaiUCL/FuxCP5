//
// Created by Damien Sprockeels on 11/06/2024.
//

#include "../../headers/Parts/Part.hpp"

/// This class represents a part, so it creates all the variables associated to that part and posts the constraints that are species independent
Part::Part(Home home, int nMes, int sp, vector<int> cf, int lb, int ub, int k) {
    nMeasures       = nMes;
    size            = nMes * 4;
    species         = sp;
    key             = k;

    lowerBound      = lb;/// rule G5 from Thibault
    upperBound      = ub;

    cf_vector    = cf;
    /// compute melodic intervals for the Cf
    for (int i = 0; i < nMeasures-1; i++)
        melodicIntervalsCf.push_back(cf_vector[i+1] - cf_vector[i]);

    cp                  = IntVarArray(home, size, lowerBound, upperBound); /// rule G2 from Thibault (rule G3 is implicit)
    hIntervalsCpCf      = IntVarArray(home, size, UNISSON, PERFECT_OCTAVE);
//    mIntervalsCp        = IntVarArray(home, nMeasures-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
//    motionsCfCp         = IntVarArray(home, nMeasures-1, IntSet(IntArgs({CONTRARY_MOTION, PARALLEL_MOTION, OBLIQUE_MOTION})));

    /// link harmonic intervals
    for (int i = 0; i < hIntervalsCpCf.size(); i++){
        /// i // 4 because the ctp has 4 notes per measure and the Cf 1
        rel(home, expr(home, abs(cp[i] - cf_vector[floor(i / 4)])), IRT_EQ, hIntervalsCpCf[i]); /// absolute value is rule G1 from Thibault
    }
    /// link melodic intervals
//    for(int i = 0; i < nMeasures-1; i++){
//        rel(home, mIntervalsCp[i], IRT_EQ, expr(home, cp[i+1] - cp[i]));
//    }
    //todo link motions using the enum in headers/Utilities.hpp

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///                                                General Constraints                                           ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// rule G4 from Thibault: The counterpoint has to be in the key of the Cantus Firmus todo move this to the parent class in a function
    dom(home, cp, IntSet(IntArgs(get_all_notes_from_scale(key, MAJOR_SCALE)))); //@todo do it correctly, this is just an example

}

string Part::to_string() const{
    string part = "Part characteristics :\n";
    part += "Cp array: " + intVarArray_to_string(cp) + "\n";
    part += "Harmonic intervals array: " + intVarArray_to_string(hIntervalsCpCf) + "\n";
    return part;
}

IntVarArray Part::getBranchingNotes(){
    return cp;
}



Part::Part(Home home, Part& s){
    nMeasures = s.nMeasures;
    species = s.species;
    key = s.key;

    lowerBound = s.lowerBound;
    upperBound = s.upperBound;

    cf_vector = s.cf_vector;
    melodicIntervalsCf = s.melodicIntervalsCf;

    cp.update(home, s.cp);
    hIntervalsCpCf.update(home, s.hIntervalsCpCf);
//    mIntervalsCp.update(home, s.mIntervalsCp);
//    motionsCfCp.update(home, s.motionsCfCp);
}

// Virtual clone function
Part* Part::clone(Home home) {
    return new Part(home, *this);
}