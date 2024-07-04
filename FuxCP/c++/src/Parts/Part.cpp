//
// Created by Damien Sprockeels on 11/06/2024.
//

#include "../../headers/Parts/Part.hpp"

/// This class represents a part, so it creates all the variables associated to that part and posts the constraints that are species independent
Part::Part(Home hme, int nMes, int sp, vector<int> cf, int lb, int ub, int k, Stratum* low, int v_type) : Voice(hme, nMes, lb, ub, v_type){
    species         = sp;
    key             = k;
    lowest          = low;
    isLowest        = BoolVarArray(home, nMeasures);

    borrowed_scale = get_all_notes_from_scale(key, BORROWED_SCALE);
    scale = get_all_notes_from_scale(key, MAJOR_SCALE);
    chromatic_scale = get_all_notes_from_scale(key, CHROMATIC_SCALE);
    cp_range = {};

//    mIntervalsCp        = IntVarArray(home, nMeasures-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);

    // /// link harmonic intervals
    // for (int i = 0; i < hIntervalsCpCf.size(); i++){
    //     /// i // 4 because the ctp has 4 notes per measure and the Cf 1
    //     rel(home, expr(home, abs(cp[i] - cf_vector[floor(i / 4)])), IRT_EQ, hIntervalsCpCf[i]); /// absolute value is rule G1 from Thibault
    // }
    /// link melodic intervals
//    for(int i = 0; i < nMeasures-1; i++){
//        rel(home, mIntervalsCp[i], IRT_EQ, expr(home, cp[i+1] - cp[i]));
//    }
    //todo link motions using the enum in headers/Utilities.hpp

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///                                                General Constraints                                           ///
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// rule G4 from Thibault: The counterpoint has to be in the key of the Cantus Firmus todo move this to the parent class in a function
    dom(home, notes, IntSet(IntArgs(get_all_notes_from_scale(key, MAJOR_SCALE)))); //@todo do it correctly, this is just an example

}

string Part::to_string() const{
    string part = "Part characteristics :\n";
    part += "Part notes : \n";
    part += intVarArray_to_string(notes);
    part += "\n";
    return part;
}




Part::Part(Part& s) : Voice(s) {
    species = s.species;
    key = s.key;
    lowest = s.lowest;

    borrowed_scale = s.borrowed_scale;
    scale = s.scale;
    chromatic_scale = s.chromatic_scale;

    cp_range = s.cp_range;

    extended_domain = s.extended_domain;
    off_domain = s.off_domain;

    isLowest.update(home, s.isLowest);

//    mIntervalsCp.update(home, s.mIntervalsCp);
//    motionsCfCp.update(home, s.motionsCfCp);
}

// Virtual clone function
Part* Part::copy() {
    return new Part(*this);
}

BoolVar Part::getIsLowestIdx(int idx){
    return isLowest[idx];
}

IntVarArray Part::getBranchingNotes(){
    return notes;
}