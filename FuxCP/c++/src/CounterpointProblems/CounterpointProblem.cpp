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
    lowerBound = ((6 * v_type) - 6) + cf[0];
    upperBound = ((6 * v_type) + 12) + cf[0];
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



    writeToLogFile("counterpointproblem constructor"); 

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
    sorted_voices = s.sorted_voices;
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
    for(int i = 0; i < sorted_voices.size(); i++){
        sorted_voices[i].update(*this, s.sorted_voices[i]);
    }
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
    text += "All costs : \n";
    text += intVarArray_to_string(unitedCosts); 
    text += "\n";
    text += "Lowest : \n";
    text += lowest->to_string();
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
}

void CounterpointProblem::setLowest(Part* cp2, Part* cp3, Stratum* upper1, Stratum* upper2, Stratum* upper3){
    int nVoices = 2;
    if(upper2!=nullptr){
        nVoices ++;
    }
    if(upper3!=nullptr){
        nVoices++;
    }

    int size = counterpoint_1->getNMeasures();
    sorted_voices = {};
    for(int i = 0; i < size; i++){
        IntVarArray voices = IntVarArray(*this, nVoices, 0, 127);
        rel(*this, voices[0], IRT_EQ, cantusFirmus->getNotes()[i]);
        rel(*this, voices[1], IRT_EQ, counterpoint_1->getFirstNotes()[i]);
        if(nVoices>=3){
            rel(*this, voices[2], IRT_EQ, counterpoint_2->getFirstNotes()[i]);
        }
        if(nVoices>=4){
            rel(*this, voices[3], IRT_EQ, counterpoint_3->getFirstNotes()[i]);
        }
        IntVarArray order = IntVarArray(*this, nVoices, 0, nVoices-1);
        sorted_voices.push_back(IntVarArray(*this, nVoices, 0, 127));

        sorted(*this, voices, sorted_voices[i], order);

        lowest->setNote(*this, i*4, sorted_voices[i][0]);

        upper1->setNote(*this, i*4, sorted_voices[i][1]);
        if(nVoices>=3){
            upper2->setNote(*this, i*4, sorted_voices[i][2]);
        }
        if(nVoices>=4){
            upper3->setNote(*this, i*4, sorted_voices[i][3]);
        }

        BoolVar cfLowest = BoolVar(*this, 0, 1);
        BoolVar cp1Lowest = BoolVar(*this, 0, 1);
        BoolVar cp1Bass = BoolVar(*this, 0, 1);

        //rel(*this, lowest->getFirstNotes()[i], IRT_EQ, counterpoint_1->getFirstNotes()[i], Reify(cp1Bass));

        //INSHALLAH IS LOWEST WORKS
        //rel(*this, (cantusFirmus->getIsLowestIdx(i)) >> (!counterpoint_1->getIsLowestIdx(i)));

        //rel(*this, cp1Bass, BOT_IMP, cfLowest, counterpoint_1->getIsLowestIdx(i));

        //rel(*this, cfLowest, BOT_XOR, cantusFirmus->getIsLowestIdx(i), 1);
        //rel(*this, cp1Lowest, BOT_XOR, counterpoint_1->getIsLowestIdx(i), 1);

        rel(*this, lowest->getFirstNotes()[i], IRT_NQ, cantusFirmus->getNotes()[i], Reify(cantusFirmus->getIsLowestIdx(i)));
        
        if(nVoices==2){

            rel(*this, expr(*this, (cantusFirmus->getIsLowestIdx(i)==1)&&(lowest->getFirstNotes()[i]==counterpoint_1->getFirstNotes()[i])), IRT_NQ, 1, Reify(counterpoint_1->getIsLowestIdx(i))); 

        } else if(nVoices==3){

            rel(*this, expr(*this, (cantusFirmus->getIsLowestIdx(i)==1)&&(lowest->getFirstNotes()[i]==counterpoint_1->getFirstNotes()[i])), IRT_NQ, 1, Reify(counterpoint_1->getIsLowestIdx(i))); //else it is the cp1
            rel(*this, expr(*this, counterpoint_1->getIsLowestIdx(i)!=cantusFirmus->getIsLowestIdx(i)), IRT_EQ, counterpoint_2->getIsLowestIdx(i)); //else it is the cp2 (in 3 voices)

            //rel(home, cantus->getIsLowestIdx(i), IRT_EQ, 0, Reify(cp1->getIsLowestIdx(i)));
            //rel(*this, ((cantus->getIsLowestIdx(i)==1)&&(this->getFirstNotes()[i]==cp1->getFirstNotes()[i])) >> (cp1->getIsLowestIdx(i)==0));
            //rel(*this, ((cantus->getIsLowestIdx(i)==0)||(this->getFirstNotes()[i]!=cp1->getFirstNotes()[i])) >> (cp1->getIsLowestIdx(i)==1));

            //rel(*this, ((cantus->getIsLowestIdx(i)==1)&&(cp1->getIsLowestIdx(i)==1)) >> (cp2->getIsLowestIdx(i)==0));
            //rel(*this, ((cantus->getIsLowestIdx(i)==0)||(cp1->getIsLowestIdx(i)==0)) >> (cp2->getIsLowestIdx(i)==1));
        } else {

            rel(*this, expr(*this, (cantusFirmus->getIsLowestIdx(i)==1)&&(lowest->getFirstNotes()[i]==counterpoint_1->getFirstNotes()[i])), IRT_NQ, 1, Reify(counterpoint_1->getIsLowestIdx(i)));
            rel(*this, expr(*this, (cantusFirmus->getIsLowestIdx(i)==1)&&(counterpoint_1->getIsLowestIdx(i)==1)&&(lowest->getFirstNotes()[i]==counterpoint_2->getFirstNotes()[i])), IRT_NQ, 1, Reify(counterpoint_2->getIsLowestIdx(i)));
            rel(*this, expr(*this, (cantusFirmus->getIsLowestIdx(i)==1) && (counterpoint_1->getIsLowestIdx(i)==1) && (counterpoint_2->getIsLowestIdx(i)==1)), IRT_NQ, counterpoint_3->getIsLowestIdx(i));

            //rel(*this, ((cantus->getIsLowestIdx(i)==1)&&(this->getFirstNotes()[i]==cp1->getFirstNotes()[i])) >> (cp1->getIsLowestIdx(i)==0));
            //rel(*this, ((cantus->getIsLowestIdx(i)==0)||(this->getFirstNotes()[i]!=cp1->getFirstNotes()[i])) >> (cp1->getIsLowestIdx(i)==1));

            //rel(*this, ((cantus->getIsLowestIdx(i)==1)&&(cp1->getIsLowestIdx(i)==1)&&(this->getFirstNotes()[i]==cp2->getFirstNotes()[i])) >> (cp2->getIsLowestIdx(i)==0));
            //rel(*this, ((cantus->getIsLowestIdx(i)==0)||(cp1->getIsLowestIdx(i)==0)||(this->getFirstNotes()[i]!=cp2->getFirstNotes()[i])) >> (cp2->getIsLowestIdx(i)==1));

            //rel(*this, ((cantus->getIsLowestIdx(i)==1)&&(cp1->getIsLowestIdx(i)==1)&&(cp2->getIsLowestIdx(i)==1)) >> (cp3->getIsLowestIdx(i)==0));
            //rel(*this, ((cantus->getIsLowestIdx(i)==0)||(cp1->getIsLowestIdx(i)==0)||(cp2->getIsLowestIdx(i)==0)) >> (cp3->getIsLowestIdx(i)==1));
        }

        if(i > 0){

            vector<IntVarArray> corresponding_m_intervals;

            //BoolVar cp2_is_lowest = BoolVar(*this, 0, 1);
            //BoolVar cp3_is_lowest = BoolVar(*this, 0, 1);

            //if(nVoices>=3){
            //    rel(*this, cp2_is_lowest, BOT_XOR, cp2->getIsLowestIdx(i), 1);
            //}
            //if(nVoices>=4){
            //    rel(*this, cp3_is_lowest, BOT_XOR, cp3->getIsLowestIdx(i), 1);
            //}

            corresponding_m_intervals.push_back(cantusFirmus->getMelodicIntervals());
            for(int j = 0; j < nVoices-1; j++){
                Voice* curr_cp;

                if(j==0) curr_cp=counterpoint_1;
                else if(j==1) curr_cp=counterpoint_2;
                else curr_cp = counterpoint_3;

                if(curr_cp->getSpecies()==FIRST_SPECIES){
                    corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(0, 4, curr_cp->getMelodicIntervals().size())));
                } else if(curr_cp->getSpecies()==SECOND_SPECIES){
                    corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(2, 4, curr_cp->getMelodicIntervals().size())));
                } else if(curr_cp->getSpecies()==THIRD_SPECIES){
                    corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(3, 4, curr_cp->getMelodicIntervals().size())));
                } else if(curr_cp->getSpecies()==FOURTH_SPECIES){
                    corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(2, 4, curr_cp->getMelodicIntervals().size())));
                } else{
                    corresponding_m_intervals.push_back(IntVarArray(*this, curr_cp->getMelodicIntervals().slice(2, 4, curr_cp->getMelodicIntervals().size())));
                }
            }

            //rel(*this, corresponding_m_intervals[0][i-1], IRT_EQ, lowest->getMelodicIntervals()[i-1], Reify(cfLowest));
            //rel(*this, corresponding_m_intervals[1][i-1], IRT_EQ, lowest->getMelodicIntervals()[i-1], Reify(cp1Lowest));
            rel(*this, (cantusFirmus->getIsLowestIdx(i)==0) >> (lowest->getMelodicIntervals()[i-1]==corresponding_m_intervals[0][i-1]));
            rel(*this, (counterpoint_1->getIsLowestIdx(i)==0) >> (lowest->getMelodicIntervals()[i-1]==corresponding_m_intervals[1][i-1]));

            //rel(*this,  (lowest->getMelodicIntervals()[i-1]==corresponding_m_intervals[0][i-1]) >> (cantusFirmus->getIsLowestIdx(i)==0));
            //rel(*this,  (lowest->getMelodicIntervals()[i-1]==corresponding_m_intervals[1][i-1]) >> (counterpoint_1->getIsLowestIdx(i)==0));
            if(nVoices>=3){
                rel(*this, (counterpoint_2->getIsLowestIdx(i)==0) >> (lowest->getMelodicIntervals()[i-1]==corresponding_m_intervals[2][i-1]));
            }
            if(nVoices>=4){
                rel(*this, (counterpoint_3->getIsLowestIdx(i)==0) >> (lowest->getMelodicIntervals()[i-1]==corresponding_m_intervals[3][i-1]));
            }

        }
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
        solution[i] = solutionArray[i].val();       // TODO : modify!!
        message += std::to_string(solution[i]) + " ";
    }
    message += "]\n";
    writeToLogFile(message.c_str());
    return solution;
}



/*************************
 * Search engine methods *
 *************************/

Gecode::Search::TimeStop global_timeout(500000);

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
    // opts.stop = &global_timeout;
    opts.threads = 32;

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
    global_timeout.reset();
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

