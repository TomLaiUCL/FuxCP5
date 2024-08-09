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
TwoVoiceCounterpoint::TwoVoiceCounterpoint(vector<int> cf, Species sp, int v_type, vector<int> m_costs, vector<int> g_costs, 
    vector<int> s_costs, vector<int> imp, int bm) : 
    CounterpointProblem(cf, v_type, m_costs, g_costs, s_costs, imp, TWO_VOICES){
    species = sp;
    upper = new Stratum(*this, nMeasures, 0, 127, lowest->getNotes()); 
    counterpoint_1 = create_counterpoint(*this, species, nMeasures, cf, (6 * v_type - 6) + cf[0], (6 * v_type + 12) + cf[0], lowest, cantusFirmus, 
        v_type, m_costs, g_costs, s_costs, bm, TWO_VOICES);

    // G6 : no chromatic melodies (works for 1st, 2nd and 3rd species)
    G6_noChromaticMelodies(*this, counterpoint_1, species);

    /// H2 from Thibault: The first harmonic interval must be a perfect consonance
    if(species!=FIFTH_SPECIES){
        H2_1_startWithPerfectConsonance(*this, counterpoint_1);
    }

    /// H3 from Thibault: The last harmonic interval must be a perfect consonance
    H3_1_endWithPerfectConsonance(*this, counterpoint_1);

    // H5 from Thibault : The cp and the cf cannot play the same note
    H5_1_cpAndCfDifferentNotes(*this, counterpoint_1, cantusFirmus);

    setLowest(nullptr, nullptr, upper, nullptr, nullptr);

    unitedCosts = IntVarArray(*this, counterpoint_1->getCosts().size(), 0, 1000000);

    for(int i = 0; i < unitedCosts.size(); i++){
        rel(*this, unitedCosts[i], IRT_EQ, counterpoint_1->getCosts()[i]);
    }

    unitedCostNames = counterpoint_1->getCostNames();

    orderCosts();

    solutionArray = IntVarArray(*this, counterpoint_1->getBranchingNotes().size(), 0, 127);
    rel(*this, solutionArray, IRT_EQ, counterpoint_1->getBranchingNotes());

    branch(*this, lowest->getNotes().slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), lowest->getNotes().size()), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    if(species==FIFTH_SPECIES){
        branch(*this, counterpoint_1->getSpeciesArray(), INT_VAR_DEGREE_MAX(), INT_VAL_MIN());
    }
    if(species==FOURTH_SPECIES){
        branch(*this, counterpoint_1->getSyncopeCostArray(),  INT_VAR_DEGREE_MAX(), INT_VAL_MIN());
    }
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
    text += "Lowest : \n";
    text += cantusFirmus->to_string();
    text += "\n";
    text += "Counterpoint 1 : \n";
    text += counterpoint_1->to_string();
    text += "\n";
    text += "Solution Array : \n";
    text += intVarArray_to_string(solutionArray);
    text += "\n";
    text += counterpoint_1->to_string();
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