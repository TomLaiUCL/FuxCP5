#include "../headers/CounterpointUtils.hpp"


Part* create_counterpoint(Home home, int species, int nMeasures, vector<int> cantusFirmus, int lowerBound, int upperBound, int key){
    switch (species) { /// call the appropriate constructor for the counterpoint
    case FIRST_SPECIES:
        return new FirstSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key);
        break;
    case SECOND_SPECIES:
        return new SecondSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key);
        break;
    default:
        throw std::invalid_argument("Species not implemented");
    }
};


CounterpointProblem* create_problem(vector<int> cf, vector<int> spList, int k, int lb, int ub){
    switch (spList.size())
    {
    case 1:
        return new TwoVoiceCounterpoint(cf, spList[0], k, lb, ub);
        break;
    
    // case 2: 
        // return new ThreeVoiceCounterpoint(cf, spList, k, lb, ub); 
        // break;

    default:
        throw std::invalid_argument("The number of voices you asked for is not implemented (yet).");
        break;
    }
}
