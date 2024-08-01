#include "../headers/CounterpointUtils.hpp"


///////////////////////////////////////////////////////////////////
//////////////      GENERAL UTILS           ///////////////////////
///////////////////////////////////////////////////////////////////


Part* create_counterpoint(Home home, int species, int nMeasures, vector<int> cantusFirmus, int lowerBound, int upperBound, int key, Stratum* low,
    CantusFirmus* c, int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs, int bm, int nV){
    switch(nV) {
        case TWO_VOICES :
            switch (species) { /// call the appropriate constructor for the counterpoint
            case FIRST_SPECIES:
                return new FirstSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm, TWO_VOICES);
                break;
            case SECOND_SPECIES:
                return new SecondSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm, TWO_VOICES);
                break;
            case THIRD_SPECIES:
                return new ThirdSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm, TWO_VOICES);
                break;
            case FOURTH_SPECIES:
                return new FourthSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm, TWO_VOICES);
                break;
            default:
                throw std::invalid_argument("Species not implemented");
            }
            break;
        case THREE_VOICES :
            switch (species) { /// call the appropriate constructor for the counterpoint
            case FIRST_SPECIES:
                return new FirstSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm,
                    TWO_VOICES, THREE_VOICES);
                break;
            case SECOND_SPECIES:
                return new SecondSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm, 
                    TWO_VOICES, THREE_VOICES);
                break;
            case THIRD_SPECIES:
                return new ThirdSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm,
                    TWO_VOICES, THREE_VOICES);
                break;
            default:
                throw std::invalid_argument("Species not implemented");
            }
            break;
        case FOUR_VOICES :
            switch (species) { /// call the appropriate constructor for the counterpoint
            case FIRST_SPECIES:
                return new FirstSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm,
                    TWO_VOICES, THREE_VOICES, FOUR_VOICES);
                break;
            case SECOND_SPECIES:
                return new SecondSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm, 
                    TWO_VOICES, THREE_VOICES, FOUR_VOICES);
                break;
            case THIRD_SPECIES:
                return new ThirdSpeciesCounterpoint(home, nMeasures, cantusFirmus, lowerBound, upperBound, key, low, c, v_type, m_costs, g_costs, s_costs, bm,
                    TWO_VOICES, THREE_VOICES, FOUR_VOICES);
                break;
            default:
                throw std::invalid_argument("Species not implemented");
            }
            break;
        default :
            throw std::invalid_argument("Number of voices not implemented");
    }
};


CounterpointProblem* create_problem(vector<int> cf, vector<Species> spList, int k, int lb, int ub, vector<int> v_type, vector<int> m_costs, vector<int> g_costs,
    vector<int> s_costs, vector<int> imp, int bm){

    // cout << "create_problem" << endl;
    // string message = "create_problem called with arguments : \n";
    // message += int_vector_to_string(cf) + "\n";
    // message += int_vector_to_string(spList) + "\n";
    // message += int_vector_to_string(v_type) + "\n";
    // writeToLogFile(message.c_str());

    switch (spList.size())
    {
    case 1:
        return new TwoVoiceCounterpoint(cf, spList[0], k, lb, ub, v_type[0], m_costs, g_costs, s_costs, imp, bm);
        break;
    case 2: 
        return new ThreeVoiceCounterpoint(cf, spList, k, lb, ub, v_type, m_costs, g_costs, s_costs, imp, bm); 
        break;
    case 3:
        return new FourVoiceCounterpoint(cf, spList, k, lb, ub, v_type, m_costs, g_costs, s_costs, imp, bm); 
        break;
    default:
        throw std::invalid_argument("The number of voices you asked for is not implemented (yet).");
        break;
    }
}

///////////////////////////////////////////////////////////////////
//////////////     CONTSTRAINTS             ///////////////////////
///////////////////////////////////////////////////////////////////