// 
// Created by Luc Cleenewerk and Diego de Patoul. 
// 

#include "../../headers/CounterpointProblems/CounterpointProblem.hpp"
#include "../../headers/CounterpointUtils.hpp"

/**
 * Constructor of the class.
 * @param cf a vector<int> representing the cantus firmus.
 * @param k the key of the score. it takes values from the notes in headers/Utilities.hpp
 * @param lb the lowest note possible for the counterpoint in MIDI
 * @param ub the highest note possible for the counterpoint in MIDI
 */
CounterpointProblem::CounterpointProblem(vector<int> cf, int v_type, vector<int> m_costs, vector<int> g_costs, vector<int> s_costs,
    vector<int> imp, int nV){
    nMeasures = cf.size();
    lowest = new Stratum(*this, nMeasures, 0, 127);
    cantusFirmus = new CantusFirmus(*this, nMeasures, cf, lowest, v_type, m_costs, g_costs, s_costs, nV);
    importance = imp;
    n_unique_costs = 0;
    importanceNames = {"borrow", "fifth", "octave", "succ", "variety", "triad", "direct", "motion", "penult", "cambiata", "triad3" ,"m2", "syncopation", "melodic"};
    
    //creating the map with the names of the costs and their importance
    
    prefs = {};

    setPreferenceMap(importanceNames);

    orderedFactors = IntVarArray(*this, 14, 0, 1000);   //orderedFactors will contain the costs in order of their importance
    for(int i = 0; i < 14; i++){
        vector<string> tmp = {};
        costLevels.push_back(tmp);                      //costLevels will contain all the costs at a specific level of importance (1 to 14)
    }

    for(const auto& entry : prefs){ 
        int val = entry.second-1;
        costLevels[val].push_back(entry.first);         //initialize the costLevels
    }

    globalCost = IntVar(*this, 0, 2000000);             //contains the global cost

    writeToLogFile("counterpointproblem constructor"); 

    // Constraints Cost
    combinedCosts = IntVarArray(*this, combinedCostNames.size(), 0, 10000);         //combinedCosts will contain the costs of the different counterpoints

    // 1H1
    // combined constraints has to be respect user preferences
    int respect_percent_1H1 = 50; //50 allowed every Fux figures tested (figures 60 is the worst)
    //TODO: should be replaced by the user preference  

    int nCP = 1; // number of counterpoints
    if (counterpoint_2 != nullptr) {
        nCP ++;
    }
    if (counterpoint_3 != nullptr) {
        nCP ++;
    }
    int n_thesis_notes = this->nMeasures * nCP;
    int threshold = n_thesis_notes - (respect_percent_1H1 * n_thesis_notes) / 100;
    rel(*this, combinedCosts[H1_1], IRT_LQ, threshold); // 1H1
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
    if(s.upper_1){
        upper_1 = s.upper_1->clone(*this);
    } else {
        upper_1 = nullptr;
    }
    if(s.upper_2){
        upper_2 = s.upper_2->clone(*this);
    } else {
        upper_2 = nullptr;
    }
    if(s.upper_3){
        upper_3 = s.upper_3->clone(*this);
    } else {
        upper_3 = nullptr;
    }
    nMeasures = s.nMeasures; 
    importance = s.importance;
    prefs = s.prefs;
    sorted_voices = s.sorted_voices;
    combinedCosts = s.combinedCosts;
    unitedCostNames = s.unitedCostNames;
    costLevels = s.costLevels;
    n_unique_costs = s.n_unique_costs;
    importanceNames = s.importanceNames;
    combinedCosts.update(*this, s.combinedCosts);
    successiveCostArray.update(*this, s.successiveCostArray);
    triadCostArray.update(*this, s.triadCostArray);
    unitedCosts.update(*this, s.unitedCosts);
    solutionArray.update(*this, s.solutionArray);
    sortedCosts.update(*this, s.sortedCosts);
    orderedFactors.update(*this, s.orderedFactors);
    finalCosts.update(*this, s.finalCosts);
    for(int i = 0; i < sorted_voices.size(); i++){
        sorted_voices[i].update(*this, s.sorted_voices[i]);
    }
    globalCost.update(*this, s.globalCost);

    hasRelaxation = s.hasRelaxation;
    problemRelaxationCosts.update(*this, s.problemRelaxationCosts);
    if(hasRelaxation){
        totalRelaxationCost.update(*this, s.totalRelaxationCost);
    }
}

IntLexMinimizeSpace* CounterpointProblem::copy(){   
    return new CounterpointProblem(*this);
}

void CounterpointProblem::constrain(const IntLexMinimizeSpace& _b){

    //const CounterpointProblem &b = dynamic_cast<const CounterpointProblem &>(_b);
    IntLexMinimizeSpace::constrain(_b); // Faster, default one (the previous one was empty ?!?)
    
}

IntVarArgs CounterpointProblem::cost() const{

    return IntVarArgs(finalCosts);

}

string CounterpointProblem::to_string() const {
    string text = "Counterpoint problem : \n";
    text += cantusFirmus->to_string(); 
    text += "\n";
    text += "All costs : \n";
    text += intVarArray_to_string(unitedCosts); 
    text += "\nLowest : \n";
    text += lowest->to_string();
    text += "\n";
    text += "Upper : \n";
    text += upper_1->to_string();
    text += "\n";
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

double CounterpointProblem::getCost() const {
    double total = 0;
    IntVarArgs costs = cost();
    for (int i = 0; i < costs.size(); ++i) {
        total += costs[i].val();
    }
    return total;
}

void CounterpointProblem::orderCosts(){
    for(int i = 0; i < 14; i++){
        if(!costLevels[i].empty()){
            int sm_size = 0;
            for(int k = 0; k < costLevels[i].size(); k++){
                //goes through every cost at the cost level
                for(int t = 0; t < unitedCostNames.size(); t++){
                    if(unitedCostNames[t]==costLevels[i][k]){
                        //adjusts the amount of costs at this level
                        sm_size++;
                    }
                }
            }
            //now cst_sm_size contains the size of all the cost array on this level combined
            IntVarArgs sm(sm_size); //sum of all the costs on this level
            int idx = 0;
            for(int k = 0; k < costLevels[i].size(); k++){
                //goes through every cost at the cost level
                for(int t = 0; t < unitedCostNames.size(); t++){
                    if(unitedCostNames[t]==costLevels[i][k]){
                        //add the cost to the intvarargs
                        sm[idx] = unitedCosts[t];
                        idx++;
                    }
                }
            }
            //if there is at least one cost at this level
            if(idx>0){
                //then sum all the intvarargs of this level together and add them to the orderedFactors
                rel(*this, orderedFactors[n_unique_costs], IRT_EQ, expr(*this, sum(sm)));
                //increase the amount of unique costs
                n_unique_costs++;
            }
        }
    }
    
    finalCosts = IntVarArray(*this, n_unique_costs, 0, 1000000);
    for(int i = 0; i < n_unique_costs; i++){
        //adds the cost/costs to the finalCosts list, which will be the one that will be minimized
        //(this eliminates all the <not assigned> values of the intvararray for costs which are not set)
        rel(*this, finalCosts[i], IRT_EQ, orderedFactors[i]);
    }

    // If relaxation costs exist, prepend totalRelaxationCost as highest priority
    if(hasRelaxation){
        IntVarArray newFinalCosts(*this, n_unique_costs + 1, 0, 1000000);
        // First element: relaxation cost (highest lex priority)
        rel(*this, newFinalCosts[0], IRT_EQ, totalRelaxationCost);
        // Rest: original costs
        for(int i = 0; i < n_unique_costs; i++){
            rel(*this, newFinalCosts[i+1], IRT_EQ, finalCosts[i]);
        }
        finalCosts = newFinalCosts;
    }

    //globalCost is the sum of all the finalCosts
    rel(*this, globalCost, IRT_EQ, expr(*this, sum(finalCosts)));
}


void CounterpointProblem::setStrata(){
    // Determine number of voices
    int nVoices = 2;
    if(upper_2 != nullptr) nVoices++;
    if(upper_3 != nullptr) nVoices++;
    
    int size = counterpoint_1->getNMeasures();

    // Initialize measure_orders for each measure
    sorted_voices = {};
    measures_order = {};

    // Process each measure
    for(int i = 0; i < size; i++){
        // For each measure, create voices array with the actual notes at that measure
        IntVarArray voices = IntVarArray(*this, nVoices, 0, 127);
        
        // Set cantus firmus note
        rel(*this, voices[0], IRT_EQ, cantusFirmus->getNotes()[i]);

        // Set counterpoint notes based on species-specific rules for this measure
        setVoiceNote(voices, 1, counterpoint_1, i);
        if(nVoices >= 3) setVoiceNote(voices, 2, counterpoint_2, i);
        if(nVoices >= 4) setVoiceNote(voices, 3, counterpoint_3, i);

        // Sort voices and create order array for this specific measure
        measures_order.push_back(IntVarArray(*this, nVoices, 0, nVoices-1));
        sorted_voices.push_back(IntVarArray(*this, nVoices, 0, 127));
        sorted(*this, voices, sorted_voices[i], measures_order[i]);

        // ---- Tie-breaker: if two voices have same pitch, enforce deterministic order
        for (int a = 0; a < nVoices; ++a) {
            for (int b = a + 1; b < nVoices; ++b) {
                BoolVar eq(*this, 0, 1);
                rel(*this, voices[a], IRT_EQ, voices[b], Reify(eq, RM_EQV));
                // If equal pitch, force order[a] < order[b] (priority: smaller index wins)
                rel(*this, eq >> (measures_order[i][a] < measures_order[i][b]));
            }
        }

        // Set stratum constraints for each position in the measure
        int maxPos = (i == size-1) ? 1 : 4;  // Only set first position for last measure
        for(int pos = 0; pos < maxPos; pos++){
            // Set stratum constraints using the position-specific ordering
            setStrataAtPosition(i, pos, nVoices, measures_order[i]);
        }
        
        // Set voice flags and melodic interval constraints (only once per measure)
        setVoiceLowestFlags(i, nVoices, size);
        if(i > 0) setMelodicIntervalConstraints(i, nVoices);
    }
}

// Helper function implementations for setStrata

void CounterpointProblem::setVoiceNote(IntVarArray& voices, int voiceIndex, Part* part, int measureIndex) {
    // Set the appropriate note for a voice at a specific position within the measure
    if ((part->getSpecies() == FOURTH_SPECIES || part->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
        // First measure: use resolution note (index 2) for fourth/fifth species
        rel(*this, voices[voiceIndex], IRT_EQ, part->getNotes()[(measureIndex*4)+2]);
    } else if (part->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
        // Middle measures of fourth species: choose between thesis and arsis based on joined degree
        BoolVar isJoinedDegree(*this, 0, 1);
        rel(*this, expr(*this, abs(part->getNotes()[measureIndex*4+2] - part->getNotes()[measureIndex*4])), IRT_LQ, 2, Reify(isJoinedDegree));
        rel(*this, (isJoinedDegree == 0) >> (voices[voiceIndex] == part->getNotes()[measureIndex*4]));      // thesis
        rel(*this, (isJoinedDegree == 1) >> (voices[voiceIndex] == part->getNotes()[(measureIndex*4)+2])); // arsis
    } else if (part->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
        // Fifth species: use species at this specific position
        IntVar currentSpecies(*this, -1, 4);
        rel(*this, currentSpecies, IRT_EQ, part->getSpeciesArray()[measureIndex*4]);
        // Fourth species logic for fifth species
        BoolVar isJoinedDegree(*this, 0, 1);
        rel(*this, expr(*this, abs(part->getNotes()[measureIndex*4+2] - part->getNotes()[measureIndex*4])), IRT_LQ, 2, Reify(isJoinedDegree));
        rel(*this, (currentSpecies == FOURTH_SPECIES && isJoinedDegree == 0) >> (voices[voiceIndex] == part->getNotes()[measureIndex*4]));
        rel(*this, (currentSpecies == FOURTH_SPECIES && isJoinedDegree == 1) >> (voices[voiceIndex] == part->getNotes()[measureIndex*4+2]));
        // Third species logic for fifth species
        rel(*this, (currentSpecies != FOURTH_SPECIES) >> (voices[voiceIndex] == part->getNotes()[measureIndex*4]));
    } else {
        // Default case: use first note of the measure
        rel(*this, voices[voiceIndex], IRT_EQ, part->getFirstNotes()[measureIndex]);
    }
}

void CounterpointProblem::setStrataAtPosition(int measureIndex, int position, int nVoices, IntVarArray& order) {
    // The order array tells us: order[voice_index] = position_in_sorted_order
    // We need to find which voice is at each sorted position (0=lowest, 1=middle, etc.)
    
    // For the first beat of each measure, we already computed the note that really matter for each stratum
    if (position == 0) {
        rel(*this, lowest->getNotes()[measureIndex*4+position], IRT_EQ, sorted_voices[measureIndex][0]);
        rel(*this, upper_1->getNotes()[measureIndex*4+position], IRT_EQ, sorted_voices[measureIndex][1]);
        if(nVoices >= 3) rel(*this, upper_2->getNotes()[measureIndex*4+position], IRT_EQ, sorted_voices[measureIndex][2]);
        if(nVoices >= 4) rel(*this, upper_3->getNotes()[measureIndex*4+position], IRT_EQ, sorted_voices[measureIndex][3]);

    } else {
        // Lowest stratum
        BoolVar cfIsLowest(*this, 0, 1);
        BoolVar cp1IsLowest(*this, 0, 1);
        BoolVar cp2IsLowest(*this, 0, 1);
        BoolVar cp3IsLowest(*this, 0, 1);
        
        rel(*this, order[0], IRT_EQ, 0, Reify(cfIsLowest));      // cantus firmus is at position 0 (lowest)
        rel(*this, order[1], IRT_EQ, 0, Reify(cp1IsLowest));     // counterpoint_1 is at position 0 (lowest)
        if(nVoices >= 3) rel(*this, order[2], IRT_EQ, 0, Reify(cp2IsLowest)); // counterpoint_2 is at position 0 (lowest)
        if(nVoices >= 4) rel(*this, order[3], IRT_EQ, 0, Reify(cp3IsLowest)); // counterpoint_3 is at position 0 (lowest)

        // For cantus firmus
        rel(*this, cfIsLowest >> (lowest->getNotes()[measureIndex*4+position] == cantusFirmus->getNotes()[measureIndex]));
        
        // For counterpoint_1
        if (counterpoint_1->getSpecies() == FIRST_SPECIES) {
            rel(*this, cp1IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_1->getFirstNotes()[measureIndex]));
        } else if (counterpoint_1->getSpecies() == SECOND_SPECIES) {
            int noteIndex = (position < 2) ? 0 : 2;  // pos 0,1 -> 0; pos 2,3 -> 2
            rel(*this, cp1IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+noteIndex]));
        } else if (counterpoint_1->getSpecies() == THIRD_SPECIES) {
            rel(*this, cp1IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
        } else if ((counterpoint_1->getSpecies() == FOURTH_SPECIES || counterpoint_1->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
            // First measure: use arsis note (index 2) for fourth/fifth species
            rel(*this, cp1IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+2]));
        } else if (counterpoint_1->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
            if (position == 2) {
                rel(*this, cp1IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
            } else {
                rel(*this, cp1IsLowest >> (lowest->getNotes()[measureIndex*4+position] == lowest->getNotes()[measureIndex*4+position-1]));
            }
        } else if (counterpoint_1->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
            if (position == 2) {
                rel(*this, cp1IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
            } else {
                 IntVar currentSpecies(*this, -1, 4);
                rel(*this, currentSpecies, IRT_EQ, counterpoint_1->getSpeciesArray()[measureIndex*4+position]);
                rel(*this, (cp1IsLowest && currentSpecies == THIRD_SPECIES) >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
                rel(*this, (cp1IsLowest && currentSpecies != THIRD_SPECIES) >> (lowest->getNotes()[measureIndex*4+position] == lowest->getNotes()[measureIndex*4+position-1]));
            }
        } else {
            // Default case: use first note of the measure
            rel(*this, cp1IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4]));
        }
        
        if(nVoices >= 3) {
            // For counterpoint_2
            if (counterpoint_2->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp2IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_2->getFirstNotes()[measureIndex]));
            } else if (counterpoint_2->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp2IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_2->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp2IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_2->getSpecies() == FOURTH_SPECIES || counterpoint_2->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp2IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_2->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp2IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp2IsLowest >> (lowest->getNotes()[measureIndex*4+position] == lowest->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_2->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp2IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_2->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp2IsLowest && currentSpecies == THIRD_SPECIES) >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp2IsLowest && currentSpecies != THIRD_SPECIES) >> (lowest->getNotes()[measureIndex*4+position] == lowest->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp2IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4]));
            }
        }
        
        if(nVoices >= 4) {
            // For counterpoint_3
            if (counterpoint_3->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp3IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_3->getFirstNotes()[measureIndex]));
            } else if (counterpoint_3->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp3IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_3->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp3IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_3->getSpecies() == FOURTH_SPECIES || counterpoint_3->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp3IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_3->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp3IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp3IsLowest >> (lowest->getNotes()[measureIndex*4+position] == lowest->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_3->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp3IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_3->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp3IsLowest && currentSpecies == THIRD_SPECIES) >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp3IsLowest && currentSpecies != THIRD_SPECIES) >> (lowest->getNotes()[measureIndex*4+position] == lowest->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp3IsLowest >> (lowest->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4]));
            }
        }

        // Set upper strata notes based on voice mapping
        // For upper_1 (sorted position 1)
        BoolVar cfIsSecond(*this, 0, 1);
        BoolVar cp1IsSecond(*this, 0, 1);
        BoolVar cp2IsSecond(*this, 0, 1);
        BoolVar cp3IsSecond(*this, 0, 1);
        
        rel(*this, order[0], IRT_EQ, 1, Reify(cfIsSecond));      // cantus firmus is at position 1 (second)
        rel(*this, order[1], IRT_EQ, 1, Reify(cp1IsSecond));     // counterpoint_1 is at position 1 (second)
        if(nVoices >= 3) rel(*this, order[2], IRT_EQ, 1, Reify(cp2IsSecond)); // counterpoint_2 is at position 1 (second)
        if(nVoices >= 4) rel(*this, order[3], IRT_EQ, 1, Reify(cp3IsSecond)); // counterpoint_3 is at position 1 (second)

        // CF (whole note)
        rel(*this, cfIsSecond >> (upper_1->getNotes()[measureIndex*4+position] == cantusFirmus->getNotes()[measureIndex]));

        // cp1: mirror lowest logic
        if (counterpoint_1->getSpecies() == FIRST_SPECIES) {
            rel(*this, cp1IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_1->getFirstNotes()[measureIndex]));
        } else if (counterpoint_1->getSpecies() == SECOND_SPECIES) {
            int noteIndex = (position < 2) ? 0 : 2;
            rel(*this, cp1IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+noteIndex]));
        } else if (counterpoint_1->getSpecies() == THIRD_SPECIES) {
            rel(*this, cp1IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
        } else if ((counterpoint_1->getSpecies() == FOURTH_SPECIES || counterpoint_1->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
            rel(*this, cp1IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+2]));
        } else if (counterpoint_1->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
            if (position == 2) {
                rel(*this, cp1IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
            } else {
                rel(*this, cp1IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == upper_1->getNotes()[measureIndex*4+position-1]));
            }
        } else if (counterpoint_1->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
            if (position == 2) {
                rel(*this, cp1IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
            } else {
                IntVar currentSpecies(*this, -1, 4);
                rel(*this, currentSpecies, IRT_EQ, counterpoint_1->getSpeciesArray()[measureIndex*4+position]);
                rel(*this, (cp1IsSecond && currentSpecies == THIRD_SPECIES) >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
                rel(*this, (cp1IsSecond && currentSpecies != THIRD_SPECIES) >> (upper_1->getNotes()[measureIndex*4+position] == upper_1->getNotes()[measureIndex*4+position-1]));
            }
        } else {
            rel(*this, cp1IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4]));
        }

        if(nVoices >= 3) {
            // cp2: mirror lowest logic
            if (counterpoint_2->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp2IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_2->getFirstNotes()[measureIndex]));
            } else if (counterpoint_2->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp2IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_2->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp2IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_2->getSpecies() == FOURTH_SPECIES || counterpoint_2->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp2IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_2->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp2IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp2IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == upper_1->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_2->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp2IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_2->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp2IsSecond && currentSpecies == THIRD_SPECIES) >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp2IsSecond && currentSpecies != THIRD_SPECIES) >> (upper_1->getNotes()[measureIndex*4+position] == upper_1->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp2IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4]));
            }
        }
        
        if(nVoices >= 4) {
            // cp3: mirror lowest logic
            if (counterpoint_3->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp3IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_3->getFirstNotes()[measureIndex]));
            } else if (counterpoint_3->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp3IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_3->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp3IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_3->getSpecies() == FOURTH_SPECIES || counterpoint_3->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp3IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_3->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp3IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp3IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == upper_1->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_3->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp3IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_3->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp3IsSecond && currentSpecies == THIRD_SPECIES) >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp3IsSecond && currentSpecies != THIRD_SPECIES) >> (upper_1->getNotes()[measureIndex*4+position] == upper_1->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp3IsSecond >> (upper_1->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4]));
            }
        }

        // For upper_2 (sorted position 2)
        if(nVoices >= 3) {
            BoolVar cfIsThird(*this, 0, 1);
            BoolVar cp1IsThird(*this, 0, 1);
            BoolVar cp2IsThird(*this, 0, 1);
            BoolVar cp3IsThird(*this, 0, 1);
            rel(*this, order[0], IRT_EQ, 2, Reify(cfIsThird));
            rel(*this, order[1], IRT_EQ, 2, Reify(cp1IsThird));
            rel(*this, order[2], IRT_EQ, 2, Reify(cp2IsThird));
            if(nVoices >= 4) rel(*this, order[3], IRT_EQ, 2, Reify(cp3IsThird));
            
            rel(*this, cfIsThird >> (upper_2->getNotes()[measureIndex*4+position] == cantusFirmus->getNotes()[measureIndex]));
            
            // cp1: mirror lowest logic
            if (counterpoint_1->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp1IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_1->getFirstNotes()[measureIndex]));
            } else if (counterpoint_1->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp1IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_1->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp1IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_1->getSpecies() == FOURTH_SPECIES || counterpoint_1->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp1IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_1->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp1IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp1IsThird >> (upper_2->getNotes()[measureIndex*4+position] == upper_2->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_1->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp1IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_1->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp1IsThird && currentSpecies == THIRD_SPECIES) >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp1IsThird && currentSpecies != THIRD_SPECIES) >> (upper_2->getNotes()[measureIndex*4+position] == upper_2->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp1IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4]));
            }
            
            // cp2: mirror lowest logic
            if (counterpoint_2->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp2IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_2->getFirstNotes()[measureIndex]));
            } else if (counterpoint_2->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp2IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_2->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp2IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_2->getSpecies() == FOURTH_SPECIES || counterpoint_2->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp2IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_2->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp2IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp2IsThird >> (upper_2->getNotes()[measureIndex*4+position] == upper_2->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_2->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp2IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_2->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp2IsThird && currentSpecies == THIRD_SPECIES) >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp2IsThird && currentSpecies != THIRD_SPECIES) >> (upper_2->getNotes()[measureIndex*4+position] == upper_2->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp2IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4]));
            }

            if(nVoices >= 4) {
                // cp3 may also be third in 4 voices: mirror lowest logic
                if (counterpoint_3->getSpecies() == FIRST_SPECIES) {
                    rel(*this, cp3IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_3->getFirstNotes()[measureIndex]));
                } else if (counterpoint_3->getSpecies() == SECOND_SPECIES) {
                    int noteIndex = (position < 2) ? 0 : 2;
                    rel(*this, cp3IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+noteIndex]));
                } else if (counterpoint_3->getSpecies() == THIRD_SPECIES) {
                    rel(*this, cp3IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                } else if ((counterpoint_3->getSpecies() == FOURTH_SPECIES || counterpoint_3->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                    rel(*this, cp3IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+2]));
                } else if (counterpoint_3->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                    if (position == 2) {
                        rel(*this, cp3IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                    } else {
                        rel(*this, cp3IsThird >> (upper_2->getNotes()[measureIndex*4+position] == upper_2->getNotes()[measureIndex*4+position-1]));
                    }
                } else if (counterpoint_3->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                    if (position == 2) {
                        rel(*this, cp3IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                    } else {
                        IntVar currentSpecies(*this, -1, 4);
                        rel(*this, currentSpecies, IRT_EQ, counterpoint_3->getSpeciesArray()[measureIndex*4+position]);
                        rel(*this, (cp3IsThird && currentSpecies == THIRD_SPECIES) >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                        rel(*this, (cp3IsThird && currentSpecies != THIRD_SPECIES) >> (upper_2->getNotes()[measureIndex*4+position] == upper_2->getNotes()[measureIndex*4+position-1]));
                    }
                } else {
                    rel(*this, cp3IsThird >> (upper_2->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4]));
                }
            }
        }
        
        // For upper_3 (sorted position 3)
        if(nVoices >= 4) {
            BoolVar cfIsFourth(*this, 0, 1);
            BoolVar cp1IsFourth(*this, 0, 1);
            BoolVar cp2IsFourth(*this, 0, 1);
            BoolVar cp3IsFourth(*this, 0, 1);

            rel(*this, order[0], IRT_EQ, 3, Reify(cfIsFourth));
            rel(*this, order[1], IRT_EQ, 3, Reify(cp1IsFourth));
            rel(*this, order[2], IRT_EQ, 3, Reify(cp2IsFourth));
            rel(*this, order[3], IRT_EQ, 3, Reify(cp3IsFourth));

            rel(*this, cfIsFourth >> (upper_3->getNotes()[measureIndex*4+position] == cantusFirmus->getNotes()[measureIndex]));
            
            // cp1: mirror lowest logic
            if (counterpoint_1->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp1IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_1->getFirstNotes()[measureIndex]));
            } else if (counterpoint_1->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp1IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_1->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp1IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_1->getSpecies() == FOURTH_SPECIES || counterpoint_1->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp1IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_1->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp1IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp1IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == upper_3->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_1->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp1IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_1->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp1IsFourth && currentSpecies == THIRD_SPECIES) >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp1IsFourth && currentSpecies != THIRD_SPECIES) >> (upper_3->getNotes()[measureIndex*4+position] == upper_3->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp1IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_1->getNotes()[measureIndex*4]));
            }
            
            // cp2: mirror lowest logic
            if (counterpoint_2->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp2IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_2->getFirstNotes()[measureIndex]));
            } else if (counterpoint_2->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp2IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_2->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp2IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_2->getSpecies() == FOURTH_SPECIES || counterpoint_2->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp2IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_2->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp2IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp2IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == upper_3->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_2->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp2IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_2->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp2IsFourth && currentSpecies == THIRD_SPECIES) >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp2IsFourth && currentSpecies != THIRD_SPECIES) >> (upper_3->getNotes()[measureIndex*4+position] == upper_3->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp2IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_2->getNotes()[measureIndex*4]));
            }
            
            // cp3: mirror lowest logic
            if (counterpoint_3->getSpecies() == FIRST_SPECIES) {
                rel(*this, cp3IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_3->getFirstNotes()[measureIndex]));
            } else if (counterpoint_3->getSpecies() == SECOND_SPECIES) {
                int noteIndex = (position < 2) ? 0 : 2;
                rel(*this, cp3IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+noteIndex]));
            } else if (counterpoint_3->getSpecies() == THIRD_SPECIES) {
                rel(*this, cp3IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
            } else if ((counterpoint_3->getSpecies() == FOURTH_SPECIES || counterpoint_3->getSpecies() == FIFTH_SPECIES) && measureIndex == 0) {
                rel(*this, cp3IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+2]));
            } else if (counterpoint_3->getSpecies() == FOURTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp3IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                } else {
                    rel(*this, cp3IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == upper_3->getNotes()[measureIndex*4+position-1]));
                }
            } else if (counterpoint_3->getSpecies() == FIFTH_SPECIES && measureIndex != 0 && measureIndex != nMeasures-1) {
                if (position == 2) {
                    rel(*this, cp3IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                } else {
                    IntVar currentSpecies(*this, -1, 4);
                    rel(*this, currentSpecies, IRT_EQ, counterpoint_3->getSpeciesArray()[measureIndex*4+position]);
                    rel(*this, (cp3IsFourth && currentSpecies == THIRD_SPECIES) >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4+position]));
                    rel(*this, (cp3IsFourth && currentSpecies != THIRD_SPECIES) >> (upper_3->getNotes()[measureIndex*4+position] == upper_3->getNotes()[measureIndex*4+position-1]));
                }
            } else {
                rel(*this, cp3IsFourth >> (upper_3->getNotes()[measureIndex*4+position] == counterpoint_3->getNotes()[measureIndex*4]));
            }
        }
    }
}

void CounterpointProblem::setVoiceLowestFlags(int measureIndex, int nVoices, int size) {
    // Set the isNotLowest flags for each voice
    rel(*this, lowest->getFirstNotes()[measureIndex], IRT_NQ, cantusFirmus->getNotes()[measureIndex], 
        Reify(cantusFirmus->getIsNotLowest()[measureIndex]));
    
    if(nVoices >= 2) {
        // Set isNotLowest boolean for counterpoint_1
        if(counterpoint_1->getSpecies() == FOURTH_SPECIES && measureIndex != size-1) {
            rel(*this, expr(*this, (cantusFirmus->getIsNotLowest()[measureIndex]==1) && 
                (lowest->getFirstNotes()[measureIndex]==counterpoint_1->getNotes()[(measureIndex*4)+2])), 
                IRT_NQ, 1, Reify(counterpoint_1->getIsNotLowest()[measureIndex]));
        } else if(counterpoint_1->getSpecies() == FIFTH_SPECIES && measureIndex == 0) {
            rel(*this, expr(*this, (cantusFirmus->getIsNotLowest()[measureIndex]==1) && 
                (lowest->getFirstNotes()[measureIndex]==counterpoint_1->getNotes()[(measureIndex*4)+2])), 
                IRT_NQ, 1, Reify(counterpoint_1->getIsNotLowest()[measureIndex]));
        } else {
            rel(*this, expr(*this, (cantusFirmus->getIsNotLowest()[measureIndex]==1) && 
                (lowest->getFirstNotes()[measureIndex]==counterpoint_1->getFirstNotes()[measureIndex])), 
                IRT_NQ, 1, Reify(counterpoint_1->getIsNotLowest()[measureIndex]));
        }
    }
    
    if(nVoices == 3) {
        rel(*this, expr(*this, counterpoint_1->getIsNotLowest()[measureIndex] != cantusFirmus->getIsNotLowest()[measureIndex]), 
            IRT_EQ, counterpoint_2->getIsNotLowest()[measureIndex]);
    }
    
    if(nVoices == 4) {
        // Set counterpoint_2 lowest boolean
        if(counterpoint_2->getSpecies() == FOURTH_SPECIES && measureIndex != size-1) {
            rel(*this, expr(*this, (cantusFirmus->getIsNotLowest()[measureIndex]==1) && 
                (counterpoint_1->getIsNotLowest()[measureIndex]==1) && 
                (lowest->getFirstNotes()[measureIndex]==counterpoint_2->getNotes()[(measureIndex*4)+2])), 
                IRT_NQ, 1, Reify(counterpoint_2->getIsNotLowest()[measureIndex]));
        } else if(counterpoint_2->getSpecies() == FIFTH_SPECIES && measureIndex == 0) {
            rel(*this, expr(*this, (cantusFirmus->getIsNotLowest()[measureIndex]==1) && 
                (counterpoint_1->getIsNotLowest()[measureIndex]==1) && 
                (lowest->getFirstNotes()[measureIndex]==counterpoint_2->getNotes()[(measureIndex*4)+2])), 
                IRT_NQ, 1, Reify(counterpoint_2->getIsNotLowest()[measureIndex]));
        } else {
            rel(*this, expr(*this, (cantusFirmus->getIsNotLowest()[measureIndex]==1) && 
                (counterpoint_1->getIsNotLowest()[measureIndex]==1) && 
                (lowest->getFirstNotes()[measureIndex]==counterpoint_2->getFirstNotes()[measureIndex])), 
                IRT_NQ, 1, Reify(counterpoint_2->getIsNotLowest()[measureIndex]));
        }
        
        rel(*this, expr(*this, (cantusFirmus->getIsNotLowest()[measureIndex]==1) && 
            (counterpoint_1->getIsNotLowest()[measureIndex]==1) && 
            (counterpoint_2->getIsNotLowest()[measureIndex]==1)), 
            IRT_NQ, counterpoint_3->getIsNotLowest()[measureIndex]);

        // Set isHighest flags for 4-voice specific constraints
        rel(*this, upper_3->getFirstNotes()[measureIndex], IRT_EQ, cantusFirmus->getNotes()[measureIndex], 
            Reify(cantusFirmus->getIsHighest()[measureIndex]));

        // ... (similar logic for isHighest flags)
    }
}

void CounterpointProblem::setMelodicIntervalConstraints(int measureIndex, int nVoices) {
    // Set melodic interval constraints for the lowest stratum
    vector<IntVarArray> corresponding_m_intervals;

    corresponding_m_intervals.push_back(cantusFirmus->getMelodicIntervals());
    
    for(int j = 0; j < nVoices-1; j++) {
        Part* curr_cp;
        if(j == 0) curr_cp = counterpoint_1;
        else if(j == 1) curr_cp = counterpoint_2;
        else curr_cp = counterpoint_3;

        if(curr_cp->getSpecies() == FIRST_SPECIES) {
            corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(0, 4, curr_cp->getMelodicIntervals().size())));
        } else if(curr_cp->getSpecies() == SECOND_SPECIES) {
            corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(2, 4, curr_cp->getMelodicIntervals().size())));
        } else if(curr_cp->getSpecies() == THIRD_SPECIES) {
            corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(3, 4, curr_cp->getMelodicIntervals().size())));
        } else if(curr_cp->getSpecies() == FOURTH_SPECIES) {
            std::vector<int> selectedIndices;
            for (int i = 4; i < curr_cp->getMelodicIntervals().size(); i += 4) {
                selectedIndices.push_back(i);
            }
            selectedIndices.push_back(curr_cp->getMelodicIntervals().size() - 2);
            
            IntVarArray selectedIntervals(*this, selectedIndices.size());
            for (size_t i = 0; i < selectedIndices.size(); ++i) {
                selectedIntervals[i] = curr_cp->getMelodicIntervals()[selectedIndices[i]];
            }
            corresponding_m_intervals.push_back(selectedIntervals);
        } else if(curr_cp->getSpecies() == FIFTH_SPECIES) {
            corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(2, 4, curr_cp->getMelodicIntervals().size())));
        }
    }

    rel(*this, (cantusFirmus->getIsNotLowest()[measureIndex]==0) >> 
        (lowest->getMelodicIntervals()[measureIndex-1]==corresponding_m_intervals[0][measureIndex-1]));
    rel(*this, (counterpoint_1->getIsNotLowest()[measureIndex]==0) >> 
        (lowest->getMelodicIntervals()[measureIndex-1]==corresponding_m_intervals[1][measureIndex-1]));
    if(nVoices >= 3) {
        rel(*this, (counterpoint_2->getIsNotLowest()[measureIndex]==0) >> 
            (lowest->getMelodicIntervals()[measureIndex-1]==corresponding_m_intervals[2][measureIndex-1]));
    }
    if(nVoices >= 4) {
        rel(*this, (counterpoint_3->getIsNotLowest()[measureIndex]==0) >> 
            (lowest->getMelodicIntervals()[measureIndex-1]==corresponding_m_intervals[3][measureIndex-1]));
    }
}

IntVarArray CounterpointProblem::getSolutionArray(){
    return solutionArray;
}

/**
 * Returns the size of the problem
 * @return an integer representing the size of the vars array           RETURNS THE SIZE OF THE SOLUTION_ARRAY
 */
int CounterpointProblem::getSize(){
    string message = "getSize function called. size = " + std::to_string(solutionArray.size()) + "\n";
    writeToLogFile(message.c_str());
    return this->solutionArray.size();  /// have to use this-> ?
}

/**
 * Returns the values taken by the variables vars in a solution
 * @todo Modify this to return the solution for your problem. This function uses @param size to generate an array of integers
 * @return an array of integers representing the values of the variables in a solution
 */
int* CounterpointProblem::return_solution(){
    string message = "return_solution method. Solution : [";
    int* solution = new int[solutionArray.size()];
    for(int i = 0; i < solutionArray.size(); i++){
        solution[i] = solutionArray[i].val();       
        message += std::to_string(solution[i]) + " ";
    }
    message += "]\n";
    writeToLogFile(message.c_str());
    return solution;
}


int* CounterpointProblem::get_species_array_5sp(int ctp_index){  

    FifthSpeciesCounterpoint* fifth_sp_ctp; 
    if (ctp_index == 0) fifth_sp_ctp = dynamic_cast<FifthSpeciesCounterpoint*>(counterpoint_1); 
    else if (ctp_index == 1) fifth_sp_ctp = dynamic_cast<FifthSpeciesCounterpoint*>(counterpoint_2); 
    else if (ctp_index == 2) fifth_sp_ctp = dynamic_cast<FifthSpeciesCounterpoint*>(counterpoint_3); 
    else{
        writeToLogFile("invalid value of ctp_index given as argument to get_species_array_5sp");
        return nullptr;
    }

    if (!fifth_sp_ctp){
        cout << "type cast of part to fifth species failed " << endl;
        writeToLogFile("type cast of part to fifth species failed");
        return nullptr;
    }

    IntVarArray speciesArray = fifth_sp_ctp->getSpeciesArray();

    string message = "return_species_array method. Species array : [";
    int* solution = new int[speciesArray.size()];
    for(int i = 0; i < speciesArray.size(); i++){
        
        switch (speciesArray[i].val())
        {
        case FIRST_SPECIES:
            solution[i] = 1;
            break;

        case SECOND_SPECIES:
            solution[i] = 2;
            break;

        case THIRD_SPECIES:
            solution[i] = 3;
            break;

        case FOURTH_SPECIES:
            solution[i] = 4;
            break;

        case -1:
            solution[i] = 0;
            break;

        default:
            cout << "invalid value in species array" << endl;
            writeToLogFile("invalid value in species array");
            return nullptr;
        }
        message += std::to_string(solution[i]) + " ";
    }
    message += "]\n";
    writeToLogFile(message.c_str());
    return solution;
}


int* CounterpointProblem::get_extended_cp_domain(int ctp_index){
    vector<int> ext_cp_dom;
    if(ctp_index == 0) ext_cp_dom = counterpoint_1->getDomain();
    else if(ctp_index == 1) ext_cp_dom = counterpoint_2->getDomain();
    else if(ctp_index == 2) ext_cp_dom = counterpoint_3->getDomain();
    else{
        writeToLogFile("invalid value of ctp_index given as argument to get_extended_cp_domain");
        return nullptr;
    }
    writeToLogFile(int_vector_to_string(ext_cp_dom).c_str());
    
    int* ext_cp_dom_int_ptr = new int[ext_cp_dom.size()];
    for(int i = 0; i < ext_cp_dom.size(); i++){
        ext_cp_dom_int_ptr[i] = ext_cp_dom[i];       
        // message += std::to_string(solution[i]) + " ";
    }

    return ext_cp_dom_int_ptr;
}

int CounterpointProblem::get_ext_cp_domain_size(int ctp_index){
    int cpDomSize;
    if(ctp_index == 0) cpDomSize = counterpoint_1->getDomain().size();
    else if(ctp_index == 1) cpDomSize = counterpoint_2->getDomain().size();
    else if(ctp_index == 2) cpDomSize = counterpoint_3->getDomain().size();
    else{
        writeToLogFile("invalid value of ctp_index given as argument to get_extended_cp_domain");
        return -1;
    }

    string message = "getextcpdomainsize function called. size = " + std::to_string(cpDomSize) + "\n";
    writeToLogFile(message.c_str());
    return cpDomSize;
}

Stratum* CounterpointProblem::getLowest(){
    return lowest;
}

Part* CounterpointProblem::getCantusFirmus(){
    return cantusFirmus;
}

Part* CounterpointProblem::getCounterpoint_1(){
    return counterpoint_1;
}

Part* CounterpointProblem::getCounterpoint_2(){
    return counterpoint_2;
}

Part* CounterpointProblem::getCounterpoint_3(){
    return counterpoint_3;
}


/*************************
 * Search engine methods *
 *************************/

//Gecode::Search::TimeStop global_timeout(50000);

/**
 * Creates a search engine for the given problem
 * Should only be used when using OM, otherwise you can create the solver etc in the main file
 * @todo Modify this function to add search options etc
 * @param pb an instance of the Problem class representing a given problem
 * @param type the type of search engine to create (see enumeration in headers/gecode_problem.hpp)
 * @return a search engine for the given problem
 */
Search::Base<CounterpointProblem>* make_solver(CounterpointProblem* pb, int type){
    string message = "make_solver function called. type of solver :\n" + to_string(type) + "\n";
    writeToLogFile(message.c_str());

    Gecode::Search::Options opts;   
    /**@todo add here any options you want*/
    //opts.stop = &global_timeout;
    opts.threads = 1;

    if (type == bab_solver)
        return new BAB<CounterpointProblem>(pb, opts);
    else // default case
        return new DFS<CounterpointProblem>(pb, opts);
}

/**
 * Returns the next solution space for the problem
 * Should only be used when using OM
 * @param solver a solver for the problem
 * @return an instance of the Problem class representing the next solution to the problem
 */
CounterpointProblem* get_next_solution_space(Search::Base<CounterpointProblem>* solver){
    string message = "get_next_solution_space function called.\n";
    // RESET TIMEOUT OBJECT HERE
    //global_timeout.reset();
    CounterpointProblem* sol_space = solver->next();
    if (sol_space == nullptr){
        message += "solution_space was null. \n";
        writeToLogFile(message.c_str());
        return NULL;
    }
    message += sol_space->to_string();
    writeToLogFile(message.c_str());
    return sol_space;
}

void CounterpointProblem::computeCombinedCosts(){
    int sz = 2;
    if(counterpoint_2!=nullptr){
        sz++;
    }
    if(counterpoint_3!=nullptr){
        sz++;
    }
    for (size_t i = 0; i < combinedCostNames.size(); i++) {
        string costName = combinedCostNames[i];
        IntVarArgs to_combined(sz);
        // get the cost of the cantus firmus
        vector<string> cf_vec = cantusFirmus->getToCombineCostNames();
        auto cf_it = std::find(cf_vec.begin(), cf_vec.end(), costName);
        if (cf_it != cf_vec.end()) {
            int index = std::distance(cf_vec.begin(), cf_it);
            to_combined[0] = cantusFirmus->getToCombineCosts()[index];
        } else {
            to_combined[0] = IntVar(*this, 0, 0);
        }
        // get the cost of the counterpoint 1
        vector<string> cp1_vec = counterpoint_1->getToCombineCostNames();
        auto cp1_it = std::find(cp1_vec.begin(), cp1_vec.end(), costName);
        if (cp1_it != cp1_vec.end()) {
            int index = std::distance(cp1_vec.begin(), cp1_it);
            to_combined[1] = counterpoint_1->getToCombineCosts()[index];
        } else {
            to_combined[1] = IntVar(*this, 0, 0);
        }
        // get the cost of the counterpoint 2
        if(counterpoint_2!=nullptr){
            vector<string> cp2_vec = counterpoint_2->getToCombineCostNames();
            auto cp2_it = std::find(cp2_vec.begin(), cp2_vec.end(), costName);
            if (cp2_it != cp2_vec.end()) {
                int index = std::distance(cp2_vec.begin(), cp2_it);
                to_combined[2] = counterpoint_2->getToCombineCosts()[index];
            } else {
                to_combined[2] = IntVar(*this, 0, 0);
            }
        }
        // get the cost of the counterpoint 3
        if(counterpoint_3!=nullptr){
            vector<string> cp3_vec = counterpoint_3->getToCombineCostNames();
            auto cp3_it = std::find(cp3_vec.begin(), cp3_vec.end(), costName);
            if (cp3_it != cp3_vec.end()) {
                int index = std::distance(cp3_vec.begin(), cp3_it);
                to_combined[3] = counterpoint_3->getToCombineCosts()[index];
            } else {
                to_combined[3] = IntVar(*this, 0, 0);
            }
        }
        // sum the costs
        rel(*this, combinedCosts[i], IRT_EQ, expr(*this, sum(to_combined)));
    }
}

void CounterpointProblem::uniteRelaxationCosts(){
    // Collect relaxation cost arrays from all parts
    int totalSize = 0;
    
    if(cantusFirmus && cantusFirmus->getRelaxationCostArray().size() > 0)
        totalSize += cantusFirmus->getRelaxationCostArray().size();
    if(counterpoint_1 && counterpoint_1->getRelaxationCostArray().size() > 0)
        totalSize += counterpoint_1->getRelaxationCostArray().size();
    if(counterpoint_2 && counterpoint_2->getRelaxationCostArray().size() > 0)
        totalSize += counterpoint_2->getRelaxationCostArray().size();
    if(counterpoint_3 && counterpoint_3->getRelaxationCostArray().size() > 0)
        totalSize += counterpoint_3->getRelaxationCostArray().size();
    
    // Add problem-level relaxation costs
    totalSize += problemRelaxationCosts.size();

    if(totalSize == 0){
        // No relaxation costs — totalRelaxationCost stays uninitialized
        return;
    }
    
    IntVarArgs allRelax(totalSize);
    int idx = 0;
    
    if(cantusFirmus && cantusFirmus->getRelaxationCostArray().size() > 0){
        for(int i = 0; i < cantusFirmus->getRelaxationCostArray().size(); i++){
            allRelax[idx++] = cantusFirmus->getRelaxationCostArray()[i];
        }
    }
    if(counterpoint_1 && counterpoint_1->getRelaxationCostArray().size() > 0){
        for(int i = 0; i < counterpoint_1->getRelaxationCostArray().size(); i++){
            allRelax[idx++] = counterpoint_1->getRelaxationCostArray()[i];
        }
    }
    if(counterpoint_2 && counterpoint_2->getRelaxationCostArray().size() > 0){
        for(int i = 0; i < counterpoint_2->getRelaxationCostArray().size(); i++){
            allRelax[idx++] = counterpoint_2->getRelaxationCostArray()[i];
        }
    }
    if(counterpoint_3 && counterpoint_3->getRelaxationCostArray().size() > 0){
        for(int i = 0; i < counterpoint_3->getRelaxationCostArray().size(); i++){
            allRelax[idx++] = counterpoint_3->getRelaxationCostArray()[i];
        }
    }
    for(int i = 0; i < problemRelaxationCosts.size(); i++){
        allRelax[idx++] = problemRelaxationCosts[i];
    }
    
    // Sum all relaxation costs into totalRelaxationCost
    totalRelaxationCost = IntVar(*this, 0, totalSize);
    rel(*this, totalRelaxationCost, IRT_EQ, expr(*this, sum(allRelax)));
}