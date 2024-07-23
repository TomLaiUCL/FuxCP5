#include "../../headers/Parts/ThirdSpeciesCounterpoint.hpp"

/**
 * Third species general constructor
 */
ThirdSpeciesCounterpoint::ThirdSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, int mSpec, Stratum* low, CantusFirmus* c, 
    int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
    FirstSpeciesCounterpoint(home, size, cf, lb, ub, k, THIRD_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV)
{
    thirdSpeciesNotesCp = IntVarArray(home, notes.size(), IntSet(IntArgs(vector_intersection(cp_range, extended_domain))));
    if(borrowMode==1){
        thirdSpeciesNotesCp[thirdSpeciesNotesCp.size()-2] = IntVar(home, IntSet(IntArgs(vector_intersection(cp_range, chromatic_scale))));
    }
    rel(home, thirdSpeciesNotesCp, IRT_EQ, notes.slice(0,4/notesPerMeasure.at(THIRD_SPECIES),(notes.size())));

    thirdSpeciesHarmonicIntervals = IntVarArray(home, h_intervals.size(), UNISSON, PERFECT_OCTAVE);
    rel(home, thirdSpeciesHarmonicIntervals, IRT_EQ, h_intervals.slice(0,4/notesPerMeasure.at(THIRD_SPECIES),(h_intervals.size())));
    for(int i = 0; i < thirdSpeciesHarmonicIntervals.size(); i++){
        rel(home, (thirdSpeciesHarmonicIntervals[i])==((thirdSpeciesNotesCp[i]-low->getNotes()[floor(i/4)*4])%12));
    }

    thirdSpeciesMelodicIntervals = IntVarArray(home, m_intervals_brut.size(), -PERFECT_OCTAVE, PERFECT_OCTAVE);
    rel(home, thirdSpeciesMelodicIntervals, IRT_EQ, m_intervals_brut.slice(0,4/notesPerMeasure.at(THIRD_SPECIES),m_intervals_brut.size()));
    for(int i = 0; i < thirdSpeciesMelodicIntervals.size(); i++)
        rel(home, thirdSpeciesMelodicIntervals[i], IRT_EQ, expr(home, thirdSpeciesNotesCp[i+1] - thirdSpeciesNotesCp[i]));

    thirdSpeciesMotions = IntVarArray(home, notes.size()/4, IntSet({-1, CONTRARY_MOTION, OBLIQUE_MOTION, PARALLEL_MOTION}));
    thirdSpeciesMotionCosts = IntVarArray(home, notes.size()/4, IntSet{0, directCost, obliqueCost, contraryCost});

    for(int i = 0; i < thirdSpeciesMotions.size(); i++){
        //direct motions help creation
        BoolVar both_up = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]>0)&&(low->getMelodicIntervals()[i]>0)); //if both parts are going in the same direction
        BoolVar both_stay = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]==0)&&(low->getMelodicIntervals()[i]==0)); //if both parts are staying
        BoolVar both_down = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]<0)&&(low->getMelodicIntervals()[i]<0)); //if both parts are going down
        //oblique motions help creation
        BoolVar cf_stays_1 = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]>0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes up
        BoolVar cf_stays_2 = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]<0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes down
        BoolVar cp_stays_1 = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]==0)&&(low->getMelodicIntervals()[i]>0)); //if the lowest part goes up and one stays
        BoolVar cp_stays_2 = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]==0)&&(low->getMelodicIntervals()[i]<0)); //if the lowest part goes down and one stays
        //contrary motions help creation
        BoolVar cpd_cfu = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]<0)&&(low->getMelodicIntervals()[i]>0)); //if the cf goes up and the cp down
        BoolVar cpu_cfd = expr(home, (thirdSpeciesMelodicIntervals[(i*4)+3]>0)&&(low->getMelodicIntervals()[i]<0)); //if the cf goes down and the cp up
 
        //direct constraints
        rel(home, ((both_up || both_stay || both_down) && (isLowest[i]==1)) >> (thirdSpeciesMotions[i]==PARALLEL_MOTION));
        rel(home, ((both_up || both_stay || both_down) && (isLowest[i]==1)) >> (thirdSpeciesMotionCosts[i]==directCost));
        //oblique constraints
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (isLowest[i]==1)) >> (thirdSpeciesMotions[i]==OBLIQUE_MOTION));
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (isLowest[i]==1)) >> (thirdSpeciesMotionCosts[i]==obliqueCost));
        //contrary constraints
        rel(home, ((cpd_cfu || cpu_cfd) && (isLowest[i]==1)) >> (thirdSpeciesMotions[i]==CONTRARY_MOTION));
        rel(home, ((cpd_cfu || cpu_cfd) && (isLowest[i]==1)) >> (thirdSpeciesMotionCosts[i]==contraryCost));
        //bass constraints
        rel(home, (isLowest[i]==0) >> (thirdSpeciesMotions[i]==-1));
        rel(home, (isLowest[i]==0) >> (thirdSpeciesMotionCosts[i]==0));
    }
    
    is5QNArray = BoolVarArray(home, nMeasures-1, 0, 1);
    for(int i = 0; i < is5QNArray.size()*4; i+=4){
        BoolVar case1 = BoolVar(home, 0, 1);

        rel(home, ((abs(thirdSpeciesMelodicIntervals[i])<=2) && (abs(thirdSpeciesMelodicIntervals[i+1])<=2) && (abs(thirdSpeciesMelodicIntervals[i+2])<=2) &&
            (abs(thirdSpeciesMelodicIntervals[i+3])<=2)) >> (case1==1));
        rel(home, ((abs(thirdSpeciesMelodicIntervals[i])>2) || (abs(thirdSpeciesMelodicIntervals[i+1])>2) || (abs(thirdSpeciesMelodicIntervals[i+2])>2) ||
            (abs(thirdSpeciesMelodicIntervals[i+3])>2)) >> (case1==0));

        BoolVar case2 = BoolVar(home, 0, 1);

        rel(home, expr(home, (thirdSpeciesMelodicIntervals[i]<=0 || thirdSpeciesMelodicIntervals[i+1]<=0 || thirdSpeciesMelodicIntervals[i+2]<=0 || 
            thirdSpeciesMelodicIntervals[i+3]<=0)) && expr(home, thirdSpeciesMelodicIntervals[i]>=0 || thirdSpeciesMelodicIntervals[i+1]>=0 || 
            thirdSpeciesMelodicIntervals[i+2]>=0 || thirdSpeciesMelodicIntervals[i+3]>=0) >> (case2==0));
        rel(home, expr(home, (thirdSpeciesMelodicIntervals[i]>0 && thirdSpeciesMelodicIntervals[i+1]>0 && thirdSpeciesMelodicIntervals[i+2]>0 && 
            thirdSpeciesMelodicIntervals[i+3]>0)) || expr(home, thirdSpeciesMelodicIntervals[i]<0 && thirdSpeciesMelodicIntervals[i+1]<0 && 
            thirdSpeciesMelodicIntervals[i+2]<0 && thirdSpeciesMelodicIntervals[i+3]<0) >> (case2==1));

        rel(home, is5QNArray[i/4], IRT_EQ, expr(home, case1&&case2));
    }

    isDiminution = BoolVarArray(home, nMeasures-1, 0, 1);
    for(int i = 0; i < isDiminution.size()*4; i+=4){
        //thirdMelodic[i+1], thirdMelodic[i+2], thirdMelodicIntervals[i+1]+thirdMelodicIntervals[i+2]+thirdMelodicIntervals[i+3]+thirdMelodicIntervals[i+4]
        // mta, mat, mtt
        rel(home, (abs(thirdSpeciesMelodicIntervals[i+1])<=MAJOR_SECOND && abs(thirdSpeciesMelodicIntervals[i+2])<=MAJOR_SECOND && 
            (abs(thirdSpeciesMelodicIntervals[i+1]+thirdSpeciesMelodicIntervals[i+2]+thirdSpeciesMelodicIntervals[i+3])==MINOR_THIRD
            || abs(thirdSpeciesMelodicIntervals[i+1]+thirdSpeciesMelodicIntervals[i+2]+thirdSpeciesMelodicIntervals[i+3])==MAJOR_THIRD
            )) >> (isDiminution[i/4]==1));
        rel(home, (abs(thirdSpeciesMelodicIntervals[i+1])>MAJOR_SECOND || abs(thirdSpeciesMelodicIntervals[i+2])>MAJOR_SECOND ||
            (abs(thirdSpeciesMelodicIntervals[i+1]+thirdSpeciesMelodicIntervals[i+2]+thirdSpeciesMelodicIntervals[i+3])!=MINOR_THIRD
            && abs(thirdSpeciesMelodicIntervals[i+1]+thirdSpeciesMelodicIntervals[i+2]+thirdSpeciesMelodicIntervals[i+3])!=MAJOR_THIRD
            )) >> (isDiminution[i/4]==0));
    }

    cambiataCostArray = IntVarArray(home, nMeasures-1, IntSet({0, cambiataCost}));

    m2ZeroArray = IntVarArray(home, thirdSpeciesMelodicIntervals.size()-2, IntSet({0, m2ZeroCost}));

    //third note of the penultimate measure must be below the fourth one
    rel(home, thirdSpeciesMelodicIntervals[thirdSpeciesMelodicIntervals.size()-2], IRT_GR, 1);
    //second one must also be more distant than a semi tone from the last note of the penultimate measure
    rel(home, expr(home, thirdSpeciesMelodicIntervals[thirdSpeciesMelodicIntervals.size()-3]+thirdSpeciesMelodicIntervals[thirdSpeciesMelodicIntervals.size()-2])
        , IRT_NQ, 1);

    //3.H1 : five consecutive notes by joint degree implies that the first and the third note are consonants
    for(int i = 0; i < is5QNArray.size(); i++){
        rel(home, (is5QNArray[i]==1) >> (h_intervals[(i*4)+3]==0)||(h_intervals[(i*4)+3]==7));
    }

    //3.H2 : any dissonant note implies that it is surrounded by consonant notes
    for(int i = 0; i < isDiminution.size(); i++){
        rel(home, expr(home, (h_intervals[(i*4)+1]==UNISSON || h_intervals[(i*4)+1]==PERFECT_FIFTH) && 
            (h_intervals[(i*4)+3]==UNISSON || h_intervals[(i*4)+3]==PERFECT_FIFTH) && isDiminution[i]) || 
            expr(home, h_intervals[(i*4)+2]==UNISSON || h_intervals[(i*4)+2]==PERFECT_FIFTH));
    }

    //3.H3 : cambiata cost
    for(int i = 0; i < cambiataCostArray.size(); i++){
        rel(home, (h_intervals[(i*4)+1]==UNISSON || h_intervals[(i*4)+1]==PERFECT_FIFTH)&&(h_intervals[(i*4)+2]==UNISSON || h_intervals[(i*4)+2]==PERFECT_FIFTH)
            &&(abs(m_intervals_brut[(i*4)+1])<=2) >> (cambiataCostArray[i]==cambiataCost));
        rel(home, (h_intervals[(i*4)+1]!=UNISSON && h_intervals[(i*4)+1]!=PERFECT_FIFTH)||(h_intervals[(i*4)+2]!=UNISSON && h_intervals[(i*4)+2]!=PERFECT_FIFTH)
            ||(abs(m_intervals_brut[(i*4)+1])>2) >> (cambiataCostArray[i]==0));
    }
    
    //3.H4 : in the penultimate measure, if the cantusFirmus is in the upper part, then the h_interval of the first note should be a minor third
    rel(home, (c->getIsLowestIdx(c->getNotes().size()-2)) >> (firstSpeciesHarmonicIntervals[firstSpeciesHarmonicIntervals.size()-2]==MINOR_THIRD));

    //no melodic interval between 9 and 11
    for(int i = 0; i < nMeasures-1; i++){
        rel(home, abs(m_intervals_brut[(i*4)+3])!=MAJOR_SIXTH && abs(m_intervals_brut[(i*4)+3])!=MINOR_SEVENTH && abs(m_intervals_brut[(i*4)+3])!=MAJOR_SEVENTH);
    }

    //no chromatic motion between 3 consecutive notes
    for(int i = 1; i < thirdSpeciesMelodicIntervals.size(); i++){
        rel(home, thirdSpeciesMelodicIntervals[i]!=-1 && (thirdSpeciesMelodicIntervals[i-1]+thirdSpeciesMelodicIntervals[i])!=-2);
        rel(home, thirdSpeciesMelodicIntervals[i]!=1 && (thirdSpeciesMelodicIntervals[i-1]+thirdSpeciesMelodicIntervals[i])!=2);
    }

    //Marcel's rule

    //3.M1 : each note and its two beats further peer are preferred to be different
    //i + i+1 + i+2
    for(int i = 0; i < m2ZeroArray.size(); i++){
        rel(home, ((thirdSpeciesMelodicIntervals[i]+thirdSpeciesMelodicIntervals[i+1]+thirdSpeciesMelodicIntervals[i+2])==0) >> (m2ZeroArray[i]==m2ZeroCost));
        rel(home, ((thirdSpeciesMelodicIntervals[i]+thirdSpeciesMelodicIntervals[i+1]+thirdSpeciesMelodicIntervals[i+2])!=0) >> (m2ZeroArray[i]==0));
    }

    //no direct move for thirdmotions (for 2v and 3v)
    
    //no battuta adapted for third species
    for(int j = 0; j < thirdSpeciesMotions.size(); j++){
        rel(home, expr(home, thirdSpeciesMotions[j]==CONTRARY_MOTION && firstSpeciesHarmonicIntervals[j+1]==0 && m_intervals_brut[(j*4)+3]<-4),
            BOT_AND, isLowest[j], 0);
    }
}

ThirdSpeciesCounterpoint::ThirdSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type,
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
    ThirdSpeciesCounterpoint(home, size, cf, lb, ub, k, THIRD_SPECIES, low, c, v_type, m_costs,g_costs, s_costs, bm, nV)
{
    //3.H7,H8 adapted
    rel(home, (isLowest[isLowest.size()-2]==0) >> (h_intervals[h_intervals.size()-2]==MINOR_THIRD));
    rel(home, (isLowest[isLowest.size()-2]==1) >> (h_intervals[h_intervals.size()-2]==MAJOR_SIXTH));

    //3.P1 adapted (why does it make an endless loop???)
    //for(int j = 0; j < thirdSpeciesMotions.size(); j++){
    //    rel(home, (firstSpeciesHarmonicIntervals[j+1]==UNISSON || firstSpeciesHarmonicIntervals[j+1]==PERFECT_FIFTH) >> 
    //    (thirdSpeciesMotions[j]!=2));
    //}

    costs = IntVarArray(home, 7, 0, 10000);

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be motion cost
    add_cost(home, 2, thirdSpeciesMotionCosts, costs);
    //set cost[3] to be melodic cost
    add_cost(home, 3, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), melodicDegreeCost.size())), costs);
    //need to set cost[4] to be off cost
    add_cost(home, 4, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), offCostArray.size())), costs);
    //need to set cost[5] to be cambiata cost
    add_cost(home, 5, cambiataCostArray, costs);
    //need to set cost[5] to be cambiata cost
    add_cost(home, 6, m2ZeroArray, costs);
}

ThirdSpeciesCounterpoint::ThirdSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type,
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV1, int nV2):
    ThirdSpeciesCounterpoint(home, size, cf, lb, ub, k, THIRD_SPECIES, low, c, v_type, m_costs,g_costs, s_costs, bm, nV2)
{
    varietyCostArray = IntVarArray(home, 3*(thirdSpeciesHarmonicIntervals.size()-2), IntSet({0, varietyCost}));
    directCostArray = IntVarArray(home, thirdSpeciesMotions.size()-1,IntSet({0, directMoveCost}));

    //1.H7,H8 adapted
    rel(home, h_intervals[h_intervals.size()-1]==UNISSON||h_intervals[h_intervals.size()-1]==MINOR_THIRD||h_intervals[h_intervals.size()-1]==PERFECT_FIFTH||
        h_intervals[h_intervals.size()-1]==MAJOR_SIXTH);

    //3.H6 : harmonic triad should be used on the second or third beat
    thirdHTriadArray = IntVarArray(home, nMeasures-1, IntSet({0, triad3rdCost}));
    for(int i = 0; i < thirdHTriadArray.size(); i++){
        rel(home, ((h_intervals[(i*4)+1]!=UNISSON&&h_intervals[(i*4)+1]!=MINOR_THIRD&&h_intervals[(i*4)+1]!=MAJOR_THIRD&&h_intervals[(i*4)+1]!=PERFECT_FIFTH)&&
            (h_intervals[(i*4)+2]!=UNISSON&&h_intervals[(i*4)+2]!=MINOR_THIRD&&h_intervals[(i*4)+2]!=MAJOR_THIRD&&h_intervals[(i*4)+2]!=PERFECT_FIFTH)) >> 
            (thirdHTriadArray[i]==triad3rdCost));
        rel(home, ((h_intervals[(i*4)+1]==UNISSON||h_intervals[(i*4)+1]==MINOR_THIRD||h_intervals[(i*4)+1]==MAJOR_THIRD||h_intervals[(i*4)+1]==PERFECT_FIFTH)||
            (h_intervals[(i*4)+2]==UNISSON||h_intervals[(i*4)+2]==MINOR_THIRD||h_intervals[(i*4)+2]==MAJOR_THIRD||h_intervals[(i*4)+2]==PERFECT_FIFTH)) >> 
            (thirdHTriadArray[i]==0));
    }

    //1.P1 3 voices version
    for(int j = 0; j < firstSpeciesMotions.size()-1; j++){
        //set a cost when it is reached through direct motion, it is 0 when not
        rel(home, (thirdSpeciesMotions[j]==2&&(firstSpeciesHarmonicIntervals[j+1]==0||firstSpeciesHarmonicIntervals[j+1]==7))>>
            (directCostArray[j]==directMoveCost));
        rel(home, (thirdSpeciesMotions[j]!=2||(firstSpeciesHarmonicIntervals[j+1]!=0&&firstSpeciesHarmonicIntervals[j+1]!=7))>>
            (directCostArray[j]==0));
    }

    costs = IntVarArray(home, 10, 0, 10000);

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be motion cost
    add_cost(home, 2, thirdSpeciesMotionCosts, costs);
    //set cost[3] to be melodic cost
    add_cost(home, 3, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), melodicDegreeCost.size())), costs);
    //need to set cost[4] to be off cost
    add_cost(home, 4, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), offCostArray.size())), costs);
    //need to set cost[5] to be cambiata cost
    add_cost(home, 5, cambiataCostArray, costs);
    //need to set cost[6] to be m2Zero cost
    add_cost(home, 6, m2ZeroArray, costs);
    //need to set cost[7] to be third h triad cost
    add_cost(home, 7, thirdHTriadArray, costs);
    //need to set cost[8] to be variety cost
    add_cost(home, 8, varietyCostArray, costs);
    //need to set cost[9] to be direct cost
    add_cost(home, 9, directCostArray, costs);
    
}

ThirdSpeciesCounterpoint::ThirdSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type,
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV1, int nV2, int nV3):
    ThirdSpeciesCounterpoint(home, size, cf, lb, ub, k, THIRD_SPECIES, low, c, v_type, m_costs,g_costs, s_costs, bm, nV3)
{
    varietyCostArray = IntVarArray(home, 3*(thirdSpeciesHarmonicIntervals.size()-2), IntSet({0, varietyCost}));
    directCostArray = IntVarArray(home, thirdSpeciesMotions.size()-1,IntSet({0, directMoveCost}));

    //1.H7,H8 adapted
    rel(home, h_intervals[h_intervals.size()-1]==UNISSON||h_intervals[h_intervals.size()-1]==MINOR_THIRD||h_intervals[h_intervals.size()-1]==PERFECT_FIFTH||
        h_intervals[h_intervals.size()-1]==MAJOR_SIXTH);

    //3.H6 : harmonic triad should be used on the second or third beat
    thirdHTriadArray = IntVarArray(home, nMeasures-1, IntSet({0, triad3rdCost}));
    for(int i = 0; i < thirdHTriadArray.size(); i++){
        rel(home, ((h_intervals[(i*4)+1]!=UNISSON&&h_intervals[(i*4)+1]!=MINOR_THIRD&&h_intervals[(i*4)+1]!=MAJOR_THIRD&&h_intervals[(i*4)+1]!=PERFECT_FIFTH)&&
            (h_intervals[(i*4)+2]!=UNISSON&&h_intervals[(i*4)+2]!=MINOR_THIRD&&h_intervals[(i*4)+2]!=MAJOR_THIRD&&h_intervals[(i*4)+2]!=PERFECT_FIFTH)) >> 
            (thirdHTriadArray[i]==triad3rdCost));
        rel(home, ((h_intervals[(i*4)+1]==UNISSON||h_intervals[(i*4)+1]==MINOR_THIRD||h_intervals[(i*4)+1]==MAJOR_THIRD||h_intervals[(i*4)+1]==PERFECT_FIFTH)||
            (h_intervals[(i*4)+2]==UNISSON||h_intervals[(i*4)+2]==MINOR_THIRD||h_intervals[(i*4)+2]==MAJOR_THIRD||h_intervals[(i*4)+2]==PERFECT_FIFTH)) >> 
            (thirdHTriadArray[i]==0));
    }

    //1.P1 3 voices version
    for(int j = 0; j < firstSpeciesMotions.size()-1; j++){
        //set a cost when it is reached through direct motion, it is 0 when not
        rel(home, (thirdSpeciesMotions[j]==2&&(firstSpeciesHarmonicIntervals[j+1]==0||firstSpeciesHarmonicIntervals[j+1]==7))>>
            (directCostArray[j]==directMoveCost));
        rel(home, (thirdSpeciesMotions[j]!=2||(firstSpeciesHarmonicIntervals[j+1]!=0&&firstSpeciesHarmonicIntervals[j+1]!=7))>>
            (directCostArray[j]==0));
    }

    costs = IntVarArray(home, 10, 0, 10000);

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be motion cost
    add_cost(home, 2, thirdSpeciesMotionCosts, costs);
    //set cost[3] to be melodic cost
    add_cost(home, 3, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), melodicDegreeCost.size())), costs);
    //need to set cost[4] to be off cost
    add_cost(home, 4, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(THIRD_SPECIES), offCostArray.size())), costs);
    //need to set cost[5] to be cambiata cost
    add_cost(home, 5, cambiataCostArray, costs);
    //need to set cost[6] to be m2Zero cost
    add_cost(home, 6, m2ZeroArray, costs);
    //need to set cost[7] to be third h triad cost
    add_cost(home, 7, thirdHTriadArray, costs);
    //need to set cost[8] to be variety cost
    add_cost(home, 8, varietyCostArray, costs);
    //need to set cost[9] to be direct cost
    add_cost(home, 9, directCostArray, costs);
}

string ThirdSpeciesCounterpoint::to_string() const {
    string text = Part::to_string() + "\nThird species : \n";
    text += "Third species notes : " + intVarArray_to_string(thirdSpeciesNotesCp) + "\n";
    text += "Third species harmonic intervals : " + intVarArray_to_string(thirdSpeciesHarmonicIntervals) + "\n";
    text += "Third species melodic intervals : " + intVarArray_to_string(thirdSpeciesMelodicIntervals) + "\n";
    text += "Third species motion intervals : " + intVarArray_to_string(thirdSpeciesMotions) + "\n";
    text += "is 5 QN Array : " + boolVarArray_to_string(is5QNArray) + "\n";
    text += "is Diminution Array : " + boolVarArray_to_string(isDiminution) + "\n";
    text += "Third h triad cost : " + intVarArray_to_string(thirdHTriadArray) + "\n";
    text += "Costs: " + intVarArray_to_string(costs) + "\n";
    return text;
}

ThirdSpeciesCounterpoint::ThirdSpeciesCounterpoint(Home home, ThirdSpeciesCounterpoint &s): FirstSpeciesCounterpoint(home, s){  
    thirdSpeciesNotesCp.update(home, s.thirdSpeciesNotesCp);
    thirdSpeciesHarmonicIntervals.update(home, s.thirdSpeciesHarmonicIntervals);
    thirdSpeciesMelodicIntervals.update(home, s.thirdSpeciesMelodicIntervals);
    thirdSpeciesMotions.update(home, s.thirdSpeciesMotions);
    thirdSpeciesMotionCosts.update(home, s.thirdSpeciesMotionCosts);
    is5QNArray.update(home, s.is5QNArray);
    isDiminution.update(home, s.isDiminution);
    cambiataCostArray.update(home, s.cambiataCostArray);
    m2ZeroArray.update(home, s.m2ZeroArray);
    thirdHTriadArray.update(home, s.thirdHTriadArray);
}

ThirdSpeciesCounterpoint* ThirdSpeciesCounterpoint::clone(Home home){
    return new ThirdSpeciesCounterpoint(home, *this);
}

IntVarArray ThirdSpeciesCounterpoint::getFirstHInterval(){
    return firstSpeciesHarmonicIntervals;
}

IntVarArray ThirdSpeciesCounterpoint::getMotions(){
    return thirdSpeciesMotions;
}

IntVarArray ThirdSpeciesCounterpoint::getFirstMInterval(){
    return firstSpeciesMelodicIntervals;
}

IntVarArray ThirdSpeciesCounterpoint::getBranchingNotes(){
    return thirdSpeciesNotesCp;
}

int ThirdSpeciesCounterpoint::getHIntervalSize(){
    return thirdSpeciesHarmonicIntervals.size();
}