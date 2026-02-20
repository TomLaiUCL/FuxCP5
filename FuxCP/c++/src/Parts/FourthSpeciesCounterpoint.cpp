// 
// Created by Luc Cleenewerk and Diego de Patoul. 
// 

#include "../../headers/Parts/FourthSpeciesCounterpoint.hpp"

/**
 * GENERAL CONSTRUCTOR
 */
FourthSpeciesCounterpoint::FourthSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, Species mSpecies, Stratum* low, CantusFirmus* c,
     int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
    Part(home, nMes, mSpecies, cf, lb, ub, v_type, m_costs, g_costs, s_costs, nV, bm)
{

    for(int i = lowerBound; i <= upperBound; i++){
        cp_range.push_back(i);
    }
    
    /*
    if borrowMode is enabled, the extended domain is extended to make the inclusion of borrowed notes possible. We can see from Fux's examples
    that he does like to borrow notes, so the borrow cost should just do the job and still allow borrowed notes, not outright forbid them
    */
    if(borrowMode==1){
        extended_domain = vector_union(cp_range, vector_union(scale, borrowed_scale));
    } else {
        extended_domain = vector_intersection(cp_range, vector_union(scale, borrowed_scale));
    }

    off_domain = vector_difference(vector_intersection(cp_range, scale), lowerBound, upperBound);

    fourthSpeciesNotesCp = IntVarArray(home, ((nMeasures*notesPerMeasure.at(FOURTH_SPECIES))-1)-1, IntSet(IntArgs(vector_intersection(cp_range, extended_domain))));
    if(borrowMode==1){
        fourthSpeciesNotesCp[fourthSpeciesNotesCp.size()-2] = IntVar(home, IntSet(IntArgs(vector_intersection(cp_range, chromatic_scale))));
    }
    
    sol = IntVarArray(home, fourthSpeciesNotesCp.slice(0,1,fourthSpeciesNotesCp.size()));

    //Harmonic intervals
    for (int i = 0; i < h_intervals.size(); i++) {
        rel(home, (h_intervals[i])==(notes[i]-low->getNotes()[i])%12);
    }

    fourthSpeciesHIntervals = IntVarArray(home, ((nMeasures*notesPerMeasure.at(FOURTH_SPECIES))-1)-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);

    for(int i = 0; i < fourthSpeciesHIntervals.size(); i++){
        rel(home, (fourthSpeciesHIntervals[i])==((fourthSpeciesNotesCp[i]-low->getNotes()[floor((i+1)/2)*4])%12));
    }


    firstHInterval = IntVarArray(home, fourthSpeciesHIntervals.slice(1, 2, fourthSpeciesHIntervals.size()));
    
    //m-intervals : arsis and next thesis
    //m-succ-intervals : thesis and arsis in same measure
    //m2-intervals : between n and n+2 for whole counterpoint
    //m-all-intervals : between all notes

    fourthSpeciesMelodicIntervals = IntVarArray(home, fourthSpeciesNotesCp.size()-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    for(int i = 0; i < fourthSpeciesMelodicIntervals.size(); i++){
        rel(home, fourthSpeciesMelodicIntervals[i] == expr(home, fourthSpeciesNotesCp[i+1] - fourthSpeciesNotesCp[i]));
    }

    m2IntervalsArray = IntVarArray(home, fourthSpeciesNotesCp.size()-2, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    for(int i = 0; i < fourthSpeciesNotesCp.size()-2; i++){
        rel(home, m2IntervalsArray[i], IRT_EQ, expr(home, fourthSpeciesNotesCp[i+2]-fourthSpeciesNotesCp[i]));
    }

    //create motions array
    // Arsis motions for the fourth species correspond to the first species motions
    firstSpeciesMotions = IntVarArray(home, nMeasures-1, IntSet{-1, CONTRARY_MOTION, OBLIQUE_MOTION, PARALLEL_MOTION});
    firstSpeciesMotionCosts = IntVarArray(home, firstSpeciesMotions.size(), IntSet{0, directCost, obliqueCost, contraryCost});

    //create motions
    for(int i = 0; i < firstSpeciesMotions.size(); i++){
        // cout << "i: " << i << endl;
        int j = 1;
        if (i == firstSpeciesMotions.size()-1) {
            j = 0; // last motion is between the arsis of penultimate measure and the thesis of the last measure
        }

        //direct motions help creation
        BoolVar both_up = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]>0)&&(low->getMelodicIntervals()[i]>0)); //if both parts are going in the same direction
        BoolVar both_stay = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]==0)&&(low->getMelodicIntervals()[i]==0)); //if both parts are staying
        BoolVar both_down = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]<0)&&(low->getMelodicIntervals()[i]<0)); //if both parts are going 
        //oblique motions help creation
        BoolVar cf_stays_1 = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]>0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes up
        BoolVar cf_stays_2 = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]<0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes down
        BoolVar cp_stays_1 = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]==0)&&(low->getMelodicIntervals()[i]>0)); //if the lowest part goes up and one stays
        BoolVar cp_stays_2 = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]==0)&&(low->getMelodicIntervals()[i]<0)); //if the lowest part goes down and one stays
        //contrary motions help creation
        BoolVar cpd_cfu = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]<0)&&(low->getMelodicIntervals()[i]>0)); //if the cf goes up and the cp down
        BoolVar cpu_cfd = expr(home, (fourthSpeciesMelodicIntervals[i*2+j]>0)&&(low->getMelodicIntervals()[i]<0)); //if the cf goes down and the cp up

        //direct constraints
        rel(home, ((both_up || both_stay || both_down) && (this->isNotLowest[i]==1)) >> (firstSpeciesMotions[i]==PARALLEL_MOTION));
        rel(home, ((both_up || both_stay || both_down) && (this->isNotLowest[i]==1)) >> (firstSpeciesMotionCosts[i]==directCost));
        //oblique constraints
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (this->isNotLowest[i]==1)) >> (firstSpeciesMotions[i]==OBLIQUE_MOTION));
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (this->isNotLowest[i]==1)) >> (firstSpeciesMotionCosts[i]==obliqueCost));
        //contrary constraints
        rel(home, ((cpd_cfu || cpu_cfd) && (this->isNotLowest[i]==1)) >> (firstSpeciesMotions[i]==CONTRARY_MOTION));
        rel(home, ((cpd_cfu || cpu_cfd) && (this->isNotLowest[i]==1)) >> (firstSpeciesMotionCosts[i]==contraryCost));
        //bass constraints
        rel(home, (this->isNotLowest[i]==0) >> (firstSpeciesMotions[i]==-1));
        rel(home, (this->isNotLowest[i]==0) >> (firstSpeciesMotionCosts[i]==0));
    }
    
    is_off = BoolVarArray(home, notes.size(), 0, 1);
    for(int i = 0; i < is_off.size(); i++){ // Using Reify domain membership
        BoolVar inOff(home, 0, 1);
        dom(home, notes[i], IntSet(IntArgs(off_domain)), Reify(inOff, RM_EQV));
        rel(home, is_off[i] == inOff);
    }

    //create off_cost array
    offCostArray = IntVarArray(home, is_off.size(), IntSet({0, borrowCost}));
    //set the cost for borrowing this note
    for(int i = 0; i < is_off.size(); i++){
        rel(home, (is_off[i]==0) >> (offCostArray[i]==0));
        rel(home, (is_off[i]==1) >> (offCostArray[i]==borrowCost));
    }

    melodicDegreeCost = IntVarArray(home, m_intervals_brut.size(), IntSet({secondCost, thirdCost, fourthCost, tritoneCost, fifthCost, 
        sixthCost, seventhCost, octaveCost}));
    
    // create pefectConsArray
    fifthCostArray = IntVarArray(home, h_intervals.size(), IntSet({0, h_fifthCost}));
    octaveCostArray = IntVarArray(home, h_intervals.size(), IntSet({0, h_octaveCost}));
    
    isConsonance = BoolVarArray(home, h_intervals.size(), 0, 1);
    for(int i = 0; i < isConsonance.size(); i++){
        rel(home, expr(home, (abs(h_intervals[i])==UNISSON)||(abs(h_intervals[i])==MINOR_THIRD)||(abs(h_intervals[i])==MAJOR_THIRD)||(abs(h_intervals[i])==PERFECT_FIFTH)||
            (abs(h_intervals[i])==MINOR_SIXTH)||(abs(h_intervals[i])==MAJOR_SIXTH)||(abs(h_intervals[i])==PERFECT_OCTAVE)), IRT_EQ, isConsonance[i]);
    }

    isNoSyncopeArray = BoolVarArray(home, nMeasures-1, 0, 1);
    for(int i = 0; i < isNoSyncopeArray.size(); i++){
        rel(home, fourthSpeciesMelodicIntervals[(i*2)], IRT_NQ, 0, Reify(isNoSyncopeArray[i]));
    }
    
    // G6 : no chromatic melodies
    if (activeConstraints[SP4_G6]) {
        for(int i = 0; i < m_intervals_brut.size()-1; i+=4/notesPerMeasure.at(FOURTH_SPECIES)){
            rel(home, expr(home, m_intervals_brut[i]==1), BOT_AND, expr(home, m_intervals_brut[i+1]==1), 0);
            rel(home, expr(home, m_intervals_brut[i]==-1), BOT_AND, expr(home, m_intervals_brut[i+1]==-1), 0);
        }
    }   
    
    // G7 : melodic intervals should be small (works for 1st, 2nd and 3rd species)
    if (activeConstraints[SP4_G7]) {
        int idx = 0;
        for(int i = 0; i < m_intervals_brut.size(); i+=4/notesPerMeasure.at(FOURTH_SPECIES)){
            rel(home, (abs(m_intervals_brut[i])<MINOR_THIRD) >> (melodicDegreeCost[i]==secondCost));
            rel(home, (abs(m_intervals_brut[i])==MINOR_THIRD || abs(m_intervals_brut[i])==MAJOR_THIRD) >> (melodicDegreeCost[i]==thirdCost));
            rel(home, (abs(m_intervals_brut[i])==PERFECT_FOURTH) >> (melodicDegreeCost[i]==fourthCost));
            rel(home, (abs(m_intervals_brut[i])==TRITONE) >> (melodicDegreeCost[i]==tritoneCost));
            rel(home, (abs(m_intervals_brut[i])==PERFECT_FIFTH) >> (melodicDegreeCost[i]==fifthCost));
            rel(home, (abs(m_intervals_brut[i])==MINOR_SIXTH || abs(m_intervals_brut[i])==MAJOR_SIXTH) >> (melodicDegreeCost[i]==sixthCost));
            rel(home, (abs(m_intervals_brut[i])==MINOR_SEVENTH || abs(m_intervals_brut[i])==MAJOR_SEVENTH) >> (melodicDegreeCost[i]==seventhCost));
            rel(home, (abs(m_intervals_brut[i])==PERFECT_OCTAVE) >> (melodicDegreeCost[i]==octaveCost));
            idx++;
        }
    }

    m2ZeroArray = IntVarArray(home, ((fourthSpeciesNotesCp.size()-1)/2)-2, IntSet({0, m2ZeroCost}));
    snycopeCostArray = IntVarArray(home, (fourthSpeciesMelodicIntervals.size())/2, IntSet({0, syncopationCost}));
    
    //link them
    rel(home, fourthSpeciesNotesCp, IRT_EQ, notes.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), notes.size()));
    rel(home, fourthSpeciesMelodicIntervals, IRT_EQ, m_intervals_brut.slice(2,4/notesPerMeasure.at(FOURTH_SPECIES),m_intervals_brut.size()));
    // rel(home, fourthSpeciesHIntervals, IRT_EQ, h_intervals.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), h_intervals.size()));

    // H6 from Thibault : Imperfect consonances are preferred
    if (activeConstraints[SP4_1H6]) {
        for(int i = 0; i < h_intervals.size(); i++){

            rel(home, octaveCostArray[i], IRT_EQ, h_octaveCost, Reify(expr(home, h_intervals[i]==UNISSON), RM_PMI));
            rel(home, octaveCostArray[i], IRT_EQ, 0, Reify(expr(home, h_intervals[i]!=UNISSON), RM_PMI));
    
            rel(home, fifthCostArray[i], IRT_EQ, h_fifthCost, Reify(expr(home, h_intervals[i]==PERFECT_FIFTH), RM_PMI));
            rel(home, fifthCostArray[i], IRT_EQ, 0, Reify(expr(home, h_intervals[i]!=PERFECT_FIFTH), RM_PMI));
    
        }
    }
    

    //4.H1 : arsis harmonies must be consonant (could be the reason for 3 / 4 voice bugs)
    if (activeConstraints[SP4_4H1]) {
        for(int i = 2; i < h_intervals.size(); i+=4){
            dom(home, expr(home, abs(h_intervals[i])), IntSet(CONSONANCES));
        }
    }
    
    //4.M1 Arsis half notes should be the same as their next halves in thesis
    if (activeConstraints[SP4_4M1]) {
        for(int i = 0; i < snycopeCostArray.size(); i++){
            rel(home, (isNoSyncopeArray[i]==1) >> (snycopeCostArray[i]==syncopationCost));
            rel(home, (isNoSyncopeArray[i]==0) >> (snycopeCostArray[i]==0));
        }  
    }
    
    //4.M2 notes and two beats further are preferred to be different
    if (activeConstraints[SP4_4M2]) {
        for(int i = 0; i < m2ZeroArray.size(); i++){
            rel(home, (fourthSpeciesNotesCp[(i*2)]==fourthSpeciesNotesCp[(i*2)+4]) >> (m2ZeroArray[i]==m2ZeroCost));
            rel(home, (fourthSpeciesNotesCp[(i*2)]!=fourthSpeciesNotesCp[(i*2)+4]) >> (m2ZeroArray[i]==0));
        }
    }
    
    //4.P1
    if (activeConstraints[SP4_4P1]) {
        for(int i = 1; i < nMeasures-2; i++){
            rel(home, (isConsonance[(i*4)]==0) >> ((fourthSpeciesMelodicIntervals[(i*2)-1]==-MINOR_SECOND) || 
               (fourthSpeciesMelodicIntervals[(i*2)-1]==-MAJOR_SECOND)));
        }
    }

    //4.P2
    if (activeConstraints[SP4_4P2]) {
        for(int i = 0; i < nMeasures-1; i++){
            BoolVar buni = BoolVar(home, 0, 1);
            BoolVar band = BoolVar(home, 0, 1);
            rel(home, fourthSpeciesHIntervals[(i*2)], IRT_EQ, UNISSON, buni);
            rel(home, expr(home, !c->getIsNotLowest()[i]), BOT_AND, buni, band);
            rel(home, fourthSpeciesHIntervals[i*2], IRT_NQ, 1, Reify(band, RM_IMP));
            rel(home, fourthSpeciesHIntervals[i*2], IRT_NQ, 2, Reify(band, RM_IMP));
        }
    }

    //1.M2 Melodic intervals cannot be greater than Minor Sixth (except octave)
    if (activeConstraints[SP4_1M2]) {
        dom(home, fourthSpeciesMelodicIntervals, IntSet({UNISSON, -PERFECT_OCTAVE, -MINOR_SIXTH, -PERFECT_FIFTH, -AUGMENTED_FOURTH, -PERFECT_FOURTH, 
            -MAJOR_THIRD, -MINOR_THIRD, -MAJOR_SECOND, -MINOR_SECOND, PERFECT_OCTAVE, MINOR_SIXTH, PERFECT_FIFTH, AUGMENTED_FOURTH, PERFECT_FOURTH, 
            MAJOR_THIRD, MINOR_THIRD, MAJOR_SECOND, MINOR_SECOND}));
    }
    
    //DISABELED
    // //no chromatic motion between 3 consecutive notes
    // if (activeConstraints[SP4_U2]) {
    //     for(int i = 1; i < fourthSpeciesMelodicIntervals.size()-1; i++){
    //         BoolVar b1 = BoolVar(home, 0, 1);
    //         BoolVar b2 = BoolVar(home, 0, 1);
    //         BoolVar b3 = BoolVar(home, 0, 1);
    //         BoolVar b4 = BoolVar(home, 0, 1);
    
    //         rel(home, fourthSpeciesMelodicIntervals[i+1], IRT_EQ, 1, Reify(b1));
    //         rel(home, m2IntervalsArray[i], IRT_EQ, 2, Reify(b2));
    //         rel(home, b1, BOT_AND, b2, 0);
    
    //         rel(home, fourthSpeciesMelodicIntervals[i+1], IRT_EQ, -1, Reify(b3));
    //         rel(home, m2IntervalsArray[i], IRT_EQ, -2, Reify(b4));
    //         rel(home, b3, BOT_AND, b4, 0);
    //     }
    // }
}   

/**
 * 2 VOICES CONSTRUCTOR
 */
FourthSpeciesCounterpoint::FourthSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, Stratum* low, CantusFirmus* c,  int v_type, 
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
    FourthSpeciesCounterpoint(home, nMes, cf, lb, ub, FOURTH_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV)
{
    
    //4.H2 : If the 4th species is the lowest stratum, then no hamonic seventh
    if (activeConstraints[SP4_4H2_2V]) {
        for(int j = 1; j < getIsNotLowest().size(); j++){
            rel(home, (getIsNotLowest()[j]==0) >> (fourthSpeciesHIntervals[(j*2)-1]!=MINOR_SEVENTH && fourthSpeciesHIntervals[(j*2)-1]!=-MINOR_SEVENTH));
            rel(home, (getIsNotLowest()[j]==0) >> (fourthSpeciesHIntervals[(j*2)-1]!=MAJOR_SEVENTH && fourthSpeciesHIntervals[(j*2)-1]!=-MAJOR_SEVENTH));
        }
    }

    //4.H3 Penult note condition
    if (activeConstraints[SP4_4H3_2V]) {
        rel(home,(isNotLowest[isNotLowest.size()-2]==1) >> (expr(home, abs(h_intervals[h_intervals.size()-3]))==MAJOR_SIXTH));
    }
    
    //Must start with a perfect consonance (since 1.H2 applies to the first note, which is most likely a rest in 4th species)
    if (activeConstraints[SP4_1H2_2V]) {
        dom(home, fourthSpeciesHIntervals[0], IntSet({UNISSON, PERFECT_FIFTH, -PERFECT_FIFTH}));
    }
    
    costs = IntVarArray(home, 6, 0, 1000000);
    cost_names = {"fifth", "octave", "melodic", "borrow", "m2", "syncopation"};

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be melodic cost
    add_cost(home, 2, IntVarArray(home, melodicDegreeCost.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), melodicDegreeCost.size())), costs);
    //set cost[3] to be off cost
    add_cost(home, 3, IntVarArray(home, offCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), offCostArray.size())), costs);
    //need to set cost[4] to be m2Zero cost
    add_cost(home, 4, m2ZeroArray, costs);
    //need to set cost[5] to be syncopation cost
    add_cost(home, 5, snycopeCostArray, costs);
}

/**
 * 3 VOICES CONSTRUCTOR
 */
FourthSpeciesCounterpoint::FourthSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, Stratum* low, CantusFirmus* c,  int v_type, 
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV1, int nV2):
    FourthSpeciesCounterpoint(home, nMes, cf, lb, ub, FOURTH_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV2)
{
    varietyCostArray = IntVarArray(home, 3*(getHIntervalSize()-2), IntSet({0, varietyCost}));

    //4.P5 -- after careful testing, Fux does not seem to follow this rule in many of his examples. Suspended for now, but implementation left in case the decision is taken to reactivate it. 
    if (activeConstraints[SP4_4P5_3V] && false) {
        for(int j = 1; j < nMeasures-1; j++){
            rel(home, (low->getMelodicIntervals()[j]==0)>>(expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MINOR_SECOND || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MAJOR_SECOND
                || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==PERFECT_FOURTH || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==AUGMENTED_FOURTH ||
                expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MINOR_SEVENTH || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MAJOR_SEVENTH));
        }
    }
    

    costs = IntVarArray(home, 7, 0, 1000000);
    cost_names = {"fifth", "octave", "melodic", "borrow", "m2", "syncopation", "variety"};

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be melodic cost
    add_cost(home, 2, IntVarArray(home, melodicDegreeCost.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), melodicDegreeCost.size())), costs);
    //set cost[3] to be off cost
    add_cost(home, 3, IntVarArray(home, offCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), offCostArray.size())), costs);
    //need to set cost[4] to be m2Zero cost
    add_cost(home, 4, m2ZeroArray, costs);
    //need to set cost[5] to be syncopation cost
    add_cost(home, 5, snycopeCostArray, costs);
    //need to set cost[6] to be variety cost
    add_cost(home, 6, varietyCostArray, costs);
}

/**
 * 4 VOICES CONSTRUCTOR
 */
FourthSpeciesCounterpoint::FourthSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, Stratum* low, CantusFirmus* c,  int v_type, 
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV1, int nV2, int nV3):
    FourthSpeciesCounterpoint(home, nMes, cf, lb, ub, FOURTH_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV2)
{
    varietyCostArray = IntVarArray(home, 3*(getHIntervalSize()-2), IntSet({0, varietyCost}));

    //4.P5 -- after careful testing, Fux does not seem to follow this rule in many of his examples. Suspended for now, but implementation left in case the decision is taken to reactivate it. 
    if (activeConstraints[SP4_4P5_4V] && false) {
        for(int j = 1; j < nMeasures-1; j++){
            rel(home, (low->getMelodicIntervals()[j]==0)>>(expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MINOR_SECOND || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MAJOR_SECOND
                || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==PERFECT_FOURTH || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==AUGMENTED_FOURTH ||
                expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MINOR_SEVENTH || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MAJOR_SEVENTH));
        }
    }
    

    costs = IntVarArray(home, 7, 0, 1000000);
    cost_names = {"fifth", "octave", "melodic", "borrow", "m2", "syncopation", "variety"};

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be melodic cost
    add_cost(home, 2, IntVarArray(home, melodicDegreeCost.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), melodicDegreeCost.size())), costs);
    //set cost[3] to be off cost
    add_cost(home, 3, IntVarArray(home, offCostArray.slice(2, 4/notesPerMeasure.at(FOURTH_SPECIES), offCostArray.size())), costs);
    //need to set cost[4] to be m2Zero cost
    add_cost(home, 4, m2ZeroArray, costs);
    //need to set cost[5] to be syncopation cost
    add_cost(home, 5, snycopeCostArray, costs);
    //need to set cost[6] to be variety cost
    add_cost(home, 6, varietyCostArray, costs);
}

/** j < nMeasures-1; j++){
    //     rel(home, (low->getMelodicIntervals()[j]==0)>>(expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MINOR_SECOND || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MAJOR_SECOND
    //        || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==PERFECT_FOURTH || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==AUGMENTED_FOURTH ||
    //        expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MINOR_SEVENTH || expr(home, abs(fourthSpeciesHIntervals[(j*2)+1]))==MAJOR_SEVENTH));
    // }
 * This function returns a string with the characteristics of the counterpoint. It calls the to_string() method from
 * the Part class and adds 1st species specific characteristics.
 * @return a string representation of the current instance of the FirstSpeciesCounterpoint class.
 */
string FourthSpeciesCounterpoint::to_string() const {
    string text = Part::to_string() + "\nFourth species :\n";
    text += "No Syncope array : " + boolVarArray_to_string(isNoSyncopeArray) + "\n";
    text += "Fourth species notes : " + intVarArray_to_string(fourthSpeciesNotesCp) + "\n";
    text += "Fourth species h intervals : " + intVarArray_to_string(fourthSpeciesHIntervals) + "\n";
    text += "Fourth species m intervals : " + intVarArray_to_string(fourthSpeciesMelodicIntervals) + "\n";
    text += "is Consonance array : " + boolVarArray_to_string(isConsonance) + "\n";
    return text;
}

// clone constructor
FourthSpeciesCounterpoint::FourthSpeciesCounterpoint(Home home, FourthSpeciesCounterpoint &s) : Part(home, s){
    fourthSpeciesNotesCp.update(home, s.fourthSpeciesNotesCp);
    fourthSpeciesHIntervals.update(home, s.fourthSpeciesHIntervals);
    fourthSpeciesMelodicIntervals.update(home, s.fourthSpeciesMelodicIntervals);
    m2IntervalsArray.update(home, s.m2IntervalsArray);
    firstHInterval.update(home, s.firstHInterval);
    m2ZeroArray.update(home, s.m2ZeroArray);
    sol.update(home, s.sol);
}

FourthSpeciesCounterpoint* FourthSpeciesCounterpoint::clone(Home home){
    return new FourthSpeciesCounterpoint(home, *this);
}

IntVarArray FourthSpeciesCounterpoint::getBranchingNotes(){
    return sol;
}

IntVarArray FourthSpeciesCounterpoint::getFirstHInterval(){
    return firstHInterval;
}

IntVarArray FourthSpeciesCounterpoint::getMotions(){
    return motions;
}

IntVarArray FourthSpeciesCounterpoint::getFirstMInterval(){
    return m2IntervalsArray;
}

int FourthSpeciesCounterpoint::getHIntervalSize(){
    return fourthSpeciesHIntervals.size();
}