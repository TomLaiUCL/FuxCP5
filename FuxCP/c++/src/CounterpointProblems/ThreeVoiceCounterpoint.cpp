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
ThreeVoiceCounterpoint::ThreeVoiceCounterpoint(vector<int> cf, vector<int> sp, int k, int lb, int ub, vector<int> v_type, vector<int> m_costs, vector<int> g_costs) :
    CounterpointProblem(cf, k, lb, ub, -1, m_costs, g_costs, THREE_VOICES){
    species = sp;

    //G9 last chord must have the same fundamental as the cf (used throughout the composition)
    rel(*this, expr(*this, lowest->getNotes()[lowest->getNotes().size()-4]%12), IRT_EQ, expr(*this, cantusFirmus->getNotes()[cantusFirmus->getNotes().size()-1]%12));

    upper1 = new Stratum(*this, nMeasures, 0, 127, -1, lowest->getNotes(), THREE_VOICES);
    upper2 = new Stratum(*this, nMeasures, 0, 127, -1, lowest->getNotes(), THREE_VOICES);

    counterpoint_1 = create_counterpoint(*this, species[0], nMeasures, cf, (6 * v_type[0] - 6) + cf[0], (6 * v_type[0] + 12) + cf[0], key, lowest, 
        cantusFirmus, v_type[0], m_costs, g_costs, THREE_VOICES);
    counterpoint_2 = create_counterpoint(*this, species[1], nMeasures, cf, (6 * v_type[1] - 6) + cf[0], (6 * v_type[1] + 12) + cf[0], key, lowest, 
        cantusFirmus, v_type[1], m_costs, g_costs, THREE_VOICES);

    //P4 avoid successive perfect consonances
    int scc_cz = 0;
    if(sp.size()==2){
        scc_cz += ((cantusFirmus->getSize()/4)-1);
    } else if(sp.size()==3){
        scc_cz += 2*((cantusFirmus->getSize()/4)-1);
    }
    successiveCostArray = IntVarArray(*this, scc_cz, IntSet({0, counterpoint_1->getSuccCost()}));
    vector<Part*> parts = {cantusFirmus, counterpoint_1, counterpoint_2};
    int idx = 0;
    // cout << "SUCC SIZE :" + to_string(succ_cost.size()) << endl;
    for(int p1 = 1; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            // cout << "Part 1 : " << to_string(parts[p1].species) << endl;
            // cout << "Part 2 : " << to_string(parts[p2].species) << endl;
            if(parts[p1]->getSpecies()!=SECOND_SPECIES && parts[p2]->getSpecies()!=SECOND_SPECIES){
                for(int i = 0; i < parts[p1]->getFirstNotes().size()-1; i++){
                    // cout << "IDX : " + to_string(idx) << endl;
                    rel(*this, successiveCostArray[idx], IRT_EQ, counterpoint_1->getSuccCost(), 
                        Reify(expr(*this, (parts[p1]->getFirstHInterval()[i]==0 || parts[p1]->getFirstHInterval()[i]==7) &&
                            (parts[p2]->getFirstHInterval()[i]==0 || parts[p2]->getFirstHInterval()[i]==7))));
                    idx++;
                }        
            }/*
            else if(parts[p1].species==2){
                for(int i = 0; i < parts[p1].is_P_cons.size()-1; i++){
                    BoolVar case1 = expr(home, (parts[p1].is_P_cons[i]==1 && parts[p2].is_P_cons[i]==1) && 
                        (parts[p1].hIntervalsCpCf[0][i]!=PERFECT_FIFTH || parts[p2].hIntervalsCpCf[0][i]!=PERFECT_FIFTH));
                    BoolVar case2 = expr(home, (parts[p1].m_succ_intervals[0][i]!=MINOR_THIRD && parts[p1].m_succ_intervals[0][i]!=MAJOR_THIRD) && 
                        (parts[p1].hIntervalsCpCf[0][i]==PERFECT_FIFTH && parts[p2].hIntervalsCpCf[0][i]==PERFECT_FIFTH));
                    //first expression states that the melodic succ interval is not a third, second that we have successive fifths
                    rel(home, succ_cost[idx], IRT_EQ, parts[p2].succ, Reify(expr(home, (case1==1 || case2==1))));
                    idx++;
                }
            }
            else if(parts[p2].species==2){
                cout << "P cons size : " + to_string(parts[p1].is_P_cons.size()) << endl;
                cout << "M succ size : " + to_string(parts[p2].m_succ_intervals[0].size()) << endl;
                for(int i = 0; i < parts[p1].is_P_cons.size()-1; i++){
                    BoolVar case1 = expr(home, (parts[p1].is_P_cons[i]==1 && parts[p2].is_P_cons[i]==1) && 
                        (parts[p1].hIntervalsCpCf[0][i]!=PERFECT_FIFTH || parts[p2].hIntervalsCpCf[0][i]!=PERFECT_FIFTH));
                    cout << "i : " + to_string(i) << endl;
                    BoolVar case2 = expr(home, (parts[p2].m_succ_intervals[0][i]!=MINOR_THIRD && parts[p2].m_succ_intervals[0][i]!=MAJOR_THIRD) && 
                        (parts[p1].hIntervalsCpCf[0][i]==PERFECT_FIFTH && parts[p2].hIntervalsCpCf[0][i]==PERFECT_FIFTH));
                    //first expression states that the melodic succ interval is not a third, second that we have successive fifths
                    rel(home, succ_cost[idx], IRT_EQ, parts[p2].succ, Reify(expr(home, (case1==1 || case2==1))));
                    idx++;
                }
            }*/
        }
    }

    solutionArray = IntVarArray(*this, counterpoint_1->getBranchingNotes().size()*2, 0, 127);

    int c_sz = 0;
    if(counterpoint_1->getSpecies() >= counterpoint_2->getSpecies()){
        c_sz = counterpoint_1->getCosts().size();
    } else {
        c_sz = counterpoint_2->getCosts().size();
    }
    c_sz++;
    unitedCosts = IntVarArray(*this, c_sz, 0, 10000);

    lowest->setCantusPointer(cantusFirmus);
    lowest->setCpPointer(*this, counterpoint_1, counterpoint_2);
    lowest->setLowest(*this, upper1, upper2);

    uniteCounterpoints();
    uniteCosts();
    branch(*this, solutionArray, INT_VAR_SIZE_MIN(), INT_VAL_MIN());

}

// COPY CONSTRUCTOR
ThreeVoiceCounterpoint::ThreeVoiceCounterpoint(ThreeVoiceCounterpoint& s) : CounterpointProblem(s){
    species = s.species;
    solutionArray.update(*this, s.solutionArray);
    unitedCosts.update(*this, s.unitedCosts);
    successiveCostArray.update(*this, s.successiveCostArray);
    if (s.counterpoint_1) {
        counterpoint_1 = s.counterpoint_1->clone(*this);
    } else {
        counterpoint_1 = nullptr;
    }
    if (s.counterpoint_2) {
        counterpoint_2 = s.counterpoint_2->clone(*this);
    } else {
        counterpoint_2 = nullptr;
    }
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

Space* ThreeVoiceCounterpoint::copy(){  
    return new ThreeVoiceCounterpoint(*this);
}


string ThreeVoiceCounterpoint::to_string() const {
    string text = "CantusFirmus : \n";
    text += cantusFirmus->to_string();
    text += "Counterpoint 1 : \n";
    text += counterpoint_1->to_string();  
    text += "Counterpoint 2 : \n";
    text += counterpoint_2->to_string(); 
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
    //TODO : THIS WILL CREATE BUGS FROM 3RD SPECIES ONWARDS; TOO BAD
    for(int i = 0; i < unitedCosts.size()-1; i++){
        int sz = 0;
        if(i<counterpoint_1->getCosts().size()){
            sz++;
        }
        if(i<counterpoint_2->getCosts().size()){
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
        rel(*this, unitedCosts[i], IRT_EQ, expr(*this, sum(x)));
    }
    IntVarArgs y(successiveCostArray.size());
    for(int i = 0; i < successiveCostArray.size(); i++){
        y[i] = successiveCostArray[i];
    }
    rel(*this, unitedCosts[unitedCosts.size()-1], IRT_EQ, expr(*this, sum(y)));
}