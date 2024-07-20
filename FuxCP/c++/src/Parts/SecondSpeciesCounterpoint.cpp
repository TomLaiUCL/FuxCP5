//
// Created by Damien Sprockeels on 12/06/2024.
//

#include "../../headers/Parts/SecondSpeciesCounterpoint.hpp"

/**
 * Second species constructor. Does general constructing. Then calls the general first species constructor
 */
SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, int mSpec, Stratum* low, CantusFirmus* c, int v_type
    , vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
    FirstSpeciesCounterpoint(home, size, cf, lb, ub, k, SECOND_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV) /// super constructor. Applies all rules for the first species to the 1st note of each measure
{
    /// Second species notes in the counterpoint
    secondSpeciesNotesCp = IntVarArray(home, (nMeasures*notesPerMeasure.at(SECOND_SPECIES))-1, IntSet(IntArgs(vector_intersection(cp_range, extended_domain))));
    if(borrowMode==1){
        secondSpeciesNotesCp[secondSpeciesNotesCp.size()-2] = IntVar(home, IntSet(IntArgs(vector_intersection(cp_range, chromatic_scale))));
    }
    rel(home, secondSpeciesNotesCp, IRT_EQ, notes.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),(notes.size())));
    /// Harmonic intervals for the second species notes
    secondSpeciesHarmonicIntervals = IntVarArray(home, (nMeasures*notesPerMeasure.at(SECOND_SPECIES))-1, UNISSON, PERFECT_OCTAVE);
    rel(home, secondSpeciesHarmonicIntervals, IRT_EQ, h_intervals.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),(h_intervals.size())));
    for(int i = 0; i < secondSpeciesHarmonicIntervals.size(); i++){
        rel(home, (secondSpeciesHarmonicIntervals[i])==((secondSpeciesNotesCp[i]-low->getNotes()[floor(i/2)*4])%12));
    }

    /// Melodic intervals for the second species notes
    secondSpeciesMelodicIntervals = IntVarArray(home, secondSpeciesNotesCp.size()-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    rel(home, secondSpeciesMelodicIntervals, IRT_EQ, m_intervals_brut.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),m_intervals_brut.size()));
    //rel(home, secondSpeciesMelodicIntervals, IRT_EQ, m_intervals_brut.slice(0,4/notesPerMeasure.at(SECOND_SPECIES),m_intervals_brut.size()));
    /// link melodic intervals
    for(int i = 0; i < secondSpeciesMelodicIntervals.size(); i++)
        rel(home, secondSpeciesMelodicIntervals[i], IRT_EQ, expr(home, secondSpeciesNotesCp[i+1] - secondSpeciesNotesCp[i]));

    secondSpeciesArsisArray = IntVarArray(home, firstSpeciesMelodicIntervals.size(), -PERFECT_OCTAVE, PERFECT_OCTAVE);
    for(int i = 1; i < secondSpeciesMelodicIntervals.size()-1; i+=2){
        rel(home, secondSpeciesArsisArray[floor(i/2)], IRT_EQ, expr(home, secondSpeciesMelodicIntervals[i]+secondSpeciesMelodicIntervals[i+1]));
    }
    rel(home, secondSpeciesArsisArray[secondSpeciesArsisArray.size()-1], IRT_EQ, secondSpeciesMelodicIntervals[secondSpeciesMelodicIntervals.size()-1]);
    secondSpeciesMotions = IntVarArray(home, (nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1), IntSet{-1, CONTRARY_MOTION, OBLIQUE_MOTION, PARALLEL_MOTION});
    secondSpeciesMotionCosts = IntVarArray(home, (nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1), IntSet{0, directCost, obliqueCost, contraryCost});

    //the second motions are between the two arsis notes as are the first motions between thesis notes. this way of doing it takes the correct
    //m intervals between two arsis notes and then calculates the motion with respect to the cantusFirmus
    for(int i = 0; i < secondSpeciesArsisArray.size(); i++){

        //direct motions help creation
        BoolVar both_up = expr(home, (secondSpeciesArsisArray[i]>0)&&(low->getMelodicIntervals()[i]>0)); //if both parts are going in the same direction
        BoolVar both_stay = expr(home, (secondSpeciesArsisArray[i]==0)&&(low->getMelodicIntervals()[i]==0)); //if both parts are staying
        BoolVar both_down = expr(home, (secondSpeciesArsisArray[i]<0)&&(low->getMelodicIntervals()[i]<0)); //if both parts are going down
        //oblique motions help creation
        BoolVar cf_stays_1 = expr(home, (secondSpeciesArsisArray[i]>0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes up
        BoolVar cf_stays_2 = expr(home, (secondSpeciesArsisArray[i]<0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes down
        BoolVar cp_stays_1 = expr(home, (secondSpeciesArsisArray[i]==0)&&(low->getMelodicIntervals()[i]>0)); //if the lowest part goes up and one stays
        BoolVar cp_stays_2 = expr(home, (secondSpeciesArsisArray[i]==0)&&(low->getMelodicIntervals()[i]<0)); //if the lowest part goes down and one stays
        //contrary motions help creation
        BoolVar cpd_cfu = expr(home, (secondSpeciesArsisArray[i]<0)&&(low->getMelodicIntervals()[i]>0)); //if the cf goes up and the cp down
        BoolVar cpu_cfd = expr(home, (secondSpeciesArsisArray[i]>0)&&(low->getMelodicIntervals()[i]<0)); //if the cf goes down and the cp up
 
        //direct constraints
        rel(home, ((both_up || both_stay || both_down) && (isLowest[i]==1)) >> (secondSpeciesMotions[i]==PARALLEL_MOTION));
        rel(home, ((both_up || both_stay || both_down) && (isLowest[i]==1)) >> (secondSpeciesMotionCosts[i]==directCost));
        //oblique constraints
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (isLowest[i]==1)) >> (secondSpeciesMotions[i]==OBLIQUE_MOTION));
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (isLowest[i]==1)) >> (secondSpeciesMotionCosts[i]==obliqueCost));
        //contrary constraints
        rel(home, ((cpd_cfu || cpu_cfd) && (isLowest[i]==1)) >> (secondSpeciesMotions[i]==CONTRARY_MOTION));
        rel(home, ((cpd_cfu || cpu_cfd) && (isLowest[i]==1)) >> (secondSpeciesMotionCosts[i]==contraryCost));
        //bass constraints
        rel(home, (isLowest[i]==0) >> (secondSpeciesMotions[i]==-1));
        rel(home, (isLowest[i]==0) >> (secondSpeciesMotionCosts[i]==0));
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

    //create isDiminution Array
    isDiminution = BoolVarArray(home, (nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1)-1, 0, 1);

    for(int i = 0; i < isDiminution.size(); i++){

        rel(home, isDiminution[i], IRT_EQ, expr(home, ((abs(firstSpeciesMelodicIntervals[i])==3)||(abs(firstSpeciesMelodicIntervals[i])==4))&&
            (abs(secondSpeciesMelodicIntervals[i*2])<=2)&&(abs(secondSpeciesMelodicIntervals[(i*2)+1])<=2)));

    }

    isConsonance = BoolVarArray(home, h_intervals.size(), 0, 1);
    for(int i = 0; i < isConsonance.size(); i++){
        rel(home, expr(home, (h_intervals[i]==UNISSON)||(h_intervals[i]==MINOR_THIRD)||(h_intervals[i]==MAJOR_THIRD)||(h_intervals[i]==PERFECT_FIFTH)||
            (h_intervals[i]==MINOR_SIXTH)||(h_intervals[i]==MAJOR_SIXTH)||(h_intervals[i]==PERFECT_OCTAVE)), IRT_EQ, isConsonance[i]);
    }

    penultCostArray = IntVarArray(home, 1, IntSet({0, penultCost}));
    /// Constraints

    // 2.H2 : Arsis harmonies cannot be dissonant except if there is a diminution.
    for(int j = 0; j < isDiminution.size(); j++){
        rel(home, (isConsonance[(j*4)+2]==0) >> (isDiminution[j]==1));
        rel(home, (isConsonance[(j*4)+2]==1) >> (isDiminution[j]==0));
    }

    // 2.H3 : penult cost 
    dom(home, firstSpeciesHarmonicIntervals[firstSpeciesHarmonicIntervals.size()-2], IntSet({PERFECT_FIFTH, MINOR_SIXTH, MAJOR_SIXTH}));
    rel(home, (firstSpeciesHarmonicIntervals[firstSpeciesHarmonicIntervals.size()-2]!=7) >> (penultCostArray[0]==penultCost));
    rel(home, (firstSpeciesHarmonicIntervals[firstSpeciesHarmonicIntervals.size()-2]==7) >> (penultCostArray[0]==0));

    // 2.M1 : If the two voices are getting so close that there is no contrary motion possible without crossing each other, then the melodic interval of the counterpoint can be an octave leap.
    //TODO : check if the expression makes sense when I have internet connection
    for(int j = 0; j < firstSpeciesMelodicIntervals.size(); j++){
        rel(home, firstSpeciesMelodicIntervals[j], IRT_EQ, 12, Reify(expr(home,(abs(firstSpeciesHarmonicIntervals[j])>=4)&&
            (expr(home, expr(home, abs(low->getMelodicIntervals()[j])>0) == isLowest[j]))), RM_PMI));
    }
    

    //2.P2 : battuta adapted
    for(int j = 0; j < secondSpeciesMotions.size(); j++){
        rel(home, expr(home, secondSpeciesMotions[j]==CONTRARY_MOTION && firstSpeciesHarmonicIntervals[j+1]==0 && firstSpeciesMelodicIntervals[j]<-4),
            BOT_AND, isLowest[j], 0);
    }

}

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type
    , vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV) :
    SecondSpeciesCounterpoint(home, size, cf, lb, ub, k, SECOND_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV)
{
    costs = IntVarArray(home, 7, 0, 10000);

    /// 2.M2: Two consecutive notes cannot be the same
    //can do better than this?
    rel(home, secondSpeciesMelodicIntervals, IRT_NQ, 0); /// plus efficace que de faire cp[i] /= cp[i+1]

    //2.P1 : adapted no direct motion rule from first species to real motions
    //can do better than this?
    for(int j = 0; j < secondSpeciesRealMotions.size(); j++){
        rel(home, (firstSpeciesHarmonicIntervals[j+1]==UNISSON || firstSpeciesHarmonicIntervals[j+1]==PERFECT_FIFTH) >> 
        (secondSpeciesRealMotions[j]!=2));
    }
    

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(0, 4/notesPerMeasure.at(SECOND_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(0, 4/notesPerMeasure.at(SECOND_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be motion cost
    add_cost(home, 2, secondSpeciesRealMotionCosts, costs);
    //set cost[3] to be melodic cost
    add_cost(home, 3, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(SECOND_SPECIES), melodicDegreeCost.size())), costs);
    //need to set cost[4] to be off cost
    add_cost(home, 4, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(SECOND_SPECIES), offCostArray.size())), costs);
    //set cost[5] to be penult sixth cost
    add_cost(home, 5, penultCostArray, costs);
    //need to set cost[6] to be direct move cost
    add_cost(home, 6, directCostArray, costs);

}

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type, 
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV1, int nV2) :
    SecondSpeciesCounterpoint(home, size, cf, lb, ub, k, SECOND_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV2)
{
    costs = IntVarArray(home, 8, 0, 10000);

    varietyCostArray = IntVarArray(home, 3*(secondSpeciesHarmonicIntervals.size()-2), IntSet({0, varietyCost}));
    directCostArray = IntVarArray(home, secondSpeciesRealMotions.size()-1,IntSet({0, directMoveCost}));

    //P1 3 voices version
    for(int j = 0; j < firstSpeciesMotions.size()-1; j++){
        //set a cost when it is reached through direct motion, it is 0 when not
        rel(home, (secondSpeciesRealMotions[j]==2&&(firstSpeciesHarmonicIntervals[j+1]==0||firstSpeciesHarmonicIntervals[j+1]==7))>>
            (directCostArray[j]==directMoveCost));
        rel(home, (secondSpeciesRealMotions[j]!=2||(firstSpeciesHarmonicIntervals[j+1]!=0&&firstSpeciesHarmonicIntervals[j+1]!=7))>>
            (directCostArray[j]==0));
    }

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(0, 4/notesPerMeasure.at(SECOND_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(0, 4/notesPerMeasure.at(SECOND_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be motion cost
    add_cost(home, 2, secondSpeciesRealMotionCosts, costs);
    //set cost[3] to be melodic cost
    add_cost(home, 3, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(SECOND_SPECIES), melodicDegreeCost.size())), costs);
    //need to set cost[4] to be off cost
    add_cost(home, 4, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(SECOND_SPECIES), offCostArray.size())), costs);
    //set cost[5] to be penult sixth cost
    add_cost(home, 5, penultCostArray, costs);
    //need to set cost[5] to be variety cost
    add_cost(home, 6, varietyCostArray, costs);
    //need to set cost[7] to be direct cost
    add_cost(home, 7, directCostArray, costs);

}

string SecondSpeciesCounterpoint::to_string() const {
    string text = FirstSpeciesCounterpoint::to_string() + "\n Second species : \n";
    text += "Second species notes: " + intVarArray_to_string(secondSpeciesNotesCp) + "\n";
    text += "Second species harmonic intervals: " + intVarArray_to_string(secondSpeciesHarmonicIntervals) + "\n";
    text += "Second species melodic intervals: " + intVarArray_to_string(secondSpeciesMelodicIntervals) + "\n";
    text += "Second species motion intervals: " + intVarArray_to_string(secondSpeciesMotions) + "\n";
    text += "Second species arsis intervals : " + intVarArray_to_string(secondSpeciesArsisArray) + "\n";
    text += "Second species melodic degree costs : " + intVarArray_to_string(melodicDegreeCost) + "\n";
    text += "Costs: " + intVarArray_to_string(costs) + "\n";
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

SecondSpeciesCounterpoint::SecondSpeciesCounterpoint(Home home, SecondSpeciesCounterpoint &s): FirstSpeciesCounterpoint(home, s){  
    secondSpeciesNotesCp.update(home, s.secondSpeciesNotesCp);
    secondSpeciesHarmonicIntervals.update(home, s.secondSpeciesHarmonicIntervals);
    secondSpeciesMotions.update(home, s.secondSpeciesMotions);
    secondSpeciesMotionCosts.update(home, s.secondSpeciesMotionCosts);
    secondSpeciesMelodicIntervals.update(home, s.secondSpeciesMelodicIntervals);
    secondSpeciesRealMotions.update(home, s.secondSpeciesRealMotions);
    secondSpeciesRealMotionCosts.update(home, s.secondSpeciesRealMotionCosts);
    isDiminution.update(home, s.isDiminution);
    penultCostArray.update(home, s.penultCostArray);
    secondSpeciesArsisArray.update(home, s.secondSpeciesArsisArray);
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

int SecondSpeciesCounterpoint::getHIntervalSize(){
    return secondSpeciesHarmonicIntervals.size();
}