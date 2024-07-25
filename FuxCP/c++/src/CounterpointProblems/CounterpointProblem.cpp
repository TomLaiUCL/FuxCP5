#include "../../headers/CounterpointProblems/CounterpointProblem.hpp"
#include "../../headers/CounterpointUtils.hpp"

/**
 * Constructor of the class.
 * @param cf a vector<int> representing the cantus firmus.
 * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
 * @param lb the lowest note possible for the counterpoint in MIDI
 * @param ub the highest note possible for the counterpoint in MIDI
 */
CounterpointProblem::CounterpointProblem(vector<int> cf, int k, int lb, int ub, int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs,
    vector<int> imp, int nV){
    nMeasures = cf.size();
    key = k;
    lowerBound = (6 * v_type - 6) + cf[0];
    upperBound = (6 * v_type + 12) + cf[0];
    lowest = new Stratum(*this, nMeasures, 0, 127, v_type);
    cantusFirmus = new CantusFirmus(*this, nMeasures, cf, key, lowest, v_type, m_costs, g_costs, s_costs, nV);
    importance = imp;
    n_unique_costs = 0;
    importanceNames = {"borrow", "fifth", "octave", "succ", "variety", "triad", "motion", "melodic", "direct", "penult", "syncopation" ,"cambiata", "m2", "triad3"};
    //creating the map with the names of the costs and their importance
    
    prefs = {};
    setPreferenceMap(importanceNames);
    orderedFactors = IntVarArray(*this, 14, 0, 1000);
    for(int i = 0; i < 14; i++){
        vector<string> tmp = {};
        costLevels.push_back(tmp);
    }

    for(const auto& entry : prefs){ 
        int val = entry.second-1;
        costLevels[val].push_back(entry.first);
    }



}

// COPY CONSTRUCTOR
CounterpointProblem::CounterpointProblem(CounterpointProblem& s) : IntLexMinimizeSpace(s){
    if (s.cantusFirmus) {
        cantusFirmus = s.cantusFirmus->clone(*this);
    } else {
        cantusFirmus = nullptr;
    }
    if (s.lowest) {
        lowest = s.lowest->clone(*this);
    } else {
        lowest = nullptr;
    }
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
    if (s.counterpoint_3) {
        counterpoint_3 = s.counterpoint_3->clone(*this);
    } else {
        counterpoint_3 = nullptr;
    }
    nMeasures = s.nMeasures; 
    importance = s.importance;
    key = s.key;
    lowerBound = s.lowerBound;
    upperBound = s.upperBound;
    prefs = s.prefs;
    unitedCostNames = s.unitedCostNames;
    costLevels = s.costLevels;
    n_unique_costs = s.n_unique_costs;
    importanceNames = s.importanceNames;
    successiveCostArray.update(*this, s.successiveCostArray);
    triadCostArray.update(*this, s.triadCostArray);
    unitedCosts.update(*this, s.unitedCosts);
    solutionArray.update(*this, s.solutionArray);
    sortedCosts.update(*this, s.sortedCosts);
    orderedFactors.update(*this, s.orderedFactors);
    finalCosts.update(*this, s.finalCosts);
}

IntLexMinimizeSpace* CounterpointProblem::copy(){   // todo use 'bool share' in copy constructor?
    return new CounterpointProblem(*this);
}

void CounterpointProblem::constrain(const IntMinimizeSpace& _b){

    const CounterpointProblem &b = dynamic_cast<const CounterpointProblem &>(_b);
    
}

IntVarArgs CounterpointProblem::cost() const{

    return IntVarArgs(finalCosts);

}

string CounterpointProblem::to_string() const {
    string text = "Counterpoint problem : \n";
    text += cantusFirmus->to_string(); 
    text += "\n";
    text = "All costs : \n";
    text += intVarArray_to_string(unitedCosts); 
    text += "\n";
    text += "Preference map : \n";
    /*for(int i = 0; i < costLevels.size(); i++){
        for(int k = 0; k < costLevels[i].size(); k++){
            text += costLevels[i][k];
            text += " ";
        }
        text += "\n";
    }*/
    text += "Final Costs : \n";
    text += intVarArray_to_string(finalCosts);
    text += "\n";
    return text;
}

Home CounterpointProblem::getHome(){
    return *this;
}

void CounterpointProblem::setPreferenceMap(vector<string> importance_names){
    for(int i = 0; i < importance.size(); i++){
        prefs.insert({importance_names[i], importance[i]});
    }
}

void CounterpointProblem::orderCosts(){
    for(int i = 0; i < 14; i++){
        if(!costLevels[i].empty()){
            for(int k = 0; k < costLevels[k].size(); k++){
                for(int t = 0; t < unitedCostNames.size(); t++){
                    if(unitedCostNames[t]==costLevels[i][k]){
                        cout << unitedCostNames[t] << endl;
                        rel(*this, orderedFactors[n_unique_costs], IRT_EQ, unitedCosts[t]);
                        n_unique_costs++;
                    }
                }
            }
        }
    }
    finalCosts = IntVarArray(*this, n_unique_costs, 0, 1000000);
    for(int i = 0; i < n_unique_costs; i++){
        rel(*this, finalCosts[i], IRT_EQ, orderedFactors[i]);
    }
    cout << finalCosts << endl;
}
