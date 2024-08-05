#include "../../headers/Parts/FifthSpeciesCounterpoint.hpp"

/**
 * GENERAL CONSTRUCTOR
 */
FifthSpeciesCounterpoint::FifthSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int mSpecies, Stratum* low, CantusFirmus* c,
    int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
    Part(home, nMes, mSpecies, cf, lb, ub, v_type, m_costs, g_costs, s_costs, nV, bm)
{
    motherSpecies =         mSpecies;

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

    solutionLength = notes.size();
    speciesArray = IntVarArray(home, solutionLength, IntSet({-1, FIRST_SPECIES, SECOND_SPECIES, THIRD_SPECIES, FOURTH_SPECIES}));
    isNthSpeciesArray = BoolVarArray(home, notes.size()*5, 0, 1);
    isConstrainedArray = BoolVarArray(home, solutionLength, 0, 1);

    /**
     * CREATE THE SPECIES ARRAY
     */
    createSpeciesArrays(home);

    fifthSpeciesNotesCp = IntVarArray(home, notes.size(), IntSet(IntArgs(vector_intersection(cp_range, extended_domain))));
    if(borrowMode==1){
        fifthSpeciesNotesCp[fifthSpeciesNotesCp.size()-2] = IntVar(home, IntSet(IntArgs(vector_intersection(cp_range, chromatic_scale))));
    }
    

    fifthSpeciesHIntervals = IntVarArray(home, h_intervals.size(), -PERFECT_OCTAVE, PERFECT_OCTAVE);

    firstSpeciesHarmonicIntervals = IntVarArray(home, fifthSpeciesHIntervals.slice(0, 4, fifthSpeciesHIntervals.size()));

    for(int i = 0; i < fifthSpeciesHIntervals.size(); i++){
        rel(home, (fifthSpeciesHIntervals[i])==((fifthSpeciesNotesCp[i]-low->getNotes()[floor(i/4)*4])%12));
    }

    fifthSpeciesSuccMIntervals = IntVarArray(home, m_intervals_brut.size(), -PERFECT_OCTAVE, PERFECT_OCTAVE);

    firstSpeciesMelodicIntervals = IntVarArray(home, nMeasures-1, -PERFECT_OCTAVE, PERFECT_OCTAVE);

    for(int i = 0; i < fifthSpeciesSuccMIntervals.size(); i++){
        rel(home, fifthSpeciesSuccMIntervals[i] == (fifthSpeciesNotesCp[i+1]-fifthSpeciesNotesCp[i]));
    }

    for(int i = 0; i < firstSpeciesMelodicIntervals.size(); i++){
        rel(home, firstSpeciesMelodicIntervals[i] == (fifthSpeciesNotesCp[(i+1)*4]-fifthSpeciesNotesCp[i*4]));
    }

    fifthSpeciesMIntervals = IntVarArray(home, m_intervals_brut.size(), -16, 16);
    for(int i = 0; i < fifthSpeciesMIntervals.size(); i+=4){
        rel(home, fifthSpeciesMIntervals[i+2] == (fifthSpeciesNotesCp[i+4]-fifthSpeciesNotesCp[i+2]));
        rel(home, fifthSpeciesMIntervals[i+3] == (fifthSpeciesNotesCp[i+4]-fifthSpeciesNotesCp[i+3]));
    }

    fifthSpeciesMTAIntervals = IntVarArray(home, nMeasures-1, -16, 16);
    for(int i = 0; i < fifthSpeciesMTAIntervals.size(); i++){
        rel(home, fifthSpeciesMTAIntervals[i] == (fifthSpeciesNotesCp[(i*4)+2]-fifthSpeciesNotesCp[(i*4)]));
    }

    m2Len = ((nMeasures-1)*4)-1;
    fifthSpeciesM2Intervals = IntVarArray(home, m2Len, -16, 16);
    for(int i = 0; i < m2Len; i++){
        rel(home, fifthSpeciesM2Intervals[i] == (fifthSpeciesNotesCp[i+2]-fifthSpeciesNotesCp[i]));
    }

    fifthSpeciesMAllIntervals = IntVarArray(home, m_intervals_brut.size(), -PERFECT_OCTAVE, PERFECT_OCTAVE);
    for(int i = 0; i < fifthSpeciesMAllIntervals.size(); i++){
        rel(home, fifthSpeciesMAllIntervals[i] == (fifthSpeciesNotesCp[i+1]-fifthSpeciesNotesCp[i]));
    }
    
    fifthSpeciesMotions = IntVarArray(home, nMeasures-1, IntSet({-1, CONTRARY_MOTION, OBLIQUE_MOTION, PARALLEL_MOTION}));
    fifthSpeciesMotionsCosts = IntVarArray(home, nMeasures-1, IntSet({0, contraryCost, obliqueCost, directCost}));

    //create motions
    for(int i = 0; i < fifthSpeciesMotions.size(); i++){
        //direct motions help creation
        
        BoolVar both_up = expr(home, (fifthSpeciesMIntervals[(i*4)+3]>0)&&(low->getMelodicIntervals()[i]>0)); //if both parts are going in the same direction
        BoolVar both_stay = expr(home, (fifthSpeciesMIntervals[(i*4)+3]==0)&&(low->getMelodicIntervals()[i]==0)); //if both parts are staying
        BoolVar both_down = expr(home, (fifthSpeciesMIntervals[(i*4)+3]<0)&&(low->getMelodicIntervals()[i]<0)); //if both parts are going down
        //oblique motions help creation
        BoolVar cf_stays_1 = expr(home, (fifthSpeciesMIntervals[(i*4)+3]>0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes up
        BoolVar cf_stays_2 = expr(home, (fifthSpeciesMIntervals[(i*4)+3]<0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes down
        BoolVar cp_stays_1 = expr(home, (fifthSpeciesMIntervals[(i*4)+3]==0)&&(low->getMelodicIntervals()[i]>0)); //if the lowest part goes up and one stays
        BoolVar cp_stays_2 = expr(home, (fifthSpeciesMIntervals[(i*4)+3]==0)&&(low->getMelodicIntervals()[i]<0)); //if the lowest part goes down and one stays
        //contrary motions help creation
        BoolVar cpd_cfu = expr(home, (fifthSpeciesMIntervals[(i*4)+3]<0)&&(low->getMelodicIntervals()[i]>0)); //if the cf goes up and the cp down
        BoolVar cpu_cfd = expr(home, (fifthSpeciesMIntervals[(i*4)+3]>0)&&(low->getMelodicIntervals()[i]<0)); //if the cf goes down and the cp up

        //direct constraints
        rel(home, ((both_up || both_stay || both_down) && (this->isNotLowest[i]==1)) >> (fifthSpeciesMotions[i]==PARALLEL_MOTION));
        rel(home, ((both_up || both_stay || both_down) && (this->isNotLowest[i]==1)) >> (fifthSpeciesMotionsCosts[i]==directCost));
        //oblique constraints
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (this->isNotLowest[i]==1)) >> (fifthSpeciesMotions[i]==OBLIQUE_MOTION));
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (this->isNotLowest[i]==1)) >> (fifthSpeciesMotionsCosts[i]==obliqueCost));
        //contrary constraints
        rel(home, ((cpd_cfu || cpu_cfd) && (this->isNotLowest[i]==1)) >> (fifthSpeciesMotions[i]==CONTRARY_MOTION));
        rel(home, ((cpd_cfu || cpu_cfd) && (this->isNotLowest[i]==1)) >> (fifthSpeciesMotionsCosts[i]==contraryCost));
        //bass constraints
        rel(home, (this->isNotLowest[i]==0) >> (fifthSpeciesMotions[i]==-1));
        rel(home, (this->isNotLowest[i]==0) >> (fifthSpeciesMotionsCosts[i]==0));

    }

    is5QNArray = BoolVarArray(home, nMeasures-1, 0, 1);

    for(int i = 0; i < is5QNArray.size()*4; i+=4){

        BoolVar b1 = BoolVar(home, 0, 1);
        BoolVar b2 = BoolVar(home, 0, 1);
        BoolVar b3 = BoolVar(home, 0, 1);
        BoolVar b4 = BoolVar(home, 0, 1);
        BoolVar bb1 = BoolVar(home, 0, 1);
        BoolVar bb2 = BoolVar(home, 0, 1);
        BoolVar bb3 = BoolVar(home, 0, 1);
        BoolVar bb4 = BoolVar(home, 0, 1);
        BoolVar band1 = BoolVar(home, 0, 1);
        BoolVar band2 = BoolVar(home, 0, 1);
        BoolVar band3 = BoolVar(home, 0, 1);
        BoolVar beq1 = BoolVar(home, 0, 1);
        BoolVar beq2 = BoolVar(home, 0, 1);
        BoolVar beq3 = BoolVar(home, 0, 1);

        rel(home, expr(home, abs(fifthSpeciesMAllIntervals[i])), IRT_LQ, 2, Reify(b1));
        rel(home, expr(home, abs(fifthSpeciesMAllIntervals[i+1])), IRT_LQ, 2, Reify(b2));
        rel(home, expr(home, abs(fifthSpeciesMAllIntervals[i+2])), IRT_LQ, 2, Reify(b3));
        rel(home, expr(home, abs(fifthSpeciesMAllIntervals[i+3])), IRT_LQ, 2, Reify(b4));

        rel(home, fifthSpeciesMAllIntervals[i], IRT_GQ, 0, Reify(bb1));
        rel(home, fifthSpeciesMAllIntervals[i+1], IRT_GQ, 0, Reify(bb2));
        rel(home, fifthSpeciesMAllIntervals[i+2], IRT_GQ, 0, Reify(bb3));
        rel(home, fifthSpeciesMAllIntervals[i+3], IRT_GQ, 0, Reify(bb4));

        rel(home, b1, BOT_AND, b2, band1);
        rel(home, b3, BOT_AND, b4, band2);
        rel(home, band1, BOT_AND, band2, band3);

        rel(home, bb1, BOT_EQV, bb2, beq1);
        rel(home, bb3, BOT_EQV, bb4, beq2);
        rel(home, beq1, BOT_EQV, beq2, beq3);
        rel(home, band3, BOT_AND, beq3, is5QNArray[i/4]);
    }

    isMostlyThirdArray = BoolVarArray(home, nMeasures-1, 0, 1);
    for(int i = 0; i < isMostlyThirdArray.size(); i++){
        BoolVar b23 = BoolVar(home, 0, 1);
        /**
         * EXPLANATION : 
         * -for each measure there are 20 entries in the isNthSpeciesArray (5 possibilities for each note, and a measure has 4 notes)
         * -therefor, we multiply i by 20 to always land at the start of a measure (0 is the first note of the first measure, 20 the first of the second 
         *  measure etc etc)
         * -since each note has 5 entries to check for which type of species it is, to land on the first entry of the second note, we do 20+5
         * -now to check if the second note is of the third species, we do 20+5+3 (second measure, second note, check for third species) 
         */
        rel(home, isNthSpeciesArray[(i*20)+8], BOT_AND, isNthSpeciesArray[(i*20)+13], b23); 
        rel(home, b23, BOT_AND, isNthSpeciesArray[(i*20)+18], isMostlyThirdArray[i]);
    }

    isConsonance = BoolVarArray(home, notes.size(), 0, 1);
    isThirdSpeciesArray = BoolVarArray(home, notes.size(), 0, 1);
    isFourthSpeciesArray = BoolVarArray(home, notes.size(), 0, 1);

    //create isConsonance array
    for(int i = 0; i < isConsonance.size(); i++){
        rel(home, expr(home, (h_intervals[i]==UNISSON)||(h_intervals[i]==MINOR_THIRD)||(h_intervals[i]==MAJOR_THIRD)||(h_intervals[i]==PERFECT_FIFTH)||
            (h_intervals[i]==MINOR_SIXTH)||(h_intervals[i]==MAJOR_SIXTH)||(h_intervals[i]==PERFECT_OCTAVE)), IRT_EQ, isConsonance[i]);
    }

    //create isThird and isFourth arrays
    for(int i = 0; i < notes.size(); i++){
        rel(home, isThirdSpeciesArray[i], IRT_EQ, isNthSpeciesArray[(i*5)+3]);
        rel(home, isFourthSpeciesArray[i], IRT_EQ, isNthSpeciesArray[(i*5)+4]);
    }

    isDiminution = BoolVarArray(home, nMeasures-1, 0, 1);
    for(int i = 0; i < isDiminution.size()*4; i+=4){

        BoolVar btt3 = BoolVar(home, 0, 1);
        BoolVar btt4 = BoolVar(home, 0, 1);
        BoolVar bta2nd = BoolVar(home, 0, 1);
        BoolVar btt3rd = BoolVar(home, 0, 1);
        BoolVar bat2nd = BoolVar(home, 0, 1);
        BoolVar band = BoolVar(home, 0, 1);
        IntVar addition = expr(home, abs(fifthSpeciesM2Intervals[i+1]));
        rel(home, addition, IRT_EQ, 3, Reify(btt3));
        rel(home, addition, IRT_EQ, 4, Reify(btt4));
        rel(home, fifthSpeciesSuccMIntervals[i+1], IRT_LQ, 2, Reify(bta2nd));
        rel(home, fifthSpeciesSuccMIntervals[i+2], IRT_LQ, 2, Reify(bat2nd));
        rel(home, btt3, BOT_OR, btt4, btt3rd);
        rel(home, bta2nd, BOT_AND, btt3rd, band);
        rel(home, band, BOT_AND, bat2nd, isDiminution[i/4]);

    }

    //set cambiata array
    isNotCambiata = BoolVarArray(home, nMeasures-1, 0, 1);
    for(int i = 0; i < isNotCambiata.size(); i++){
        rel(home, ((fifthSpeciesHIntervals[(i*4)+1]==UNISSON || fifthSpeciesHIntervals[(i*4)+1]==PERFECT_FIFTH)&&(fifthSpeciesHIntervals[(i*4)+2]==UNISSON || fifthSpeciesHIntervals[(i*4)+2]==PERFECT_FIFTH)
            &&(abs(fifthSpeciesSuccMIntervals[(i*4)+1])<=2)) >> (isNotCambiata[i]==1));
        rel(home, ((fifthSpeciesHIntervals[(i*4)+1]!=UNISSON && fifthSpeciesHIntervals[(i*4)+1]!=PERFECT_FIFTH)||(fifthSpeciesHIntervals[(i*4)+2]!=UNISSON && fifthSpeciesHIntervals[(i*4)+2]!=PERFECT_FIFTH)
            ||(abs(fifthSpeciesSuccMIntervals[(i*4)+1])>2)) >> (isNotCambiata[i]==0));
    }

    is_off = BoolVarArray(home, notes.size(), 0, 1);
    initializeIsOffArray(home, this);

    isNoSyncopeArray = BoolVarArray(home, nMeasures-1, 0, 1);
    for(int i = 0; i < isNoSyncopeArray.size(); i++){
        rel(home, fifthSpeciesMIntervals[(i*2)+2], IRT_EQ, 0, Reify(isNoSyncopeArray[i]));
    }
    rel(home, fifthSpeciesNotesCp, IRT_EQ, notes.slice(0,4/notesPerMeasure.at(FIFTH_SPECIES),(notes.size())));
    rel(home, fifthSpeciesHIntervals, IRT_EQ, h_intervals.slice(0,4/notesPerMeasure.at(FIFTH_SPECIES),h_intervals.size()));
    rel(home, fifthSpeciesSuccMIntervals, IRT_EQ, m_intervals_brut.slice(0,4/notesPerMeasure.at(FIFTH_SPECIES),m_intervals_brut.size()));

    //Only one possible value for non constrained variables
    for(int i = 0; i < fifthSpeciesNotesCp.size()-1; i++){
        rel(home, (isConstrainedArray[i]==0) >> (fifthSpeciesNotesCp[i]==fifthSpeciesNotesCp[i+1]));
    }

    //Third note of penult measure must be below the fourth one
    rel(home, fifthSpeciesSuccMIntervals[fifthSpeciesSuccMIntervals.size()-3], IRT_GR, MINOR_SECOND, 
        Reify(isThirdSpeciesArray[isThirdSpeciesArray.size()-2], RM_IMP));
    //Second and third note distant by more than one semi tone from fourth note
    rel(home, expr(home, abs(fifthSpeciesM2Intervals[fifthSpeciesM2Intervals.size()-2])), IRT_NQ, 1, 
        Reify(isThirdSpeciesArray[isThirdSpeciesArray.size()-2], RM_IMP));
}

FifthSpeciesCounterpoint::FifthSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, Stratum* low, CantusFirmus* c,  int v_type, 
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
FifthSpeciesCounterpoint(home, nMes, cf, lb, ub, FIFTH_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV)
{
    //following two rel are H2_1 but modified, so we check in TwoVoiceProblem that that constrained isn't activated if fifth species
    //if the first note is constrained, then it must be a perfect consonance (works)
    rel(home, (isConstrainedArray[0]==1) >> (fifthSpeciesHIntervals[0]==UNISSON || fifthSpeciesHIntervals[0]==PERFECT_FIFTH));
    //else if the first note is not constrained, then the third note must be a perfect consonance (works)
    rel(home, (isConstrainedArray[0]==0) >> (fifthSpeciesHIntervals[2]==UNISSON || fifthSpeciesHIntervals[2]==PERFECT_FIFTH));

    //H3_1 can be activated as-is

    //penultimate note (only applies if it is 3rd species)
    BoolVar bandThen_1 = BoolVar(home, 0, 1);
    BoolVar testNot_1 = BoolVar(home, 0, 1);
    BoolVar bandElse_1 = BoolVar(home, 0, 1);
    BoolVar test_1 = expr(home, c->getNotes()[c->getNotes().size()-2] <= fifthSpeciesNotesCp[fifthSpeciesNotesCp.size()-2]);

    rel(home, test_1, BOT_AND, isThirdSpeciesArray[isThirdSpeciesArray.size()-2], bandThen_1);
    rel(home, test_1, BOT_EQV, BoolVar(home, 0, 0), testNot_1);
    rel(home, testNot_1, BOT_AND, isThirdSpeciesArray[isThirdSpeciesArray.size()-2], bandElse_1);

    rel(home, expr(home, fifthSpeciesNotesCp[fifthSpeciesNotesCp.size()-2]-c->getNotes()[c->getNotes().size()-2]), IRT_EQ, MAJOR_SIXTH, Reify(bandThen_1, RM_IMP));
    rel(home, expr(home, fifthSpeciesNotesCp[fifthSpeciesNotesCp.size()-2]-c->getNotes()[c->getNotes().size()-2]), IRT_EQ, MINOR_THIRD, Reify(bandElse_1, RM_IMP));

    //penultimate note (only applies if it is 4th species)
    BoolVar bandThen = BoolVar(home, 0, 1);
    BoolVar testNot = BoolVar(home, 0, 1);
    BoolVar bandElse = BoolVar(home, 0, 1);
    BoolVar test = expr(home, c->getNotes()[c->getNotes().size()-2] <= fifthSpeciesNotesCp[fifthSpeciesNotesCp.size()-3]);

    rel(home, test, BOT_AND, isThirdSpeciesArray[isThirdSpeciesArray.size()-2], bandThen);
    rel(home, test, BOT_EQV, BoolVar(home, 0, 0), testNot);
    rel(home, testNot, BOT_AND, isThirdSpeciesArray[isThirdSpeciesArray.size()-2], bandElse);
    rel(home, expr(home, fifthSpeciesNotesCp[fifthSpeciesNotesCp.size()-3]-c->getNotes()[c->getNotes().size()-2]), IRT_EQ, MAJOR_SIXTH, Reify(bandThen, RM_IMP));
    rel(home, expr(home, fifthSpeciesNotesCp[fifthSpeciesNotesCp.size()-3]-c->getNotes()[c->getNotes().size()-2]), IRT_EQ, MINOR_THIRD, Reify(bandElse, RM_IMP));



    costs = IntVarArray(home, 1, 0, 1000000);
    cost_names = {"motion"};

    //set cost[0] to be motion cost
    add_cost(home, 0, fifthSpeciesMotionsCosts, costs);

}

/**
 * This function returns a string with the characteristics of the counterpoint. It calls the to_string() method from
 * the Part class and adds 1st species specific characteristics.
 * @return a string representation of the current instance of the FirstSpeciesCounterpoint class.
 */
string FifthSpeciesCounterpoint::to_string() const {
    string text = Part::to_string() + "\n Fifth species :\n";
    text += "Fifth species notes : " + intVarArray_to_string(fifthSpeciesNotesCp) + "\n";
    text += "is Constrained array : " + boolVarArray_to_string(isConstrainedArray) + "\n";
    text += "Species array : " + intVarArray_to_string(speciesArray) + "\n";
    return text;
}

// clone constructor
FifthSpeciesCounterpoint::FifthSpeciesCounterpoint(Home home, FifthSpeciesCounterpoint &s) : Part(home, s){
    motherSpecies = s.motherSpecies;
    solutionLength = s.solutionLength;
    m2Len = s.m2Len;
    fifthSpeciesNotesCp.update(home, s.fifthSpeciesNotesCp);
    fifthSpeciesHIntervals.update(home, s.fifthSpeciesHIntervals);
    firstHInterval.update(home, s.firstHInterval);
    isNthSpeciesArray.update(home, s.isNthSpeciesArray);
    isConstrainedArray.update(home, s.isConstrainedArray);
    fifthSpeciesSuccMIntervals.update(home, s.fifthSpeciesSuccMIntervals);
    fifthSpeciesMIntervals.update(home, s.fifthSpeciesMIntervals);
    fifthSpeciesMTAIntervals.update(home, s.fifthSpeciesMTAIntervals);
    fifthSpeciesM2Intervals.update(home, s.fifthSpeciesM2Intervals);
    fifthSpeciesMAllIntervals.update(home, s.fifthSpeciesMAllIntervals);
    fifthSpeciesMotions.update(home, s.fifthSpeciesMotions);
    fifthSpeciesMotionsCosts.update(home, s.fifthSpeciesMotionsCosts);
    isMostlyThirdArray.update(home, s.isMostlyThirdArray);
    isFourthSpeciesArray.update(home, s.isFourthSpeciesArray);
    isThirdSpeciesArray.update(home, s.isThirdSpeciesArray);
    isNotCambiata.update(home, s.isNotCambiata);
}

FifthSpeciesCounterpoint* FifthSpeciesCounterpoint::clone(Home home){
    return new FifthSpeciesCounterpoint(home, *this);
}

IntVarArray FifthSpeciesCounterpoint::getBranchingNotes(){
    return fifthSpeciesNotesCp;
}

IntVarArray FifthSpeciesCounterpoint::getFirstHInterval(){
    return firstSpeciesHarmonicIntervals;
}

IntVarArray FifthSpeciesCounterpoint::getMotions(){
    return fifthSpeciesMotions;
}

IntVarArray FifthSpeciesCounterpoint::getFirstMInterval(){
    return firstSpeciesMelodicIntervals;
}

int FifthSpeciesCounterpoint::getHIntervalSize(){
    return fifthSpeciesHIntervals.size();
}

void FifthSpeciesCounterpoint::createSpeciesArrays(Home home){
    IntVarArray countThird = IntVarArray(home, solutionLength, 0, 1);
    IntVarArray countFourth = IntVarArray(home, solutionLength, 0, 1);
    int nThirdInt = floor((prefSlider-1)*0.66);
    int nFourthInt = floor(prefSlider*0.5);
    IntVar sumThird = IntVar(home, nThirdInt, solutionLength);
    IntVar sumFourth = IntVar(home, nFourthInt, solutionLength);
    
    //Count 3rd and 4th species
    for(int i = 0; i < solutionLength; i++){
        rel(home, (speciesArray[i]==THIRD_SPECIES) >> (countThird[i]==1));
        rel(home, (speciesArray[i]!=THIRD_SPECIES) >> (countThird[i]==0));

        rel(home, (speciesArray[i]==FOURTH_SPECIES) >> (countFourth[i]==1));
        rel(home, (speciesArray[i]!=FOURTH_SPECIES) >> (countFourth[i]==0));
    }
    
    //sum the counts
    //rel(home, sumThird, IRT_EQ, expr(home, sum(countThird)));
    //rel(home, sumFourth, IRT_EQ, expr(home, sum(countFourth)));

    //no 4th species in second and fourth positions in the species array
    for(int i = 1; i < solutionLength; i+=2){
        rel(home, speciesArray[i], IRT_NQ, FOURTH_SPECIES);
    }

    //4th species in the third position is followed by no species (no note / constraint) and then a 4th species
    for(int i = 0; i < solutionLength-1; i+=4){
        BoolVar b34 = BoolVar(home, 0, 1);
        BoolVar b14 = BoolVar(home, 0, 1);

        rel(home, speciesArray[i+2], IRT_EQ, FOURTH_SPECIES, Reify(b34));
        rel(home, speciesArray[i+4], IRT_EQ, FOURTH_SPECIES, Reify(b14));
        rel(home, speciesArray[i+3], IRT_EQ, -1, Reify(b34, RM_IMP));
        rel(home, b34, BOT_EQV, b14, 1);
    }
   
    //only 3rd and 4th species
    rel(home, speciesArray, IRT_NQ, FIRST_SPECIES);
    rel(home, speciesArray, IRT_NQ, SECOND_SPECIES);
 
    //first and penultimate measures are 4th species
    rel(home, speciesArray[0], IRT_EQ, -1);
    rel(home, speciesArray[1], IRT_EQ, -1);
    rel(home, speciesArray[2], IRT_EQ, FOURTH_SPECIES);

    rel(home, speciesArray[solutionLength-5], IRT_EQ, FOURTH_SPECIES);
    rel(home, speciesArray[solutionLength-4], IRT_EQ, -1);
    rel(home, speciesArray[solutionLength-3], IRT_EQ, FOURTH_SPECIES);
    
    //no silence after third species notes
    for(int i = 0; i < solutionLength-1; i++){
        BoolVar b = BoolVar(home, 0, 1);
        rel(home, speciesArray[i], IRT_EQ, THIRD_SPECIES, Reify(b));
        rel(home, speciesArray[i+1], IRT_NQ, -1, Reify(b, RM_IMP));
    }

    //no silence after fourth species notes
    for(int i = 0; i < solutionLength-5; i++){
        BoolVar b = BoolVar(home, 0, 1);
        rel(home, speciesArray[i], IRT_EQ, FOURTH_SPECIES, Reify(b));
        rel(home, speciesArray[i+4], IRT_NQ, -1, Reify(b, RM_IMP));
    }

    //maximum two consecutive measures without a fourth species
    for(int i = 2; i < solutionLength-13; i+=4){
        BoolVar b1n4 = BoolVar(home, 0, 1);
        BoolVar b2n4 = BoolVar(home, 0, 1);
        BoolVar band = BoolVar(home, 0, 1);

        rel(home, speciesArray[i+4], IRT_NQ, FOURTH_SPECIES, Reify(b1n4));
        rel(home, speciesArray[i+8], IRT_NQ, FOURTH_SPECIES, Reify(b2n4));
        rel(home, b1n4, BOT_AND, b2n4, band);
        rel(home, speciesArray[i+12], IRT_EQ, FOURTH_SPECIES, Reify(band, RM_IMP));
    }
    
    /**
     * CREATE NTH SPECIES ARRAY
     */
    
    for(int i = 0; i < isNthSpeciesArray.size(); i+=5){
        rel(home, speciesArray[i/5], IRT_EQ, -1, Reify(isNthSpeciesArray[i]));
        rel(home, speciesArray[i/5], IRT_EQ, FIRST_SPECIES, Reify(isNthSpeciesArray[i+1]));
        rel(home, speciesArray[i/5], IRT_EQ, SECOND_SPECIES, Reify(isNthSpeciesArray[i+2]));
        rel(home, speciesArray[i/5], IRT_EQ, THIRD_SPECIES, Reify(isNthSpeciesArray[i+3]));
        rel(home, speciesArray[i/5], IRT_EQ, FOURTH_SPECIES, Reify(isNthSpeciesArray[i+4]));
    }
    
    /**
     * CREATE IS CONSTRAINED ARRAY
     */
    for(int i = 0; i < isConstrainedArray.size(); i++){
        rel(home, isNthSpeciesArray[i*5], IRT_EQ, 0, Reify(isConstrainedArray[i]));
    }
}