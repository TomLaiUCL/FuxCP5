#include "../../headers/CounterpointProblems/FourVoiceCounterpoint.hpp"
#include "../../headers/CounterpointUtils.hpp"

FourVoiceCounterpoint::FourVoiceCounterpoint(vector<int> cf, vector<Species> sp, vector<int> v_type, vector<int> m_costs, vector<int> g_costs, 
        vector<int> s_costs, vector<int> imp, int bm):
    CounterpointProblem(cf, -1, m_costs, g_costs, s_costs, imp, FOUR_VOICES)
{
    species = sp;

    upper_1 = new Stratum(*this, nMeasures, 0, 127, lowest->getNotes(), THREE_VOICES, FOUR_VOICES);
    upper_2 = new Stratum(*this, nMeasures, 0, 127, lowest->getNotes(), THREE_VOICES, FOUR_VOICES);
    upper_3 = new Stratum(*this, nMeasures, 0, 127, lowest->getNotes(), THREE_VOICES, FOUR_VOICES);

    counterpoint_1 = create_counterpoint(*this, species[0], nMeasures, cf, (6 * v_type[0] - 6) + cf[0], (6 * v_type[0] + 12) + cf[0], lowest, 
        cantusFirmus, v_type[0], m_costs, g_costs, s_costs, bm, FOUR_VOICES);
    counterpoint_2 = create_counterpoint(*this, species[1], nMeasures, cf, (6 * v_type[1] - 6) + cf[0], (6 * v_type[1] + 12) + cf[0], lowest, 
        cantusFirmus, v_type[1], m_costs, g_costs, s_costs, bm, FOUR_VOICES);
    counterpoint_3 = create_counterpoint(*this, species[2], nMeasures, cf, (6 * v_type[2] - 6) + cf[0], (6 * v_type[2] + 12) + cf[0], lowest, 
        cantusFirmus, v_type[2], m_costs, g_costs, s_costs, bm, FOUR_VOICES);

    setLowest(counterpoint_2, counterpoint_3, upper_1, upper_2, upper_3);

    vector<Part*> parts = {cantusFirmus, counterpoint_1, counterpoint_2, counterpoint_3};

    triadCostArray = IntVarArray(*this, counterpoint_1->getFirstHInterval().size(), IntSet({0, not_harmonic_triad_cost, double_fifths_cost, double_thirds_cost,
        triad_with_octave_cost}));

    int scc_cz = 3*((cantusFirmus->getSize()/4)-1);

    bool containsThirdSpecies = 0;

    successiveCostArray = IntVarArray(*this, scc_cz, IntSet({0, counterpoint_1->getSuccCost()}));

    G9_lastChordSameAsFundamental(*this, lowest, cantusFirmus);

    for(int p = 1; p < parts.size(); p++){
        // G6 : no chromatic melodies (works for 1st, 2nd and 3rd species)
        G6_noChromaticMelodies(*this, parts[p], sp[p-1]);

        // H5 from Thibault : The cp and the cf cannot play the same note
        H5_1_cpAndCfDifferentNotes(*this, parts[p], cantusFirmus);
    }

    //H8 : harmonic triads are preferred, adapted for 4 voices
    H8_4v_preferHarmonicTriad(*this, triadCostArray, upper_1, upper_2, upper_3);
    
    //M4 variety cost (notes should be as diverse as possible)
    M4_varietyCost(*this, parts);

    //two fifth species counterpoints should be as different as possible
    if(counterpoint_1->getSpecies()==FIFTH_SPECIES && counterpoint_3->getSpecies()==FIFTH_SPECIES){
        BoolVarArray isSameSpecies = BoolVarArray(*this, counterpoint_1->getNotes().size(), 0, 1);
        IntVarArray isSameSpeciesInt = IntVarArray(*this, counterpoint_1->getNotes().size(), 0, 1);
        IntVar percentageSame = IntVar(*this, 0, counterpoint_1->getNotes().size());
        for(int i = 0; i < counterpoint_1->getNotes().size(); i++){
            rel(*this, counterpoint_1->getSpeciesArray()[i], IRT_EQ, counterpoint_3->getSpeciesArray()[i], Reify(isSameSpecies[i]));
            rel(*this, isSameSpeciesInt[i], IRT_EQ, 1, Reify(isSameSpecies[i]));
        }
        rel(*this, percentageSame, IRT_EQ, expr(*this, sum(isSameSpeciesInt)));
        rel(*this, percentageSame, IRT_LE, floor(counterpoint_1->getNotes().size()/2));
    }
    
    //P4 avoid successive perfect consonances
    P4_successiveCost(*this, parts, scc_cz, successiveCostArray, species);
    
    //P6 : no move in same direction
    if(counterpoint_1->getSpecies()!=FOURTH_SPECIES&&counterpoint_2->getSpecies()!=FOURTH_SPECIES&&counterpoint_3->getSpecies()!=FOURTH_SPECIES){
        P6_noMoveInSameDirection(*this, parts);
    }
    
    //P7 : no suxxessive ascending sixths
    P7_noSuccessiveAscendingSixths(*this, parts);

    //2.M2, have to write it here since it has a weird interaction with the third species
    M2_2_3v_melodicIntervalsNotExceedMinorSixth(*this, parts, containsThirdSpecies);

    solutionArray = IntVarArray(*this, counterpoint_1->getBranchingNotes().size() + counterpoint_2->getBranchingNotes().size() + 
        counterpoint_3->getBranchingNotes().size(), 0, 127);
    
    unitedCosts = IntVarArray(*this, 14, 0, 10000000);
    unitedCostNames = {};
    
    //TEST

    uniteCounterpoints();
    uniteCosts();

    orderCosts();
    
    branch(*this, lowest->getNotes().slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), lowest->getNotes().size()), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());

    if(species[0]==FIFTH_SPECIES){
        branch(*this, counterpoint_1->getSpeciesArray(), INT_VAR_DEGREE_MAX(), INT_VAL_RND(3U));
    }
    if(species[1]==FIFTH_SPECIES){
        branch(*this, counterpoint_2->getSpeciesArray(), INT_VAR_DEGREE_MAX(), INT_VAL_RND(3U));
    }
    if(species[2]==FIFTH_SPECIES){
        branch(*this, counterpoint_3->getSpeciesArray(), INT_VAR_DEGREE_MAX(), INT_VAL_RND(3U));
    }

    if(species[0]==FIFTH_SPECIES){
        //branch(*this, counterpoint_1->getCambiataCostArray(), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    }
    if(species[1]==FIFTH_SPECIES){
        //branch(*this, counterpoint_2->getCambiataCostArray(), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    }
    if(species[2]==FIFTH_SPECIES){
        //branch(*this, counterpoint_3->getCambiataCostArray(), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    }

    if(species[0]==FIFTH_SPECIES){
        //branch(*this, counterpoint_1->getSyncopeCostArray(), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    }
    if(species[1]==FIFTH_SPECIES){
        //branch(*this, counterpoint_2->getSyncopeCostArray(), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    }
    if(species[2]==FIFTH_SPECIES){
        //branch(*this, counterpoint_3->getSyncopeCostArray(), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());
    }
    

    if(species[0]==FOURTH_SPECIES){
        //branch(*this, counterpoint_1->getSyncopeCostArray(),  INT_VAR_DEGREE_MAX(), INT_VAL_MIN());
    }
    if(species[1]==FOURTH_SPECIES){
        //branch(*this, counterpoint_2->getSyncopeCostArray(),  INT_VAR_DEGREE_MAX(), INT_VAL_MIN());
    }
    if(species[2]==FOURTH_SPECIES){
        //branch(*this, counterpoint_3->getSyncopeCostArray(),  INT_VAR_DEGREE_MAX(), INT_VAL_MIN());
    }
    
    branch(*this, solutionArray, INT_VAR_SIZE_MIN(), INT_VAL_MIN());

}

// COPY CONSTRUCTOR
FourVoiceCounterpoint::FourVoiceCounterpoint(FourVoiceCounterpoint& s) : CounterpointProblem(s){
    species = s.species;
}

IntLexMinimizeSpace* FourVoiceCounterpoint::copy(){  
    return new FourVoiceCounterpoint(*this);
}

string FourVoiceCounterpoint::to_string() const {
    string text = CounterpointProblem::to_string();
    text += "Counterpoint 1 : \n";
    text += counterpoint_1->to_string();
    text += "\n";
    text += "Counterpoint 2 : \n";
    text += counterpoint_2->to_string();
    text += "\n";
    text += "Counterpoint 3 : \n";
    text += counterpoint_3->to_string();
    text += "\n";
    text += " Solution array : \n";
    text += intVarArray_to_string(solutionArray);
    text += "\n";
    return text;
}

void FourVoiceCounterpoint::uniteCounterpoints(){
    int idx = 0;
    for(int i = 0; i < counterpoint_1->getBranchingNotes().size(); i++){
        rel(*this, solutionArray[idx], IRT_EQ, counterpoint_1->getBranchingNotes()[i]);
        idx++;
    }
    for(int i = 0; i < counterpoint_2->getBranchingNotes().size(); i++){
        rel(*this, solutionArray[idx], IRT_EQ, counterpoint_2->getBranchingNotes()[i]);
        idx++;
    }
    for(int i = 0; i < counterpoint_3->getBranchingNotes().size(); i++){
        rel(*this, solutionArray[idx], IRT_EQ, counterpoint_3->getBranchingNotes()[i]);
        idx++;
    }
}

void FourVoiceCounterpoint::uniteCosts(){
    int cp1_idx = 0;
    int cp2_idx = 0;
    int cp3_idx = 0;
    for(int i = 0; i < 14; i++){
        string name = importanceNames[i];
        if(name=="succ"){
            unitedCostNames.push_back(name);
            rel(*this, unitedCosts[i], IRT_EQ, expr(*this, sum(IntVarArgs(successiveCostArray))));
        } else if(name=="triad"){
            unitedCostNames.push_back(name);
            rel(*this, unitedCosts[i], IRT_EQ, expr(*this, sum(IntVarArgs(triadCostArray))));
        } else {

            //decides if the cost is present for any counterpoint

            bool cp1_contains = 0;
            bool cp2_contains = 0;
            bool cp3_contains = 0;
            int sz = 0;
            for(int t = 0; t < counterpoint_1->getCostNames().size(); t++){
                if(name==counterpoint_1->getCostNames()[t]){
                    cp1_contains=1;
                    sz++;
                }
            }
            for(int t = 0; t < counterpoint_2->getCostNames().size(); t++){
                if(name==counterpoint_2->getCostNames()[t]){
                    cp2_contains=1;
                    sz++;
                }
            }
            for(int t = 0; t < counterpoint_3->getCostNames().size(); t++){
                if(name==counterpoint_3->getCostNames()[t]){
                    cp3_contains=1;
                    sz++;
                }
            }
            if(!cp1_contains && !cp2_contains && !cp3_contains){
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
                if(cp2_contains){
                    x[idx] = counterpoint_2->getCosts()[cp2_idx];
                    idx++;
                    cp2_idx++;
                }
                if(cp3_contains){
                    x[idx] = counterpoint_3->getCosts()[cp3_idx];
                    idx++;
                    cp3_idx++;
                }
                rel(*this, unitedCosts[i], IRT_EQ, expr(*this, sum(x)));
            }
        }
    }
}