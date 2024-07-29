//
// Created by Damien Sprockeels on 11/06/2024.
//

#include "../../headers/Parts/FirstSpeciesCounterpoint.hpp"

/**
 * GENERAL CONSTRUCTOR
 */

FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, int mSpecies, Stratum* low, CantusFirmus* c,
     int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV):
        Part(home, nMes, mSpecies, cf, lb, ub, k, v_type, m_costs, g_costs, s_costs, nV, bm) { /// super constructor

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

    /// First species notes in the counterpoint
    firstSpeciesNotesCp = IntVarArray(home, nMeasures * notesPerMeasure.at(FIRST_SPECIES), IntSet(IntArgs(vector_intersection(cp_range, extended_domain))));
    if(borrowMode==1 && motherSpecies==FIRST_SPECIES){
        firstSpeciesNotesCp[firstSpeciesNotesCp.size()-2] = IntVar(home, IntSet(IntArgs(vector_intersection(cp_range, chromatic_scale))));
    } else {
        firstSpeciesNotesCp[firstSpeciesNotesCp.size()-2] = IntVar(home, IntSet(IntArgs(vector_intersection(cp_range, extended_domain))));
    }
    rel(home, firstSpeciesNotesCp, IRT_EQ, notes.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),notes.size()));
    
    /// Harmonic intervals for the first species notes
    firstSpeciesHarmonicIntervals = IntVarArray(home, nMeasures* notesPerMeasure.at(FIRST_SPECIES), -PERFECT_OCTAVE, PERFECT_OCTAVE);

    rel(home, firstSpeciesHarmonicIntervals, IRT_EQ, h_intervals.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),h_intervals.size()));

    for(int i = 0; i < firstSpeciesHarmonicIntervals.size(); i++){
        rel(home, (firstSpeciesHarmonicIntervals[i])==((firstSpeciesNotesCp[i]-low->getNotes()[i*4])%12));
    }
    
    /// Melodic intervals for the first species notes
    firstSpeciesMelodicIntervals = IntVarArray(home, nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1, -PERFECT_OCTAVE, PERFECT_OCTAVE);
    /**
     * melodic intervals are linked in first species specific constructors EXCLUSIVELY, since the way m_intervals are calculated differentiates
     * from the species which extend the first species.
     */
    
    ///link melodic intervals
    for(int i = 0; i < firstSpeciesMelodicIntervals.size(); i++)
      rel(home, firstSpeciesMelodicIntervals[i], IRT_EQ, expr(home, firstSpeciesNotesCp[i+1] - firstSpeciesNotesCp[i]));
    
    //create the is off array which determines if a notes is borrowed or not
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

    //create motions arrays
    firstSpeciesMotions = IntVarArray(home, nMeasures* notesPerMeasure.at(FIRST_SPECIES) -1, IntSet{-1, CONTRARY_MOTION, OBLIQUE_MOTION, PARALLEL_MOTION});
    firstSpeciesMotionCosts = IntVarArray(home, firstSpeciesMotions.size(), IntSet{0, directCost, obliqueCost, contraryCost});

    //create motions

    for(int i = 0; i < firstSpeciesMotions.size(); i++){
        //direct motions help creation
        
        BoolVar both_up = expr(home, (firstSpeciesMelodicIntervals[i]>0)&&(low->getMelodicIntervals()[i]>0)); //if both parts are going in the same direction
        BoolVar both_stay = expr(home, (firstSpeciesMelodicIntervals[i]==0)&&(low->getMelodicIntervals()[i]==0)); //if both parts are staying
        BoolVar both_down = expr(home, (firstSpeciesMelodicIntervals[i]<0)&&(low->getMelodicIntervals()[i]<0)); //if both parts are going down
        //oblique motions help creation
        BoolVar cf_stays_1 = expr(home, (firstSpeciesMelodicIntervals[i]>0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes up
        BoolVar cf_stays_2 = expr(home, (firstSpeciesMelodicIntervals[i]<0)&&(low->getMelodicIntervals()[i]==0)); //if the lowest part stays and one goes down
        BoolVar cp_stays_1 = expr(home, (firstSpeciesMelodicIntervals[i]==0)&&(low->getMelodicIntervals()[i]>0)); //if the lowest part goes up and one stays
        BoolVar cp_stays_2 = expr(home, (firstSpeciesMelodicIntervals[i]==0)&&(low->getMelodicIntervals()[i]<0)); //if the lowest part goes down and one stays
        //contrary motions help creation
        BoolVar cpd_cfu = expr(home, (firstSpeciesMelodicIntervals[i]<0)&&(low->getMelodicIntervals()[i]>0)); //if the cf goes up and the cp down
        BoolVar cpu_cfd = expr(home, (firstSpeciesMelodicIntervals[i]>0)&&(low->getMelodicIntervals()[i]<0)); //if the cf goes down and the cp up

        //direct constraints
        rel(home, ((both_up || both_stay || both_down) && (this->isLowest[i]==1)) >> (firstSpeciesMotions[i]==PARALLEL_MOTION));
        rel(home, ((both_up || both_stay || both_down) && (this->isLowest[i]==1)) >> (firstSpeciesMotionCosts[i]==directCost));
        //oblique constraints
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (this->isLowest[i]==1)) >> (firstSpeciesMotions[i]==OBLIQUE_MOTION));
        rel(home, ((cf_stays_1 || cf_stays_2 || cp_stays_1 || cp_stays_2) && (this->isLowest[i]==1)) >> (firstSpeciesMotionCosts[i]==obliqueCost));
        //contrary constraints
        rel(home, ((cpd_cfu || cpu_cfd) && (this->isLowest[i]==1)) >> (firstSpeciesMotions[i]==CONTRARY_MOTION));
        rel(home, ((cpd_cfu || cpu_cfd) && (this->isLowest[i]==1)) >> (firstSpeciesMotionCosts[i]==contraryCost));
        //bass constraints
        rel(home, (this->isLowest[i]==0) >> (firstSpeciesMotions[i]==-1));
        rel(home, (this->isLowest[i]==0) >> (firstSpeciesMotionCosts[i]==0));
        //rel(home, firstSpeciesMotionCosts[i], IRT_EQ, 0, Reify(isLowest[i], RM_IMP));

    }
    
    // create pefectConsArray
    fifthCostArray = IntVarArray(home, h_intervals.size(), IntSet({0, h_fifthCost}));
    octaveCostArray = IntVarArray(home, h_intervals.size(), IntSet({0, h_octaveCost}));

    isConsonance = BoolVarArray(home, h_intervals.size(), 0, 1);
    for(int i = 0; i < isConsonance.size(); i++){
        rel(home, expr(home, (h_intervals[i]==UNISSON)||(h_intervals[i]==MINOR_THIRD)||(h_intervals[i]==MAJOR_THIRD)||(h_intervals[i]==PERFECT_FIFTH)||
            (h_intervals[i]==MINOR_SIXTH)||(h_intervals[i]==MAJOR_SIXTH)||(h_intervals[i]==PERFECT_OCTAVE)), IRT_EQ, isConsonance[i]);
    }
    /// General rules

    // G6 : no chromatic melodies (works for 1st, 2nd and 3rd species)
    
    for(int i = 0; i < m_intervals_brut.size()-1; i+=4/notesPerMeasure.at(motherSpecies)){
        rel(home, expr(home, m_intervals_brut[i]==1), BOT_AND, expr(home, m_intervals_brut[i+1]==1), 0);
        rel(home, expr(home, m_intervals_brut[i]==-1), BOT_AND, expr(home, m_intervals_brut[i+1]==-1), 0);
    }
    
    // G7 : melodic intervals should be small (works for 1st, 2nd and 3rd species)
    int idx = 0;
    for(int i = 0; i < m_intervals_brut.size(); i+=4/notesPerMeasure.at(motherSpecies)){
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
    
    /// Harmonic rules
    //todo put each rule in a function so it is easy to call them in different constructors depending on the species calling the first species
    /// H1 from Thibault: All harmonic intervals must be consonances
    dom(home, firstSpeciesHarmonicIntervals, IntSet(IntArgs(CONSONANCES)));
    
    //H2 and H3 are found in the TwoVoiceCounterpoint class, since these rules are 2 voice specific

    // H4 from Thibault : The key tone is tuned to the first note of the lowest strata
    rel(home, (isLowest[0]==0) >> (firstSpeciesHarmonicIntervals[0]==0));
    rel(home, (isLowest[isLowest.size()-1]==0) >> (firstSpeciesHarmonicIntervals[firstSpeciesHarmonicIntervals.size()-1]==0));

    // H5 from Thibault : The cp and the cf cannot play the same note
    for(int i = 1; i < firstSpeciesNotesCp.size()-1; i++){
        rel(home, firstSpeciesNotesCp[i], IRT_NQ, c->getNotes()[i]);
    }

    // H6 from Thibault : Imperfect consonances are preferred
    for(int i = 0; i < h_intervals.size(); i++){
        rel(home, octaveCostArray[i], IRT_EQ, h_octaveCost, Reify(expr(home, h_intervals[i]==UNISSON), RM_PMI));
        rel(home, octaveCostArray[i], IRT_EQ, 0, Reify(expr(home, h_intervals[i]!=UNISSON), RM_PMI));
        //rel(home, (h_intervals[i]==UNISSON) >> (octaveCostArray[i]==1));
        //rel(home, (h_intervals[i]!=UNISSON) >> (octaveCostArray[i]==0));

        rel(home, fifthCostArray[i], IRT_EQ, h_fifthCost, Reify(expr(home, h_intervals[i]==PERFECT_FIFTH), RM_PMI));
        rel(home, fifthCostArray[i], IRT_EQ, 0, Reify(expr(home, h_intervals[i]!=PERFECT_FIFTH), RM_PMI));
        //rel(home, (h_intervals[i]==PERFECT_FIFTH) >> (fifthCostArray[i]==1));
        //rel(home, (h_intervals[i]!=PERFECT_FIFTH) >> (fifthCostArray[i]==0));

        //rel(home, fifthCostArray[i], IRT_EQ, 0);
        //rel(home, octaveCostArray[i], IRT_EQ, 0);
    }
    
}

/**
 * 2 VOICES CONSTRUCTOR
 */

FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type
    , vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV) :
        FirstSpeciesCounterpoint(home, nMes, cf, lb, ub, k, FIRST_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV) ///call the general constructor
{
    rel(home, firstSpeciesMelodicIntervals, IRT_EQ, m_intervals_brut.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),m_intervals_brut.size()));

    //H7,H8 from Thibault : penultimate note major sixth or minor third

    int p = firstSpeciesNotesCp.size()-2;
    rel(home, firstSpeciesHarmonicIntervals[firstSpeciesHarmonicIntervals.size()-2], IRT_EQ, MAJOR_SIXTH, Reify(isLowest[isLowest.size()-2], RM_IMP));

    /// M2 from Thibault: Melodic intervals cannot exceed a minor sixth
    rel(home, firstSpeciesMelodicIntervals, IRT_LQ, MINOR_SIXTH);
    rel(home, firstSpeciesMelodicIntervals, IRT_GQ, -MINOR_SIXTH);

    /// Motion rules
    //P1 from Thibault : Perfect consonances cannot be reached by direct motion
    for(int j = 0; j < firstSpeciesMotions.size(); j++){

        rel(home, ((firstSpeciesHarmonicIntervals[j+1]==UNISSON || firstSpeciesHarmonicIntervals[j+1]==PERFECT_FIFTH)&&isLowest[j+1]==1) >>
            (firstSpeciesMotions[j]!=PARALLEL_MOTION));

    }

    //P2 from Thibault : already done when creating motions array
    
    //P3 from Thibault : no battuta
    for(int j = 0; j < firstSpeciesMotions.size(); j++){
        rel(home, expr(home, firstSpeciesMotions[j]==CONTRARY_MOTION && firstSpeciesHarmonicIntervals[j+1]==0 && firstSpeciesMelodicIntervals[j]<-4),
            BOT_AND, isLowest[j], 0);
    }

    costs = IntVarArray(home, 5, 0, 1000000);
    cost_names = {"fifth", "octave", "motion", "melodic", "borrow"};

    //set cost[0] to be fifth cost
    add_cost(home, 0, IntVarArray(home, fifthCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), fifthCostArray.size())), costs);
    //set cost[1] to be octave cost
    add_cost(home, 1, IntVarArray(home, octaveCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), octaveCostArray.size())), costs);
    //set cost[2] to be motion cost
    add_cost(home, 2, firstSpeciesMotionCosts, costs);
    //set cost[3] to be melodic cost
    add_cost(home, 3, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), melodicDegreeCost.size())), costs);
    //set cost[4] to be off cost
    add_cost(home, 4, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), offCostArray.size())), costs);
}

/**
 * 3 VOICES CONSTRUCTOR
 */

FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, Stratum* low, CantusFirmus* c,  int v_type, 
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV1, int nV2) : 
    FirstSpeciesCounterpoint(home, nMes, cf, lb, ub, k, FIRST_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV2) ///call the general constructor
{
    rel(home, firstSpeciesMelodicIntervals, IRT_EQ, m_intervals_brut.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),m_intervals_brut.size()));

    varietyCostArray = IntVarArray(home, 3*(firstSpeciesHarmonicIntervals.size()-2), IntSet({0, varietyCost}));
    directCostArray = IntVarArray(home, firstSpeciesMotions.size()-1,IntSet({0, directMoveCost}));

    /// M2 from Thibault: Melodic intervals cannot exceed a minor sixth (also include octave?)
    dom(home, firstSpeciesMelodicIntervals, IntSet({-UNISSON, -MINOR_SECOND, -MAJOR_SECOND, -MINOR_THIRD, -MAJOR_THIRD, -PERFECT_FOURTH, -TRITONE,
            -PERFECT_FIFTH, -MINOR_SIXTH, -PERFECT_OCTAVE, UNISSON, MINOR_SECOND, MAJOR_SECOND, MINOR_THIRD, MAJOR_THIRD, PERFECT_FOURTH, TRITONE,
            PERFECT_FIFTH, MINOR_SIXTH, PERFECT_OCTAVE}));

    //P1 3 voices version
    for(int j = 0; j < firstSpeciesMotions.size()-1; j++){
        //set a cost when it is reached through direct motion, it is 0 when not
        rel(home, (firstSpeciesMotions[j]==2&&(firstSpeciesHarmonicIntervals[j+1]==0||firstSpeciesHarmonicIntervals[j+1]==7))>>
            (directCostArray[j]==directMoveCost));
        rel(home, (firstSpeciesMotions[j]!=2||(firstSpeciesHarmonicIntervals[j+1]!=0&&firstSpeciesHarmonicIntervals[j+1]!=7))>>
            (directCostArray[j]==0));
    }

    //P3 from Thibault : no battuta
    for(int j = 0; j < firstSpeciesMotions.size(); j++){
        rel(home, expr(home, firstSpeciesMotions[j]==CONTRARY_MOTION && firstSpeciesHarmonicIntervals[j+1]==0 && firstSpeciesMelodicIntervals[j]<-4),
            BOT_AND, isLowest[j], 0);
    }

    costs = IntVarArray(home, 7, 0, 1000000);
    cost_names = {"borrow", "fifth", "octave", "variety", "motion", "melodic", "direct"};
    //need to set cost[0] to be off cost
    add_cost(home, 0, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), offCostArray.size())), costs);
    //set cost[1] to be fifth cost
    add_cost(home, 1, IntVarArray(home, fifthCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), fifthCostArray.size())), costs);
    //set cost[2] to be octave cost
    add_cost(home, 2, IntVarArray(home, octaveCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), octaveCostArray.size())), costs);
    //need to set cost[3] to be variety cost
    add_cost(home, 3, varietyCostArray, costs);
    //set cost[4] to be motion cost
    add_cost(home, 4, firstSpeciesMotionCosts, costs);
    //set cost[5] to be melodic cost
    add_cost(home, 5, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), melodicDegreeCost.size())), costs);
    //need to set cost[6] to be direct cost
    add_cost(home, 6, directCostArray, costs);
}

/**
 * 4 VOICES CONSTRUCTOR
 */

FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(Home home, int nMes, vector<int> cf, int lb, int ub, int k, Stratum* low, CantusFirmus* c,  int v_type, 
    vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV1, int nV2, int nV3):
    FirstSpeciesCounterpoint(home, nMes, cf, lb, ub, k, FIRST_SPECIES, low, c, v_type, m_costs, g_costs, s_costs, bm, nV3)
{
    rel(home, firstSpeciesMelodicIntervals, IRT_EQ, m_intervals_brut.slice(0,4/notesPerMeasure.at(FIRST_SPECIES),m_intervals_brut.size()));
    varietyCostArray = IntVarArray(home, 3*(firstSpeciesHarmonicIntervals.size()-2), IntSet({0, varietyCost}));
    directCostArray = IntVarArray(home, firstSpeciesMotions.size()-1,IntSet({0, directMoveCost}));

    //M2 : melodic intervals cannot exceed a minor sixth BUT also include an octave (adapted four voices version)
    dom(home, firstSpeciesMelodicIntervals, IntSet({-UNISSON, -MINOR_SECOND, -MAJOR_SECOND, -MINOR_THIRD, -MAJOR_THIRD, -PERFECT_FOURTH, -TRITONE,
            -PERFECT_FIFTH, -MINOR_SIXTH, -PERFECT_OCTAVE, UNISSON, MINOR_SECOND, MAJOR_SECOND, MINOR_THIRD, MAJOR_THIRD, PERFECT_FOURTH, TRITONE,
            PERFECT_FIFTH, MINOR_SIXTH, PERFECT_OCTAVE}));

    //P1 3 voices version
    for(int j = 0; j < firstSpeciesMotions.size()-1; j++){
        //set a cost when it is reached through direct motion, it is 0 when not
        rel(home, (firstSpeciesMotions[j]==2&&(firstSpeciesHarmonicIntervals[j+1]==0||firstSpeciesHarmonicIntervals[j+1]==7))>>
            (directCostArray[j]==directMoveCost));
        rel(home, (firstSpeciesMotions[j]!=2||(firstSpeciesHarmonicIntervals[j+1]!=0&&firstSpeciesHarmonicIntervals[j+1]!=7))>>
            (directCostArray[j]==0));
    }

    //P3 from Thibault : no battuta
    for(int j = 0; j < firstSpeciesMotions.size(); j++){
        rel(home, expr(home, firstSpeciesMotions[j]==CONTRARY_MOTION && firstSpeciesHarmonicIntervals[j+1]==0 && firstSpeciesMelodicIntervals[j]<-4),
            BOT_AND, isLowest[j], 0);
    }

    costs = IntVarArray(home, 7, 0, 1000000);
    cost_names = {"borrow", "fifth", "octave", "variety", "motion", "melodic", "direct"};
    //need to set cost[0] to be off cost
    add_cost(home, 0, IntVarArray(home, offCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), offCostArray.size())), costs);
    //set cost[1] to be fifth cost
    add_cost(home, 1, IntVarArray(home, fifthCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), fifthCostArray.size())), costs);
    //set cost[2] to be octave cost
    add_cost(home, 2, IntVarArray(home, octaveCostArray.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), octaveCostArray.size())), costs);
    //need to set cost[3] to be variety cost
    add_cost(home, 3, varietyCostArray, costs);
    //set cost[4] to be motion cost
    add_cost(home, 4, firstSpeciesMotionCosts, costs);
    //set cost[5] to be melodic cost
    add_cost(home, 5, IntVarArray(home, melodicDegreeCost.slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), melodicDegreeCost.size())), costs);
    //need to set cost[6] to be direct cost
    add_cost(home, 6, directCostArray, costs);
}

/**
 * This function returns a string with the characteristics of the counterpoint. It calls the to_string() method from
 * the Part class and adds 1st species specific characteristics.
 * @return a string representation of the current instance of the FirstSpeciesCounterpoint class.
 */
string FirstSpeciesCounterpoint::to_string() const {
    string text = Part::to_string() + "\nFirst species :\n";
    text += "First species first notes: " + intVarArray_to_string(firstSpeciesNotesCp) + "\n";
    text += "First species isLowest array : " + boolVarArray_to_string(isLowest) + "\n";
    text += "First species h intervals : " + intVarArray_to_string(firstSpeciesHarmonicIntervals) + "\n";
    return text;
}

// clone constructor
FirstSpeciesCounterpoint::FirstSpeciesCounterpoint(Home home, FirstSpeciesCounterpoint &s) : Part(home, s){
    motherSpecies = s.motherSpecies;
    //cantus = s.cantus;
    firstSpeciesNotesCp.update(home, s.firstSpeciesNotesCp);
    firstSpeciesHarmonicIntervals.update(home, s.firstSpeciesHarmonicIntervals);
    firstSpeciesMelodicIntervals.update(home, s.firstSpeciesMelodicIntervals);
    firstSpeciesMotions.update(home, s.firstSpeciesMotions);
    firstSpeciesMotionCosts.update(home, s.firstSpeciesMotionCosts);
}

FirstSpeciesCounterpoint* FirstSpeciesCounterpoint::clone(Home home){
    return new FirstSpeciesCounterpoint(home, *this);
}

IntVarArray FirstSpeciesCounterpoint::getBranchingNotes(){
    return firstSpeciesNotesCp;
}

IntVarArray FirstSpeciesCounterpoint::getFirstHInterval(){
    return firstSpeciesHarmonicIntervals;
}

IntVarArray FirstSpeciesCounterpoint::getMotions(){
    return firstSpeciesMotions;
}

IntVarArray FirstSpeciesCounterpoint::getFirstMInterval(){
    return firstSpeciesMelodicIntervals;
}

int FirstSpeciesCounterpoint::getHIntervalSize(){
    return firstSpeciesHarmonicIntervals.size();
}