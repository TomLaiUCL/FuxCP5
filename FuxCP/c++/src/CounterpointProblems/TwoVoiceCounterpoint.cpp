//
// Created by Damien Sprockeels on 13/06/2024.
// Extended and developed by Luc Cleenewerk and Diego de Patoul up to August 2024. 
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
    upper_1 = new Stratum(*this, nMeasures, 0, 127, lowest->getNotes()); 
    upper_2 = nullptr;
    upper_3 = nullptr;
    
    counterpoint_1 = create_counterpoint(*this, species, nMeasures, cf, (6 * v_type - 12) + cf[0], (6 * v_type + 12) + cf[0], lowest, cantusFirmus, 
        v_type, m_costs, g_costs, s_costs, bm, TWO_VOICES);
    counterpoint_2 = nullptr;
    counterpoint_3 = nullptr;
    
    // G6 : no chromatic melodies (works for 1st, 2nd and 3rd species)
    if (activeConstraints[V2_G6]) {
        G6_noChromaticMelodies(*this, counterpoint_1, species);
    }

    // 1.H4 (G9)
    if (activeConstraints[V2_G9]) {
        G9_lastChordSameAsFundamental(*this, lowest, cantusFirmus);
    }

    /// H2 from Thibault: The first harmonic interval must be a perfect consonance
    if (activeConstraints[V2_1H2]) {
        //we check for fifth species since it always starts with a break, so it doesn't make sense to apply the constraint in this case
        if(species!=FIFTH_SPECIES){
            H2_1_startWithPerfectConsonance(*this, counterpoint_1);
        } 
    }

    /// H3 from Thibault: The last harmonic interval must be a perfect consonance
    if (activeConstraints[V2_1H3]) {
        H3_1_endWithPerfectConsonance(*this, counterpoint_1);
    }

    // H5 from Thibault : The cp and the cf cannot play the same note
    if (activeConstraints[V2_1H5]) {
        H5_1_cpAndCfDifferentNotes(*this, counterpoint_1, cantusFirmus);
    }

    setStrata();

    unitedCosts = IntVarArray(*this, counterpoint_1->getCosts().size(), 0, 1000000);

    for(int i = 0; i < unitedCosts.size(); i++){
        rel(*this, unitedCosts[i], IRT_EQ, counterpoint_1->getCosts()[i]);
    }

    unitedCostNames = counterpoint_1->getCostNames();

    // compute combined costs
    computeCombinedCosts();

    orderCosts();

    solutionArray = IntVarArray(*this, counterpoint_1->getBranchingNotes().size(), 0, 127);
    rel(*this, solutionArray, IRT_EQ, counterpoint_1->getBranchingNotes());

    branch(*this, lowest->getNotes().slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), lowest->getNotes().size()), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    if(species==FIFTH_SPECIES){
        branch(*this, counterpoint_1->getSpeciesArray(), INT_VAR_DEGREE_MAX(), INT_VAL_RND(3U));
    }
    if(species==FIFTH_SPECIES){
        branch(*this, counterpoint_1->getCambiataCostArray(),  INT_VAR_DEGREE_MAX(), INT_VAL_MIN());
    }
    if(species==FOURTH_SPECIES || species==FIFTH_SPECIES){
        branch(*this, counterpoint_1->getSyncopeCostArray(),  INT_VAR_DEGREE_MAX(), INT_VAL_MIN());
    }
    branch(*this, solutionArray, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    

}
// COPY CONSTRUCTOR
TwoVoiceCounterpoint::TwoVoiceCounterpoint(TwoVoiceCounterpoint& s) : CounterpointProblem(s){
    species = s.species;
}

IntLexMinimizeSpace* TwoVoiceCounterpoint::copy(){   
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