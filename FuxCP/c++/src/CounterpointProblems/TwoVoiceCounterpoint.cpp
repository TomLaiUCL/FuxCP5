//
// Created by Damien Sprockeels on 13/06/2024.
//

#include "../../headers/CounterpointProblems/TwoVoiceCounterpoint.hpp"
#include "../../headers/CounterpointUtils.hpp"

/**
 * Constructor of the class.
 * @param cf a vector<int> representing the cantus firmus.
 * @param sp the species of the counterpoint. it takes values from the enum "species" in headers/Utilities.hpp
 * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
 * @param lb the lowest note possible for the counterpoint in MIDI
 * @param ub the highest note possible for the counterpoint in MIDI
 */
TwoVoiceCounterpoint::TwoVoiceCounterpoint(vector<int> cf, int sp, int k, int lb, int ub, int v_type, vector<int> m_costs, vector<int> g_costs, 
    vector<int> s_costs, vector<int> imp, int bm) : 
    CounterpointProblem(cf, k, lb, ub, v_type, m_costs, g_costs, s_costs, imp, TWO_VOICES){
    species = sp;
    upper = new Stratum(*this, nMeasures, 0, 127, v_type, lowest->getNotes()); 
    counterpoint_1 = create_counterpoint(*this, species, nMeasures, cf, lowerBound, upperBound, key, lowest, cantusFirmus, v_type, m_costs, g_costs, s_costs, bm, TWO_VOICES);

    /// H2 from Thibault: The first harmonic interval must be a perfect consonance
    dom(*this, counterpoint_1->getHInterval()[0], IntSet(IntArgs(PERFECT_CONSONANCES)));

    /// H3 from Thibault: The last harmonic interval must be a perfect consonance
    dom(*this, counterpoint_1->getHInterval()[counterpoint_1->getHInterval().size()-1], IntSet(IntArgs(PERFECT_CONSONANCES)));

    //rel(*this, counterpoint->getFirstNotes()[2], IRT_EQ, 76);
    //create_lowest(*this, lowest, cantusFirmus, counterpoint);
    lowest->setCantusPointer(cantusFirmus);
    lowest->setCpPointer(*this, counterpoint_1);
    lowest->setLowest(*this, upper);

    unitedCosts = IntVarArray(*this, counterpoint_1->getCosts().size(), 0, 1000000);

    for(int i = 0; i < unitedCosts.size(); i++){
        rel(*this, unitedCosts[i], IRT_EQ, counterpoint_1->getCosts()[i]);
    }

    unitedCostNames = counterpoint_1->getCostNames();

    orderCosts();

    solutionArray = IntVarArray(*this, counterpoint_1->getBranchingNotes().size(), 0, 127);
    rel(*this, solutionArray, IRT_EQ, counterpoint_1->getBranchingNotes());
    
    //upper->setNote(*this, 5, counterpoint->getFirstNotes()[5]);
    branch(*this, solutionArray, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
}
// COPY CONSTRUCTOR
TwoVoiceCounterpoint::TwoVoiceCounterpoint(TwoVoiceCounterpoint& s) : CounterpointProblem(s){
    //counterpoint = s.counterpoint;
    species = s.species;
    if (s.upper) {
        upper = s.upper->clone(*this);
    } else {
        upper = nullptr;
    }
}

IntLexMinimizeSpace* TwoVoiceCounterpoint::copy(){   // todo use 'bool share' in copy constructor?
    return new TwoVoiceCounterpoint(*this);
}


string TwoVoiceCounterpoint::to_string() const {
    string text = "";
    text += CounterpointProblem::to_string();
    return text;
}
/*
void TwoVoiceCounterpoint::uniteCosts(){
    int cp1_idx = 0;
    for(int i = 0; i < 14; i++){
        string name = importanceNames[i];
        //decides if the cost is present for any counterpoint
        bool cp1_contains = 0;
        int sz = 0;
        for(int t = 0; t < counterpoint_1->getCostNames().size(); t++){
            if(name==counterpoint_1->getCostNames()[t]){
                cp1_contains=1;
                sz++;
            }
        }
        if(!cp1_contains){
            unitedCostNames.push_back("NOT ADDED");
        } else {
            unitedCostNames.push_back(name);
            //adds the cost to the IntVarArgs
            IntVarArgs x(sz);
            int idx=0;
            if(cp1_contains){
                x[idx] = counterpoint_1->getCosts()[cp1_idx];
                idx++;
                cp1_idx++;
            }
            rel(*this, unitedCosts[i], IRT_EQ, expr(*this, sum(x)));
        }
    }
}*/