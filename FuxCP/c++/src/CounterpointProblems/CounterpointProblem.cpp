#include "../../headers/CounterpointProblems/CounterpointProblem.hpp"
#include "../../headers/CounterpointUtils.hpp"

/**
 * Constructor of the class.
 * @param cf a vector<int> representing the cantus firmus.
 * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
 * @param lb the lowest note possible for the counterpoint in MIDI
 * @param ub the highest note possible for the counterpoint in MIDI
 */
CounterpointProblem::CounterpointProblem(vector<int> cf, int k, int lb, int ub, int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int nV){
    nMeasures = cf.size();
    key = k;
    lowerBound = (6 * v_type - 6) + cf[0];
    upperBound = (6 * v_type + 12) + cf[0];
    lowest = new Stratum(*this, nMeasures, 0, 127, v_type);
    cantusFirmus = new CantusFirmus(*this, nMeasures, cf, key, lowest, v_type, m_costs, g_costs, s_costs, nV);

}

// COPY CONSTRUCTOR
CounterpointProblem::CounterpointProblem(CounterpointProblem& s) : Space(s){
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
    nMeasures = s.nMeasures; 
    key = s.key;
    lowerBound = s.lowerBound;
    upperBound = s.upperBound;
    successiveCostArray.update(*this, s.successiveCostArray);
}

Space* CounterpointProblem::copy(){   // todo use 'bool share' in copy constructor?
    return new CounterpointProblem(*this);
}


string CounterpointProblem::to_string() const {
    string text = "Counterpoint problem : \n";
    text += cantusFirmus->to_string(); 
    text += "\n";
    return text;
}