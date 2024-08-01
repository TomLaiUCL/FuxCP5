#include "../../headers/CounterpointProblems/ThreeVoiceCounterpoint.hpp"
#include "../../headers/CounterpointUtils.hpp"

/**
 * Constructor of the class.
 * @param cf a vector<int> representing the cantus firmus.
 * @param sp the species of the counterpoint. it takes values from the enum "species" in headers/Utilities.hpp
 * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
 * @param lb the lowest note possible for the counterpoint in MIDI
 * @param ub the highest note possible for the counterpoint in MIDI
 */
ThreeVoiceCounterpoint::ThreeVoiceCounterpoint(vector<int> cf, vector<Species> sp, int k, int lb, int ub, vector<int> v_type, vector<int> m_costs, vector<int> g_costs,
    vector<int> s_costs, vector<int> imp, int bm) :
    CounterpointProblem(cf, k, lb, ub, -1, m_costs, g_costs, s_costs, imp, THREE_VOICES){
    species = sp;

    //G9 last chord must have the same fundamental as the cf (used throughout the composition)
    rel(*this, expr(*this, lowest->getNotes()[lowest->getNotes().size()-1]%12), IRT_EQ, expr(*this, cantusFirmus->getNotes()[cantusFirmus->getNotes().size()-1]%12));

    upper1 = new Stratum(*this, nMeasures, 0, 127, -1, lowest->getNotes(), THREE_VOICES);
    upper2 = new Stratum(*this, nMeasures, 0, 127, -1, lowest->getNotes(), THREE_VOICES);

    counterpoint_1 = create_counterpoint(*this, species[0], nMeasures, cf, (6 * v_type[0] - 6) + cf[0], (6 * v_type[0] + 12) + cf[0], key, lowest, 
        cantusFirmus, v_type[0], m_costs, g_costs, s_costs, bm, THREE_VOICES);
    counterpoint_2 = create_counterpoint(*this, species[1], nMeasures, cf, (6 * v_type[1] - 6) + cf[0], (6 * v_type[1] + 12) + cf[0], key, lowest, 
        cantusFirmus, v_type[1], m_costs, g_costs, s_costs, bm, THREE_VOICES);

    setLowest(counterpoint_2, nullptr, upper1, upper2, nullptr);

    vector<Part*> parts = {cantusFirmus, counterpoint_1, counterpoint_2};
    
    triadCostArray = IntVarArray(*this, counterpoint_1->getFirstHInterval().size(), IntSet({0, counterpoint_1->getTriadCost()}));

    //H8 : the triad should be used as much as possible
    for(int i = 0; i < triadCostArray.size(); i++){
        BoolVar h1_3 = BoolVar(*this, 0 ,1);
        BoolVar h1_4 = BoolVar(*this, 0 ,1);
        BoolVar h1_third = BoolVar(*this, 0 ,1);
        BoolVar h1_7 = BoolVar(*this, 0 ,1);

        BoolVar h2_3 = BoolVar(*this, 0 ,1);
        BoolVar h2_4 = BoolVar(*this, 0 ,1);
        BoolVar h2_third = BoolVar(*this, 0 ,1);
        BoolVar h2_7 = BoolVar(*this, 0 ,1);

        BoolVar h_firstPoss = BoolVar(*this, 0, 1);
        BoolVar h_secondPoss = BoolVar(*this, 0, 1);
        BoolVar triad = BoolVar(*this, 0, 1);
        BoolVar not_triad = BoolVar(*this, 0, 1);

        rel(*this, upper1->getHInterval()[i*4], IRT_EQ, 3, Reify(h1_3));
        rel(*this, upper1->getHInterval()[i*4], IRT_EQ, 4, Reify(h1_4));
        rel(*this, upper2->getHInterval()[i*4], IRT_EQ, 7, Reify(h2_7));
        rel(*this, h1_3, BOT_OR, h1_4, h1_third);
        rel(*this, h1_third, BOT_AND, h2_7, h_firstPoss);

        rel(*this, upper2->getHInterval()[i*4], IRT_EQ, 3, Reify(h2_3));
        rel(*this, upper2->getHInterval()[i*4], IRT_EQ, 4, Reify(h2_4));
        rel(*this, upper1->getHInterval()[i*4], IRT_EQ, 7, Reify(h1_7));
        rel(*this, h2_3, BOT_OR, h2_4, h2_third);
        rel(*this, h2_third, BOT_AND, h1_7, h_secondPoss);

        rel(*this, h_firstPoss, BOT_OR, h_secondPoss, triad);
        rel(*this, triad, BOT_XOR, not_triad, 1);
        rel(*this, triadCostArray[i], IRT_EQ, 0, Reify(triad, RM_IMP));
        rel(*this, triadCostArray[i], IRT_EQ, counterpoint_1->getTriadCost(), Reify(not_triad, RM_IMP));
    }

    

    //M4 variety cost (notes should be as diverse as possible)
    for(int i = 1; i < parts.size(); i++){
        Part* p = parts[i];
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
    int scc_cz = ((cantusFirmus->getSize()/4)-1);

    successiveCostArray = IntVarArray(*this, scc_cz, IntSet({0, counterpoint_1->getSuccCost()}));
    
    int idx = 0;
   
    // cout << "SUCC SIZE :" + to_string(succ_cost.size()) << endl;
    for(int p1 = 1; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            cout << "Mother species 3 voices : " << endl;
            // cout << "Part 1 : " << to_string(parts[p1].species) << endl;
            // cout << "Part 2 : " << to_string(parts[p2].species) << endl;
            cout << species[p1-1] << endl;
            if(species[p1-1]!=SECOND_SPECIES && species[p2-1]!=SECOND_SPECIES){
                
                for(int i = 0; i < parts[p1]->getFirstHInterval().size()-1; i++){
                    // cout << "IDX : " + to_string(idx) << endl;
                    rel(*this, successiveCostArray[idx], IRT_EQ, counterpoint_1->getSuccCost(), 
                        Reify(expr(*this, (parts[p1]->getFirstHInterval()[i]==0 || parts[p1]->getFirstHInterval()[i]==7) &&
                            (parts[p2]->getFirstHInterval()[i]==0 || parts[p2]->getFirstHInterval()[i]==7))));
                    idx++;
                }        
            }
            else if(species[p1-1]==SECOND_SPECIES){
                
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
            else if(species[p2-1]==SECOND_SPECIES){
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

    //TODO : RECHEKC THIS CONSTRAINT

    //2.M2, have to write it here since it has a weird interaction with the third species
    bool containsThirdSpecies = 0;
    for(int i = 1; i < parts.size(); i++){
        if(parts[i]->getSpecies()==THIRD_SPECIES){
            containsThirdSpecies=1;
            break;
        }
    }
    for(int p1 = 1; p1 < parts.size(); p1++){
        for(int p2 = 1; p2 < parts.size(); p2++){
            if(p1!=p2 && parts[p1]->getSpecies()==SECOND_SPECIES){
                if(!containsThirdSpecies){
                    for(int i = 0; i < parts[p1]->getBranchingNotes().size()-4; i++){
                        rel(*this, parts[p1]->getMelodicIntervals().slice(0, notesPerMeasure.at(SECOND_SPECIES), parts[p1]->getMelodicIntervals().size())[i], 
                            IRT_NQ, 0);
                    }
                } else {
                    for(int i = 0; i < parts[p1]->getBranchingNotes().size()-4; i++){
                        rel(*this, parts[p1]->getMelodicIntervals().slice(0, notesPerMeasure.at(SECOND_SPECIES), parts[p1]->getMelodicIntervals().size())[i], 
                            IRT_NQ, 0);
                    }
                    //no unison in the two last notes
                    rel(*this, parts[p1]->getBranchingNotes()[parts[p1]->getBranchingNotes().size()-2], IRT_NQ, parts[p1]->getBranchingNotes()[parts[p1]->getBranchingNotes().size()-1]);
                }
            }
        }
    }

    solutionArray = IntVarArray(*this, counterpoint_1->getBranchingNotes().size() + counterpoint_2->getBranchingNotes().size(), 0, 127);

    unitedCosts = IntVarArray(*this, 14, 0, 10000000);
    unitedCostNames = {};
    
    //TEST

    uniteCounterpoints();
    uniteCosts();

    orderCosts();

    branch(*this, solutionArray, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
    branch(*this, lowest->getNotes().slice(0, 4/notesPerMeasure.at(FIRST_SPECIES), lowest->getNotes().size()), INT_VAR_DEGREE_MAX(), INT_VAL_SPLIT_MIN());


    writeToLogFile(("solution array size : " + std::to_string(solutionArray.size())).c_str());

}

// COPY CONSTRUCTOR
ThreeVoiceCounterpoint::ThreeVoiceCounterpoint(ThreeVoiceCounterpoint& s) : CounterpointProblem(s){
    species = s.species;
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
}

IntLexMinimizeSpace* ThreeVoiceCounterpoint::copy(){  
    return new ThreeVoiceCounterpoint(*this);
}


string ThreeVoiceCounterpoint::to_string() const {
    string text = CounterpointProblem::to_string();
    text += "Counterpoint 1 : \n";
    text += counterpoint_1->to_string();
    text += "\n";
    text += "Counterpoint 2 : \n";
    text += counterpoint_2->to_string();
    text += "\n";
    text += "Upper 1 : \n";
    text += upper1->to_string();
    text += "\n";
    text += "Upper 2 : \n";
    text += upper2->to_string();
    text += "\n";
    text += " Solution array : \n";
    text += intVarArray_to_string(solutionArray);
    text += "\n";
    return text;
}

void ThreeVoiceCounterpoint::uniteCounterpoints(){
    int idx = 0;
    for(int i = 0; i < counterpoint_1->getBranchingNotes().size(); i++){
        rel(*this, solutionArray[idx], IRT_EQ, counterpoint_1->getBranchingNotes()[i]);
        idx++;
    }
    for(int i = 0; i < counterpoint_2->getBranchingNotes().size(); i++){
        rel(*this, solutionArray[idx], IRT_EQ, counterpoint_2->getBranchingNotes()[i]);
        idx++;
    }
}

void ThreeVoiceCounterpoint::uniteCosts(){
    int cp1_idx = 0;
    int cp2_idx = 0;
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
            if(!cp1_contains && !cp2_contains){
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
                rel(*this, unitedCosts[i], IRT_EQ, expr(*this, sum(x)));
            }
        }
    }
}