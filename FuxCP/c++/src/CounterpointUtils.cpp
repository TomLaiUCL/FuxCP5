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
