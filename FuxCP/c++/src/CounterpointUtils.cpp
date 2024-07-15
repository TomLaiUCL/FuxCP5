#include "../headers/CounterpointUtils.hpp"


///////////////////////////////////////////////////////////////////
//////////////      GENERAL UTILS           ///////////////////////
///////////////////////////////////////////////////////////////////


Part* create_counterpoint(Home home, int species, int nMeasures, vector<int> cantusFirmus, int lowerBound, int upperBound, int key, Stratum* low,
    CantusFirmus* c, int v_type, vector<int> m_costs, vector<int> g_costs, int bm, int nV){
    switch(nV) {
        case TWO_VOICES :
            switch (species) { /// call the appropriate constructor for the counterpoint
            case FIRST_SPECIES:
                return new FirstSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, bm, TWO_VOICES);
                break;
            case SECOND_SPECIES:
                return new SecondSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, bm, TWO_VOICES);
                break;
            default:
                throw std::invalid_argument("Species not implemented");
            }
            break;
        case THREE_VOICES :
            switch (species) { /// call the appropriate constructor for the counterpoint
            case FIRST_SPECIES:
                return new FirstSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, bm,
                    TWO_VOICES, THREE_VOICES);
                break;
            case SECOND_SPECIES:
                return new SecondSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, bm, THREE_VOICES);
                break;
            default:
                throw std::invalid_argument("Species not implemented");
            }
            break;
        default :
            throw std::invalid_argument("Number of voices not implemented");
    }
};


CounterpointProblem* create_problem(vector<int> cf, vector<int> spList, int k, int lb, int ub, vector<int> v_type, vector<int> m_costs, vector<int> g_costs, int bm){
    switch (spList.size())
    {
    case 1:
        return new TwoVoiceCounterpoint(cf, spList[0], k, lb, ub, v_type[0], m_costs, g_costs, bm);
        break;
    
    case 2: 
        return new ThreeVoiceCounterpoint(cf, spList, k, lb, ub, v_type, m_costs, g_costs, bm); 
        break;

    default:
        throw std::invalid_argument("The number of voices you asked for is not implemented (yet).");
        break;
    }
}

void create_lowest(Home home, Stratum* lowest, CantusFirmus* cf, Part* cp1, Part* cp2){
    int nVoices = 2;
    if(cp2!=nullptr){
        nVoices = 3;
    }
    int size = cp1->getNMeasures();
    vector<IntVarArray> sorted_voices = {};
    for(int i = 0; i < size; i++){
        IntVarArray voices = IntVarArray(home, nVoices, 0, 127);

        rel(home, voices[0], IRT_EQ, cf->getNotes()[i]);
        rel(home, voices[1], IRT_EQ, cp1->getFirstNotes()[i]);
        if(nVoices==3){
            rel(home, voices[2], IRT_EQ, cp2->getFirstNotes()[i]);
        }
        IntVarArray order = IntVarArray(home, nVoices, 0, 1);
        sorted_voices.push_back(IntVarArray(home, nVoices, 0, 127));

        sorted(home, voices, sorted_voices[i], order);

        lowest->setNote(home, i, sorted_voices[i][0]);

        rel(home, lowest->getFirstNotes()[i], IRT_NQ, cf->getNotes()[i], Reify(cf->getIsLowestIdx(i)));

        if(nVoices==2){
            rel(home, cf->getIsLowestIdx(i), IRT_EQ, 0, Reify(cp1->getIsLowestIdx(i)));
        } else {
            rel(home, expr(home, (cf->getIsLowestIdx(i)==1)&&(lowest->getFirstNotes()[i]==cp1->getFirstNotes()[i])), IRT_NQ, 1, Reify(cp1->getIsLowestIdx(i)));
            rel(home, expr(home, cp1->getIsLowestIdx(i)!=cf->getIsLowestIdx(i)), IRT_EQ, cp2->getIsLowestIdx(i));
        }
    }
}

///////////////////////////////////////////////////////////////////
//////////////     CONTSTRAINTS             ///////////////////////
///////////////////////////////////////////////////////////////////