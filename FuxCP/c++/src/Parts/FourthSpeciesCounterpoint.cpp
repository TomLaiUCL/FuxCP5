#include "../../headers/Parts/FourthSpeciesCounterpoint.hpp"

/**
 * GENERAL CONSTRUCTOR
 */
FourthSpeciesCounterpoint::FourthSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, int mSpecies, Stratum* low, CantusFirmus* c,
     int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
    Part(home, nMes, mSpecies, cf, lb, ub, k, v_type, m_costs, g_costs, s_costs, nV, bm)
{
    motherSpecies =         mSpecies;
    cout << "Lower bound : " << endl;
    cout << lowerBound << endl;
    cout << "Upper bound : " << endl;
    cout << upperBound << endl;
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

    fourthSpeciesNotesCp = IntVarArray(home, (nMeasures*notesPerMeasure.at(FOURTH_SPECIES))-1, IntSet(IntArgs(vector_intersection(cp_range, extended_domain))));
    if(borrowMode==1){
        fourthSpeciesNotesCp[fourthSpeciesNotesCp.size()-2] = IntVar(home, IntSet(IntArgs(vector_intersection(cp_range, chromatic_scale))));
    }

    fourthSpeciesHIntervals = IntVarArray(home, (nMeasures*notesPerMeasure.at(FOURTH_SPECIES))-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);

    for(int i = 1; i < fourthSpeciesHIntervals.size(); i++){
        if(i%2==0){
            cout << i << endl;
            rel(home, (fourthSpeciesHIntervals[i])==((fourthSpeciesNotesCp[i]-low->getNotes()[(i/2)*4])%12));
        } else {
            rel(home, (fourthSpeciesHIntervals[i])==((fourthSpeciesNotesCp[i]-low->getNotes()[floor(i/2)*4])%12));
        }
    }

    firstHInterval = IntVarArray(home, fourthSpeciesHIntervals.slice(0, 2, fourthSpeciesHIntervals.size()));

    //m-intervals : arsis and next thesis
    //m-succ-intervals : thesis and arsis in same measure
    //m2-intervals : between n and n+2 for whole counterpoint
    //m-all-intervals : between all notes

    fourthSpeciesMelodicIntervals = IntVarArray(home, fourthSpeciesNotesCp.size()-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    for(int i = 0; i < fourthSpeciesMelodicIntervals.size(); i++){
        rel(home, fourthSpeciesMelodicIntervals[i], IRT_EQ, expr(home, fourthSpeciesNotesCp[i+1] - fourthSpeciesNotesCp[i]));
    }

    m2IntervalsArray = IntVarArray(home, fourthSpeciesNotesCp.size()-2, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    for(int i = 0; i < fourthSpeciesNotesCp.size()-2; i++){
        rel(home, m2IntervalsArray[i], IRT_EQ, expr(home, fourthSpeciesNotesCp[i+2]-fourthSpeciesNotesCp[i]));
    }

    is_off = BoolVarArray(home, notes.size(), 0, 1);
    for(int i = 0; i < is_off.size(); i++){
        IntVarArray res = IntVarArray(home, off_domain.size(), 0, 1);
        IntVar sm = IntVar(home, 0, off_domain.size());
        for(int l = 0; l < off_domain.size(); l++){      // TODO il y a d'office une meilleure manière de faire que double boucle for
            BoolVar b1 = BoolVar(home, 0, 1);
            rel(home, notes[i], IRT_EQ, off_domain[l], Reify(b1));   // REIFY RM_PMI?
            ite(home, b1, IntVar(home, 1, 1), IntVar(home, 0, 0), res[l]);
        }
        IntVarArgs x(res.size());
        for(int t = 0; t < off_domain.size(); t++){
            x[t] = res[t];
        }
        rel(home, sm, IRT_EQ, expr(home, sum(x)));
        rel(home, sm, IRT_GR, 0, Reify(is_off[i]));  // REIFY RM_PMI?*/
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
    fifthCostArray = IntVarArray(home, h_intervals.size()-1, IntSet({0, h_fifthCost}));
    octaveCostArray = IntVarArray(home, h_intervals.size()-1, IntSet({0, h_octaveCost}));
    // H6 from Thibault : Imperfect consonances are preferred
    for(int i = 1; i < h_intervals.size(); i++){
        rel(home, octaveCostArray[i-1], IRT_EQ, h_octaveCost, Reify(expr(home, h_intervals[i]==UNISSON), RM_PMI));
        rel(home, octaveCostArray[i-1], IRT_EQ, 0, Reify(expr(home, h_intervals[i]!=UNISSON), RM_PMI));

        rel(home, fifthCostArray[i-1], IRT_EQ, h_fifthCost, Reify(expr(home, h_intervals[i]==PERFECT_FIFTH), RM_PMI));
        rel(home, fifthCostArray[i-1], IRT_EQ, 0, Reify(expr(home, h_intervals[i]!=PERFECT_FIFTH), RM_PMI));

    }

    isConsonance = BoolVarArray(home, h_intervals.size(), 0, 1);
    for(int i = 0; i < isConsonance.size(); i++){
        rel(home, expr(home, (h_intervals[i]==UNISSON)||(h_intervals[i]==MINOR_THIRD)||(h_intervals[i]==MAJOR_THIRD)||(h_intervals[i]==PERFECT_FIFTH)||
            (h_intervals[i]==MINOR_SIXTH)||(h_intervals[i]==MAJOR_SIXTH)||(h_intervals[i]==PERFECT_OCTAVE)), IRT_EQ, isConsonance[i]);
    }

    isNoSyncopeArray = BoolVarArray(home, nMeasures-1, 0, 1);
    for(int i = 0; i < isNoSyncopeArray.size(); i++){
        rel(home, fourthSpeciesMelodicIntervals[(i*2)+1], IRT_EQ, 0, Reify(isNoSyncopeArray[i]));
    }

    // G6 : no chromatic melodies (works for 1st, 2nd and 3rd species)
    
    for(int i = 0; i < m_intervals_brut.size()-1; i+=4/notesPerMeasure.at(FOURTH_SPECIES)){
        rel(home, expr(home, m_intervals_brut[i]==1), BOT_AND, expr(home, m_intervals_brut[i+1]==1), 0);
        rel(home, expr(home, m_intervals_brut[i]==-1), BOT_AND, expr(home, m_intervals_brut[i+1]==-1), 0);
    }
    
    // G7 : melodic intervals should be small (works for 1st, 2nd and 3rd species)
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

    m2ZeroArray = IntVarArray(home, ((fourthSpeciesNotesCp.size()-1)/2)-2, IntSet({0, m2ZeroCost}));
    snycopeCostArray = IntVarArray(home, (fourthSpeciesMelodicIntervals.size())/2, IntSet({0, syncopationCost}));
    
    //link them
    //rel(home, fourthSpeciesNotesCp, IRT_EQ, notes.slice(0, 4/notesPerMeasure.at(FOURTH_SPECIES), notes.size()));
    rel(home, fourthSpeciesMelodicIntervals, IRT_EQ, m_intervals_brut.slice(0,4/notesPerMeasure.at(FOURTH_SPECIES),m_intervals_brut.size()));
    rel(home, fourthSpeciesHIntervals, IRT_EQ, h_intervals.slice(0, 4/notesPerMeasure.at(FOURTH_SPECIES), h_intervals.size()));

    //4.H1 : arsis harmonies must be consonant
    /*dom(home, fourthSpeciesHIntervals[fourthSpeciesHIntervals.size()-3], IntSet({-MAJOR_SEVENTH, -MINOR_SEVENTH, -MAJOR_SECOND, -MINOR_SECOND, UNISSON,
        MINOR_SECOND, MAJOR_SECOND, MINOR_SEVENTH, MAJOR_SEVENTH}));

    for(int i = 1; i < fourthSpeciesHIntervals.size(); i+=2){
        if(i==1){ //if first interval
            dom(home, fourthSpeciesHIntervals[i], IntSet({UNISSON, PERFECT_FIFTH, MINOR_SIXTH, MAJOR_SIXTH, PERFECT_OCTAVE, 
                -MINOR_THIRD, -MAJOR_THIRD, -PERFECT_FIFTH, -MINOR_SIXTH, -MAJOR_SIXTH, -PERFECT_OCTAVE}));
        } else if(i==fourthSpeciesHIntervals.size()-2){ //if penultimate interval
            dom(home, fourthSpeciesHIntervals[i], IntSet({-MAJOR_SIXTH, -MINOR_THIRD, UNISSON, MINOR_THIRD, MAJOR_SIXTH}));
        } else {
            BoolVar lowerStays = BoolVar(home, 0, 1);
            BoolVar notLowestAndLowerStays = BoolVar(home, 0, 1);
            BoolVar lowerNotStays = BoolVar(home, 0, 1);
            BoolVar notLowestAndNotLowerStays = BoolVar(home, 0, 1);
            IntVar hDis = IntVar(home, -11, 11);
            IntVar hCons = IntVar(home, -11, 11);

            rel(home, low->getMelodicIntervals()[(floor(i/2)-1)*4], IRT_EQ, 0, Reify(lowerStays));
            rel(home, lowerStays, BOT_AND, isLowest[floor(i/2)], notLowestAndLowerStays);
            rel(home, low->getMelodicIntervals()[(floor(i/2)-1)*4], IRT_NQ, 0, Reify(lowerNotStays));
            rel(home, lowerNotStays, BOT_AND, isLowest[floor(i/2)], notLowestAndNotLowerStays);

            dom(home, hDis, IntSet({MINOR_SECOND, MAJOR_SECOND, PERFECT_FOURTH, AUGMENTED_FOURTH, MINOR_SEVENTH, MAJOR_SEVENTH,
                -MINOR_SECOND, -MAJOR_SECOND, -PERFECT_FOURTH, -AUGMENTED_FOURTH, -MINOR_SEVENTH, -MAJOR_SEVENTH}));
            dom(home, hCons, IntSet({UNISSON, MINOR_THIRD, MAJOR_THIRD, PERFECT_FIFTH, MINOR_SIXTH, MAJOR_SIXTH, 
                -MINOR_THIRD, -MAJOR_THIRD, -PERFECT_FIFTH, -MINOR_SIXTH, -MAJOR_SIXTH}));

            rel(home, hDis, IRT_EQ, fourthSpeciesHIntervals[i], Reify(notLowestAndLowerStays));
            rel(home, hCons, IRT_EQ, fourthSpeciesHIntervals[i], Reify(notLowestAndNotLowerStays));
        }
    }
    dom(home, fourthSpeciesHIntervals[fourthSpeciesHIntervals.size()-1], IntSet({-PERFECT_FIFTH, -MAJOR_THIRD, UNISSON, MAJOR_THIRD, PERFECT_FIFTH}));

    for(int i = 0; i < isNoSyncopeArray.size(); i++){
        rel(home, (isNoSyncopeArray[i]) >> (fourthSpeciesHIntervals[(i*2)+1]!=MINOR_SECOND && fourthSpeciesHIntervals[(i*2)+1]!=MAJOR_SECOND &&
            fourthSpeciesHIntervals[(i*2)+1]!=PERFECT_FOURTH && fourthSpeciesHIntervals[(i*2)+1]!=AUGMENTED_FOURTH && 
            fourthSpeciesHIntervals[(i*2)+1]!=MINOR_SEVENTH && fourthSpeciesHIntervals[(i*2)+1]!=MAJOR_SEVENTH &&
                fourthSpeciesHIntervals[(i*2)+1]!=-MINOR_SECOND && fourthSpeciesHIntervals[(i*2)+1]!=-MAJOR_SECOND && 
                fourthSpeciesHIntervals[(i*2)+1]!=-PERFECT_FOURTH && fourthSpeciesHIntervals[(i*2)+1]!=-AUGMENTED_FOURTH &&
                fourthSpeciesHIntervals[(i*2)+1]!=-MINOR_SEVENTH && fourthSpeciesHIntervals[(i*2)+1]!=-MAJOR_SEVENTH));
    }

    //4.H2 : If the cantusFirmus is in the upper part, then no hamonic seventh
    for(int j = 1; j < c->getIsLowest().size(); j++){
        BoolVar band = BoolVar(home, 0, 1);
        rel(home, (!c->getIsLowestIdx(j)) >> (band));
        rel(home, firstHInterval[j], IRT_NQ, MINOR_SEVENTH, Reify(band, RM_IMP));
        rel(home, firstHInterval[j], IRT_NQ, MAJOR_SEVENTH, Reify(band, RM_IMP));
    }

    //Melodic intervals cannot be greater than Minor Sixth (except octave)
    dom(home, fourthSpeciesMelodicIntervals, IntSet({UNISSON, -PERFECT_OCTAVE, -MINOR_SIXTH, -PERFECT_FIFTH, -AUGMENTED_FOURTH, -PERFECT_FOURTH, 
        -MAJOR_THIRD, -MINOR_THIRD, -MAJOR_SECOND, -MINOR_SECOND, PERFECT_OCTAVE, MINOR_SIXTH, PERFECT_FIFTH, AUGMENTED_FOURTH, PERFECT_FOURTH, 
        MAJOR_THIRD, MINOR_THIRD, MAJOR_SECOND, MINOR_SECOND}));

    //no chromatic motion between 3 consecutive notes
    for(int i = 1; i < fourthSpeciesMelodicIntervals.size()-1; i++){
        BoolVar b1 = BoolVar(home, 0, 1);
        BoolVar b2 = BoolVar(home, 0, 1);
        BoolVar b3 = BoolVar(home, 0, 1);
        BoolVar b4 = BoolVar(home, 0, 1);

        rel(home, fourthSpeciesMelodicIntervals[i+1], IRT_EQ, 1, Reify(b1));
        rel(home, m2IntervalsArray[i], IRT_EQ, 2, Reify(b2));
        rel(home, b1, BOT_AND, b2, 0);

        rel(home, fourthSpeciesMelodicIntervals[i+1], IRT_EQ, -1, Reify(b3));
        rel(home, m2IntervalsArray[i], IRT_EQ, -2, Reify(b4));
        rel(home, b3, BOT_AND, b4, 0);
    }

    //4.M1 Arsis half notes should be the same as their next halves in thesis
    for(int i = 0; i < snycopeCostArray.size(); i++){
        rel(home, (fourthSpeciesMelodicIntervals[(i*2)+1]!=0) >> (snycopeCostArray[i]==syncopationCost));
        rel(home, (fourthSpeciesMelodicIntervals[(i*2)+1]==0) >> (snycopeCostArray[i]==0));
    }

    //4.M2 notes and two beats further are preferred to be different
    for(int i = 0; i < m2ZeroArray.size(); i++){
        rel(home, (fourthSpeciesNotesCp[(i*2)+1]==fourthSpeciesNotesCp[(i*2)+5]) >> (m2ZeroArray[i]==m2ZeroCost));
        rel(home, (fourthSpeciesNotesCp[(i*2)+1]!=fourthSpeciesNotesCp[(i*2)+5]) >> (m2ZeroArray[i]==0));
    }*/
}   

FourthSpeciesCounterpoint::FourthSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, Stratum* low, CantusFirmus* c,  int v_type, 
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
    FourthSpeciesCounterpoint(home, nMes, cf, lb, ub, k, FOURTH_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV)
{
    //Must start with a perfect consonance
    //dom(home, fourthSpeciesHIntervals[1], IntSet({UNISSON, PERFECT_FIFTH, -PERFECT_FIFTH}));

    //4.H3 Penult note condition
    //ite(home, isLowest[isLowest.size()-2], IntVar(home, 9, 9), IntVar(home, 3, 3), fourthSpeciesHIntervals[fourthSpeciesHIntervals.size()-2]);

    costs = IntVarArray(home, 6, 0, 1000000);
    cost_names = {"fifth", "octave", "melodic", "borrow", "m2", "syncopation"};

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(1, 4/notesPerMeasure.at(FOURTH_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(1, 4/notesPerMeasure.at(FOURTH_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be melodic cost
    add_cost(home, 2, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(FOURTH_SPECIES), melodicDegreeCost.size())), costs);
    //set cost[3] to be off cost
    add_cost(home, 3, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(FOURTH_SPECIES), offCostArray.size())), costs);
    //need to set cost[4] to be m2Zero cost
    add_cost(home, 4, m2ZeroArray, costs);
    //need to set cost[5] to be syncopation cost
    add_cost(home, 5, snycopeCostArray, costs);
}

/**
 * This function returns a string with the characteristics of the counterpoint. It calls the to_string() method from
 * the Part class and adds 1st species specific characteristics.
 * @return a string representation of the current instance of the FirstSpeciesCounterpoint class.
 */
string FourthSpeciesCounterpoint::to_string() const {
    string text = Part::to_string() + "\nFourth species :\n";
    text += "No Syncope array : " + boolVarArray_to_string(isNoSyncopeArray) + "\n";
    text += "H intervals array : " + intVarArray_to_string(fourthSpeciesHIntervals) + "\n";
    return text;
}

// clone constructor
FourthSpeciesCounterpoint::FourthSpeciesCounterpoint(Home home, FourthSpeciesCounterpoint &s) : Part(home, s){
    motherSpecies = s.motherSpecies;
    fourthSpeciesNotesCp.update(home, s.fourthSpeciesNotesCp);
    fourthSpeciesHIntervals.update(home, s.fourthSpeciesHIntervals);
    fourthSpeciesMelodicIntervals.update(home, s.fourthSpeciesMelodicIntervals);
    m2IntervalsArray.update(home, s.m2IntervalsArray);
    firstHInterval.update(home, s.firstHInterval);
    isNoSyncopeArray.update(home, s.isNoSyncopeArray);
    m2ZeroArray.update(home, s.m2ZeroArray);
    snycopeCostArray.update(home, s.snycopeCostArray);
}

FourthSpeciesCounterpoint* FourthSpeciesCounterpoint::clone(Home home){
    return new FourthSpeciesCounterpoint(home, *this);
}

IntVarArray FourthSpeciesCounterpoint::getBranchingNotes(){
    return fourthSpeciesNotesCp;
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
    return h_intervals.size();
}