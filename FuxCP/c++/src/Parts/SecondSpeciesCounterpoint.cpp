//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../../headers/Parts/SecondSpeciesCounterpoint.hpp"

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type
    , vector<int> m_costs, vector<int> g_costs, int bm, int nV):
    FirstSpeciesCounterpoint(home, size, cf, lb, ub, k, SECOND_SPECIES, low, c, v_type, m_costs, g_costs, bm, nV) /// super constructor. Applies all rules for the first species to the 1st note of each measure
{
    /// Second species notes in the counterpoint
    secondSpeciesNotesCp = IntVarArray(home, (nMeasures*notesPerMeasure.at(SECOND_SPECIES))-1, IntSet(IntArgs(extended_domain)));
    rel(home, secondSpeciesNotesCp, IRT_EQ, notes.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),(notes.size())));
    if(borrowMode==1){
        secondSpeciesNotesCp[secondSpeciesNotesCp.size()-2] = IntVar(home, IntSet(IntArgs(vector_intersection(cp_range, chromatic_scale))));
    }

    /// Harmonic intervals for the second species notes
    secondSpeciesHarmonicIntervals = IntVarArray(home, (nMeasures*notesPerMeasure.at(SECOND_SPECIES))-1, UNISSON, PERFECT_OCTAVE);
    rel(home, secondSpeciesHarmonicIntervals, IRT_EQ, h_intervals.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),(h_intervals.size())));

    /// Melodic intervals for the second species notes
    secondSpeciesMelodicIntervals = IntVarArray(home, (nMeasures*notesPerMeasure.at(SECOND_SPECIES) -1)-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    /// link melodic intervals
    for(int i = 0; i < secondSpeciesMelodicIntervals.size(); i++)
        rel(home, secondSpeciesMelodicIntervals[i], IRT_EQ, expr(home, secondSpeciesNotesCp[i+1] - secondSpeciesNotesCp[i]));

    secondSpeciesMotions = IntVarArray(home, (nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1)-1, IntSet{-1, CONTRARY_MOTION, OBLIQUE_MOTION, PARALLEL_MOTION});
    secondSpeciesMotionCosts = IntVarArray(home, (nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1)-1, IntSet{0, directCost, obliqueCost, contraryCost});

    //the second motions are between the two arsis notes as are the first motions between thesis notes. this way of doing it takes the correct
    //m intervals between two arsis notes and then calculates the motion with respect to the cantusFirmus
    for(int i = 1; i < secondSpeciesMelodicIntervals.size()-1; i+=2){

        IntVar arsisInterval = expr(home, secondSpeciesMelodicIntervals[i]+secondSpeciesMelodicIntervals[i+1]);
        //direct motions help creation
        BoolVar both_up = expr(home, (arsisInterval>0)&&(low->getMelodicIntervals()[floor(i/2)]>0)); //if both parts are going in the same direction
        BoolVar both_stay = expr(home, (arsisInterval==0)&&(low->getMelodicIntervals()[floor(i/2)]==0)); //if both parts are staying
        BoolVar both_down = expr(home, (arsisInterval<0)&&(low->getMelodicIntervals()[floor(i/2)]<0)); //if both parts are going down
        //oblique motions help creation
        BoolVar cf_stays_1 = expr(home, (arsisInterval>0)&&(low->getMelodicIntervals()[floor(i/2)]==0)); //if the lowest part stays and one goes up
        BoolVar cf_stays_2 = expr(home, (arsisInterval<0)&&(low->getMelodicIntervals()[floor(i/2)]==0)); //if the lowest part stays and one goes down
        BoolVar cp_stays_1 = expr(home, (arsisInterval==0)&&(low->getMelodicIntervals()[floor(i/2)]>0)); //if the lowest part goes up and one stays
        BoolVar cp_stays_2 = expr(home, (arsisInterval==0)&&(low->getMelodicIntervals()[floor(i/2)]<0)); //if the lowest part goes down and one stays
        //contrary motions help creation
        BoolVar cpd_cfu = expr(home, (arsisInterval<0)&&(low->getMelodicIntervals()[floor(i/2)]>0)); //if the cf goes up and the cp down
        BoolVar cpu_cfd = expr(home, (arsisInterval>0)&&(low->getMelodicIntervals()[floor(i/2)]<0)); //if the cf goes down and the cp up

        //direct constraints
        rel(home, ((both_up || both_stay || both_down)) >> (secondSpeciesMotions[floor(i/2)]==PARALLEL_MOTION));
        rel(home, ((both_up || both_stay || both_down)) >> (secondSpeciesMotionCosts[floor(i/2)]==directCost));
        //oblique constraints
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2)) >> (secondSpeciesMotions[floor(i/2)]==OBLIQUE_MOTION));
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2)) >> (secondSpeciesMotionCosts[floor(i/2)]==obliqueCost));
        //contrary constraints
        rel(home, ((cpd_cfu || cpu_cfd)) >> (secondSpeciesMotions[floor(i/2)]==CONTRARY_MOTION));
        rel(home, ((cpd_cfu || cpu_cfd)) >> (secondSpeciesMotionCosts[floor(i/2)]==contraryCost));
        //bass constraints
        //BoolVar is_lowest = expr(*this, isCfArray[i]==0);
        //rel(*this, secondSpeciesMotions[i], IRT_EQ, -1, Reify(is_lowest, RM_IMP));
        //rel(*this, secondSpeciesMotionCosts[i], IRT_EQ, 0, Reify(is_lowest, RM_IMP));
    }

    //create real motions. we have the thesis-thesis and arsis-arsis motions. now we need the successive m intervals in meas, meaning thesis-arsis inside
    //a measure. to get this, we iterate in increments of 2 starting from i=0
    secondSpeciesRealMotions = IntVarArray(home, secondSpeciesMotions.size(), IntSet{-1, CONTRARY_MOTION, OBLIQUE_MOTION, PARALLEL_MOTION});
    secondSpeciesRealMotionCosts = IntVarArray(home, secondSpeciesRealMotions.size(), IntSet{0, directCost, obliqueCost, contraryCost});
    for(int i = 0; i < secondSpeciesRealMotions.size(); i++){
        cout << i << endl;
            rel(home, (expr(home, abs(secondSpeciesMelodicIntervals[i*2])>4)==1) >> (secondSpeciesRealMotions[i]==secondSpeciesMotions[i]));
            rel(home, (expr(home, abs(secondSpeciesMelodicIntervals[i*2])>4)==0) >> (secondSpeciesRealMotions[i]==firstSpeciesMotions[i]));

            rel(home, (expr(home, abs(secondSpeciesMelodicIntervals[i*2])>4)==1) >> (secondSpeciesRealMotionCosts[i]==secondSpeciesMotionCosts[i]));
            rel(home, (expr(home, abs(secondSpeciesMelodicIntervals[i*2])>4)==0) >> (secondSpeciesRealMotionCosts[i]==firstSpeciesMotionCosts[i]));
    }
    /// Constraints

    /// 2.M2: Two consecutive notes cannot be the same
    rel(home, secondSpeciesMelodicIntervals, IRT_NQ, 0); /// plus efficace que de faire cp[i] /= cp[i+1]
}

string SecondSpeciesCounterpoint::to_string() const {
    string text = FirstSpeciesCounterpoint::to_string() + "\n Second species : \n";
    text += "Second species notes: " + intVarArray_to_string(secondSpeciesNotesCp) + "\n";
    text += "Second species harmonic intervals: " + intVarArray_to_string(secondSpeciesHarmonicIntervals) + "\n";
    text += "Second species melodic intervals: " + intVarArray_to_string(secondSpeciesMelodicIntervals) + "\n";
    return text;
}

// SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(SecondSpeciesCounterpoint &s): FirstSpeciesCounterpoint(s){     COPY CONSTRUCTOR NO LONGER NEEDED, THIS IS NO LONGER A SPACE
//     secondSpeciesNotesCp.update(*this, s.secondSpeciesNotesCp);
//     secondSpeciesHarmonicIntervals.update(*this, s.secondSpeciesHarmonicIntervals);
//     secondSpeciesMelodicIntervals.update(*this, s.secondSpeciesMelodicIntervals);

// }

// Space* SecondSpeciesCounterpoint::copy() {
//     return new SecondSpeciesCounterpoint(*this);
// }

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(Home home, SecondSpeciesCounterpoint &s): FirstSpeciesCounterpoint(s){  
    secondSpeciesNotesCp.update(home, s.secondSpeciesNotesCp);
    secondSpeciesHarmonicIntervals.update(home, s.secondSpeciesHarmonicIntervals);
    secondSpeciesMotions.update(home, s.secondSpeciesMotions);
    secondSpeciesMotionCosts.update(home, s.secondSpeciesMotionCosts);
    secondSpeciesMelodicIntervals.update(home, s.secondSpeciesMelodicIntervals);
    secondSpeciesRealMotions.update(home, s.secondSpeciesRealMotions);
    secondSpeciesRealMotionCosts.update(home, s.secondSpeciesRealMotionCosts);
}

SecondSpeciesCounterpoint* SecondSpeciesCounterpoint::clone(Home home){
    return new SecondSpeciesCounterpoint(home, *this);
}

IntVarArray SecondSpeciesCounterpoint::getFirstHInterval(){
    return firstSpeciesHarmonicIntervals;
}

IntVarArray SecondSpeciesCounterpoint::getMotions(){
    return secondSpeciesMotions;
}

IntVarArray SecondSpeciesCounterpoint::getFirstMInterval(){
    return firstSpeciesMelodicIntervals;
}

IntVarArray SecondSpeciesCounterpoint::getBranchingNotes(){
    return secondSpeciesNotesCp;
}