#include "../../headers/CounterpointProblems/FourVoiceCounterpoint.hpp"
#include "../../headers/CounterpointUtils.hpp"

FourVoiceCounterpoint::FourVoiceCounterpoint(vector<int> cf, vector<int> sp, int k, int lb, int ub, vector<int> v_type, vector<int> m_costs, vector<int> g_costs, 
        vector<int> s_costs, int bm):
    CounterpointProblem(cf, k, lb, ub, -1, m_costs, g_costs, s_costs, FOUR_VOICES)
{
    species = sp;

    upper1 = new Stratum(*this, nMeasures, 0, 127, -1, lowest->getNotes(), THREE_VOICES, FOUR_VOICES);
    upper2 = new Stratum(*this, nMeasures, 0, 127, -1, lowest->getNotes(), THREE_VOICES, FOUR_VOICES);
    upper3 = new Stratum(*this, nMeasures, 0, 127, -1, lowest->getNotes(), THREE_VOICES, FOUR_VOICES);
    counterpoint_1 = create_counterpoint(*this, species[0], nMeasures, cf, (6 * v_type[0] - 6) + cf[0], (6 * v_type[0] + 12) + cf[0], key, lowest, 
        cantusFirmus, v_type[0], m_costs, g_costs, s_costs, bm, FOUR_VOICES);
    counterpoint_2 = create_counterpoint(*this, species[1], nMeasures, cf, (6 * v_type[1] - 6) + cf[0], (6 * v_type[1] + 12) + cf[0], key, lowest, 
        cantusFirmus, v_type[1], m_costs, g_costs, s_costs, bm, FOUR_VOICES);
    counterpoint_3 = create_counterpoint(*this, species[2], nMeasures, cf, (6 * v_type[2] - 6) + cf[0], (6 * v_type[2] + 12) + cf[0], key, lowest, 
        cantusFirmus, v_type[2], m_costs, g_costs, s_costs, bm, FOUR_VOICES);

    vector<Part*> parts = {cantusFirmus, counterpoint_1, counterpoint_2, counterpoint_3};

    triadCostArray = IntVarArray(*this, counterpoint_1->getFirstHInterval().size(), IntSet({0, not_harmonic_triad_cost, double_fifths_cost, double_thirds_cost,
        triad_with_octave_cost}));

    //H8 : harmonic triads are preferred, adapted for 4 voices

    for(int i = 0; i < triadCostArray.size(); i++){

        IntVar H_b = upper1->getHInterval()[i*4];
        IntVar H_c = upper2->getHInterval()[i*4];
        IntVar H_d = upper3->getHInterval()[i*4];


        BoolVar H_b_is_third    = expr(*this, H_b==MINOR_THIRD || H_b==MAJOR_THIRD);
        BoolVar H_b_is_fifth    = expr(*this, H_b==PERFECT_FIFTH);
        BoolVar H_b_is_octave   = expr(*this, H_b==UNISSON);

        BoolVar H_c_is_third    = expr(*this, H_c==MINOR_THIRD || H_c==MAJOR_THIRD);
        BoolVar H_c_is_fifth    = expr(*this, H_c==PERFECT_FIFTH);
        BoolVar H_c_is_octave   = expr(*this, H_c==UNISSON);

        BoolVar H_d_is_third    = expr(*this, H_d==MINOR_THIRD || H_d==MAJOR_THIRD);
        BoolVar H_d_is_fifth    = expr(*this, H_d==PERFECT_FIFTH);
        BoolVar H_d_is_octave   = expr(*this, H_d==UNISSON);


        // worst case : not a harmonic triad with at least a third and a fifth
        BoolVar no_fifth_or_no_third = expr(*this, H_b_is_third + H_c_is_third + H_d_is_third == 0 || H_b_is_fifth + H_c_is_fifth + H_d_is_fifth == 0);
        BoolVar note_outside_harmonic_triad = expr(*this, H_b_is_third + H_b_is_fifth + H_b_is_octave == 0 || H_c_is_third + H_c_is_fifth + H_c_is_octave == 0 || H_d_is_third + H_d_is_fifth + H_d_is_octave == 0);

        rel(*this, (note_outside_harmonic_triad || no_fifth_or_no_third) >> (triadCostArray[i] == not_harmonic_triad_cost));

        // now we are left with only combinations with at a third, a fifth, and another note of the harmonic triad. 
        // Doubling the fifth
        rel(*this, expr(*this, H_b_is_fifth + H_c_is_fifth + H_d_is_fifth == 2) >> (triadCostArray[i] == double_fifths_cost));

        // Doubling the third, and ensure it is the same type of third (not a major and a minor)
        rel(*this, expr(*this, H_b_is_third + H_c_is_third + H_d_is_third == 2) >> (triadCostArray[i] == double_thirds_cost));
        rel(*this, (H_b_is_third && H_c_is_third) >> (H_b == H_c));
        rel(*this, (H_b_is_third && H_d_is_third) >> (H_b == H_d));
        rel(*this, (H_c_is_third && H_d_is_third) >> (H_c == H_d));

        // triad with octave
        rel(*this, (H_b_is_fifth && H_c_is_third && H_d_is_octave) >> (triadCostArray[i] == triad_with_octave_cost)); // 5 3 8
        rel(*this, (H_b_is_third && H_c_is_octave && H_d_is_fifth) >> (triadCostArray[i] == triad_with_octave_cost)); // 3 8 5
        rel(*this, (H_b_is_third && H_c_is_fifth && H_d_is_octave) >> (triadCostArray[i] == triad_with_octave_cost)); // 3 5 8
        rel(*this, (H_b_is_octave && H_c_is_third && H_d_is_fifth) >> (triadCostArray[i] == triad_with_octave_cost)); // 8 3 5
        rel(*this, (H_b_is_octave && H_c_is_fifth && H_d_is_third) >> (triadCostArray[i] == triad_with_octave_cost)); // 8 5 3

        // Best case : 5 8 3
        rel(*this, (H_b_is_fifth && H_c_is_octave && H_d_is_third) >> (triadCostArray[i] == 0)); 
    
    }
    
    //M4 variety cost (notes should be as diverse as possible)
    for(int i = 1; i < parts.size(); i++){
        Part* p = parts[i];
        int mSpec = p->getSpecies();
        int temp = 0;
        IntVarArray notes = p->getBranchingNotes();
        for(int j = 0; j < p->getHIntervalSize()-1; j++){
            int upbnd = 0;
            if(j+3<p->getHIntervalSize()){
                upbnd = j+4;
            } else {upbnd = p->getHIntervalSize();}
            for(int k = j+1; k < upbnd;k++){
                //setting a cost if notes inside a window are the same in a part
                rel(*this, (notes[j]==notes[k])>>(p->getVarietyArray(temp)==p->getVarietyCost()));
                rel(*this, (notes[j]!=notes[k])>>(p->getVarietyArray(temp)==0));
                temp++;
            }
            
        }
    }
    
    //P4 avoid successive perfect consonances
    int scc_cz = 3*((cantusFirmus->getSize()/4)-1);

    successiveCostArray = IntVarArray(*this, scc_cz, IntSet({0, counterpoint_1->getSuccCost()}));
    
    int idx = 0;
    
    // cout << "SUCC SIZE :" + to_string(succ_cost.size()) << endl;
    for(int p1 = 1; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){

            if(parts[p1]->getSpecies()!=SECOND_SPECIES && parts[p2]->getSpecies()!=SECOND_SPECIES){
                
                for(int i = 0; i < parts[p1]->getFirstHInterval().size()-1; i++){
                    // cout << "IDX : " + to_string(idx) << endl;
                    rel(*this, successiveCostArray[idx], IRT_EQ, counterpoint_1->getSuccCost(), 
                        Reify(expr(*this, (parts[p1]->getFirstHInterval()[i]==0 || parts[p1]->getFirstHInterval()[i]==7) &&
                            (parts[p2]->getFirstHInterval()[i]==0 || parts[p2]->getFirstHInterval()[i]==7))));
                    idx++;
                }        
            }
            else if(parts[p1]->getSpecies()==SECOND_SPECIES){
                
                for(int i = 0; i < parts[p1]->getFirstHInterval().size()-1; i++){
                    BoolVar case1 = expr(*this, ((parts[p1]->getFirstHInterval()[i]==UNISSON || parts[p1]->getFirstHInterval()[i]==PERFECT_FIFTH) && 
                    (parts[p2]->getFirstHInterval()[i]==UNISSON || parts[p2]->getFirstHInterval()[i]==PERFECT_FIFTH)) && 
                        ((parts[p1]->getFirstHInterval()[i+1]==UNISSON || parts[p1]->getFirstHInterval()[i+1]==PERFECT_FIFTH) && 
                    (parts[p2]->getFirstHInterval()[i+1]==UNISSON || parts[p2]->getFirstHInterval()[i+1]==PERFECT_FIFTH)));
                    BoolVar case2 = expr(*this, (parts[p1]->getFirstHInterval()[i]!=PERFECT_FIFTH || parts[p2]->getFirstHInterval()[i]!=PERFECT_FIFTH) || 
                        (parts[p1]->getFirstHInterval()[i+1]!=PERFECT_FIFTH || parts[p2]->getFirstHInterval()[i+1]!=PERFECT_FIFTH) || 
                        ((parts[p1]->getSecondHInterval()[i]==3 || parts[p2]->getSecondHInterval()[i]==3) && 
                        (parts[p1]->getSecondHInterval()[i]==4 || parts[p2]->getSecondHInterval()[i]==4)));
                    //first expression states that the melodic succ interval is not a third, second that we have successive fifths
                    rel(*this, successiveCostArray[idx], IRT_EQ, counterpoint_1->getSuccCost(), Reify(expr(*this, (case1==1 || case2==1)), RM_IMP));
                    idx++;
                }
            }
            else if(parts[p2]->getSpecies()==SECOND_SPECIES){
                for(int i = 0; i < parts[p1]->getFirstHInterval().size()-1; i++){
                    BoolVar case1 = expr(*this, ((parts[p1]->getFirstHInterval()[i]==UNISSON || parts[p1]->getFirstHInterval()[i]==PERFECT_FIFTH) && 
                    (parts[p2]->getFirstHInterval()[i]==UNISSON || parts[p2]->getFirstHInterval()[i]==PERFECT_FIFTH)) && 
                        ((parts[p1]->getFirstHInterval()[i+1]==UNISSON || parts[p1]->getFirstHInterval()[i+1]==PERFECT_FIFTH) && 
                    (parts[p2]->getFirstHInterval()[i+1]==UNISSON || parts[p2]->getFirstHInterval()[i+1]==PERFECT_FIFTH)));
                    BoolVar case2 = expr(*this, (parts[p1]->getFirstHInterval()[i]!=PERFECT_FIFTH || parts[p2]->getFirstHInterval()[i]!=PERFECT_FIFTH) || 
                        (parts[p1]->getFirstHInterval()[i+1]!=PERFECT_FIFTH || parts[p2]->getFirstHInterval()[i+1]!=PERFECT_FIFTH) || 
                        ((parts[p1]->getSecondHInterval()[i]==3 || parts[p2]->getSecondHInterval()[i]==3) && 
                        (parts[p1]->getSecondHInterval()[i]==4 || parts[p2]->getSecondHInterval()[i]==4)));
                    //first expression states that the melodic succ interval is not a third, second that we have successive fifths
                    rel(*this, successiveCostArray[idx], IRT_EQ, counterpoint_1->getSuccCost(), Reify(expr(*this, (case1==1 || case2==1)), RM_IMP));
                    idx++;
                }
            }
        }
    }

    //P6 : no move in same direction
    for(int i = 0; i < parts[0]->getMotions().size(); i++){
        rel(*this, expr(*this, parts[0]->getMotions()[i]==2 && parts[1]->getMotions()[i]==2), BOT_AND, expr(*this, parts[2]->getMotions()[i]==2), 0);
    }

    //P7 : no suxxessive ascending sixths
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            for(int j = 1; j < parts[p1]->getFirstHInterval().size()-1; j++){
                rel(*this, ((parts[p1]->getFirstHInterval()[j-1]!=MINOR_SIXTH && parts[p1]->getFirstHInterval()[j-1]!=MAJOR_SIXTH) && 
                    (parts[p2]->getFirstHInterval()[j-1]!=MINOR_SIXTH && parts[p2]->getFirstHInterval()[j-1]!=MAJOR_SIXTH)) || (
                        (parts[p1]->getFirstHInterval()[j]!=MINOR_SIXTH && parts[p1]->getFirstHInterval()[j]!=MAJOR_SIXTH) && 
                    (parts[p2]->getFirstHInterval()[j]!=MINOR_SIXTH && parts[p2]->getFirstHInterval()[j]!=MAJOR_SIXTH)) || (
                        parts[p1]->getFirstMInterval()[j]>0 || parts[p2]->getFirstMInterval()[j] > 0));
            }
        }
    }

    solutionArray = IntVarArray(*this, counterpoint_1->getBranchingNotes().size() + counterpoint_2->getBranchingNotes().size() + 
        counterpoint_3->getBranchingNotes().size(), 0, 127);

    int c_sz = 0;
    if((counterpoint_1->getSpecies() >= counterpoint_2->getSpecies()) && (counterpoint_1->getSpecies() >= counterpoint_3->getSpecies())){
        c_sz = counterpoint_1->getCosts().size();
    } else if((counterpoint_2->getSpecies() >= counterpoint_1->getSpecies()) && (counterpoint_2->getSpecies() >= counterpoint_3->getSpecies())){
        c_sz = counterpoint_2->getCosts().size();
    } else {
        c_sz = counterpoint_3->getCosts().size();
    }
    c_sz+=2;
    
    unitedCosts = IntVarArray(*this, c_sz, 0, 10000);
    
    lowest->setCantusPointer(cantusFirmus);
    lowest->setCpPointer(*this, counterpoint_1, counterpoint_2, counterpoint_3);
    lowest->setLowest(*this, upper1, upper2, upper3);
    
    //TEST

    uniteCounterpoints();
    
    uniteCosts();
    
    branch(*this, solutionArray, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    
}

// COPY CONSTRUCTOR
FourVoiceCounterpoint::FourVoiceCounterpoint(FourVoiceCounterpoint& s) : CounterpointProblem(s){
    species = s.species;
    unitedCosts.update(*this, s.unitedCosts);
    successiveCostArray.update(*this, s.successiveCostArray);
    if (s.upper1) {
        upper1 = s.upper1->clone(*this);
    } else {
        upper1 = nullptr;
    }
    if (s.upper2) {
        upper2 = s.upper2->clone(*this);
    } else {
        upper2 = nullptr;
    }
    if (s.upper3) {
        upper3 = s.upper3->clone(*this);
    } else {
        upper3 = nullptr;
    }
}

Space* FourVoiceCounterpoint::copy(){  
    return new FourVoiceCounterpoint(*this);
}

string FourVoiceCounterpoint::to_string() const {
    string text = "CantusFirmus : \n";
    text += cantusFirmus->to_string();
    text += "Counterpoint 1 : \n";
    text += counterpoint_1->to_string();  
    text += "Counterpoint 2 : \n";
    text += counterpoint_2->to_string(); 
    text += "Counterpoint 3 : \n";
    text += counterpoint_3->to_string(); 
    text += "Lowest : \n";
    text += lowest->to_string(); 
    text += "Succ costs : \n";
    text += intVarArray_to_string(successiveCostArray); 
    text += "\n";
    text += "All costs : \n";
    text += intVarArray_to_string(unitedCosts); 
    text += "\n";
    //text += "Upper 1 : \n";
    //text += upper1->to_string(); 
    //text += "Upper 2 : \n";
    //text += upper2->to_string();  
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
    //TODO : THIS WILL CREATE BUGS FROM 3RD SPECIES ONWARDS; TOO BAD
    
    for(int i = 0; i < unitedCosts.size()-2; i++){
        int sz = 0;
        if(i<counterpoint_1->getCosts().size()){
            sz++;
        }
        if(i<counterpoint_2->getCosts().size()){
            sz++;
        }
        if(i<counterpoint_3->getCosts().size()){
            sz++;
        }
        IntVarArgs x(sz);
        int idx=0;
        if(i<counterpoint_1->getCosts().size()){
            x[idx] = counterpoint_1->getCosts()[i];
            idx++;
        }
        if(i<counterpoint_2->getCosts().size()){
            x[idx] = counterpoint_2->getCosts()[i];
            idx++;
        }
        if(i<counterpoint_3->getCosts().size()){
            x[idx] = counterpoint_3->getCosts()[i];
            idx++;
        }
        rel(*this, unitedCosts[i], IRT_EQ, expr(*this, sum(x)));
    }
    
    //add successive cost
    IntVarArgs y(successiveCostArray.size());
    for(int i = 0; i < successiveCostArray.size(); i++){
        y[i] = successiveCostArray[i];
    }
    rel(*this, unitedCosts[unitedCosts.size()-2], IRT_EQ, expr(*this, sum(y)));
    
    //add triad cost
    IntVarArgs z(triadCostArray.size());
    for(int i = 0; i < triadCostArray.size(); i++){
        z[i] = triadCostArray[i];
    }
    
    rel(*this, unitedCosts[unitedCosts.size()-1], IRT_EQ, expr(*this, sum(z)));
    cout << "HEREEEE" << endl;
}