#include "headers/Utilities.hpp"
#include "headers/CounterpointUtils.hpp"
#include "headers/Parts/Part.hpp"
#include "headers/Parts/FirstSpeciesCounterpoint.hpp"
#include "headers/Parts/SecondSpeciesCounterpoint.hpp"
#include "headers/Parts/CantusFirmus.hpp"
#include "headers/CounterpointProblems/TwoVoiceCounterpoint.hpp"
#include "headers/CounterpointProblems/CounterpointProblem.hpp"
#include "headers/fuxTest.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    if(argc==1){
        cout << argv[0] << endl;
        cout << "-------------" << endl;
        vector<Species> species = {FOURTH_SPECIES};
        //la do si re do mi fa mi re do si la
        //57 60 59 62 60 64 65 64 62 60 59 57
        // vector<int> cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; //1sp 2v cf
        vector<int> cantusFirmus = {60,   62,   65,   64,   67,   65,   64,   62,   60};
        
        int size = cantusFirmus.size();
        vector<int> v_type = {3, 1};

        vector<int> melodic_params = {0, 1, 1, 576, 2, 2, 2, 1};
        //borrow, h-5th, h-octave, succ, variety, triad, direct move, penult rule check
        vector<int> general_params = {4, 1, 1, 2, 2, 2, 8, 1};

        //penult sixth, non-ciambata, con m after skip, h triad 3rd species, m2 eq zero, no syncopation, pref species slider
        vector<int> specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};


        vector<int> importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};

        int borrowMode = 1;

        // create a new problem
        // auto* problem = new TwoVoiceCounterpoint(cantusFirmus, species[0], C, lower_bound_domain, upper_bound_domain);
        auto* problem = create_problem(cantusFirmus, species, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
        //cout << problem->to_string() << endl;
        // create a new search engine
        /*rel(problem->getHome(), problem->getSolutionArray()[2], IRT_EQ, 79);
        rel(problem->getHome(), problem->getSolutionArray()[3], IRT_EQ, 79);

        rel(problem->getHome(), problem->getSolutionArray()[4], IRT_EQ, 79);
        rel(problem->getHome(), problem->getSolutionArray()[5], IRT_EQ, 74);
        rel(problem->getHome(), problem->getSolutionArray()[6], IRT_EQ, 77);
        rel(problem->getHome(), problem->getSolutionArray()[7], IRT_EQ, 76);

        rel(problem->getHome(), problem->getSolutionArray()[8], IRT_EQ, 81);
        rel(problem->getHome(), problem->getSolutionArray()[9], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[10], IRT_EQ, 83);
        rel(problem->getHome(), problem->getSolutionArray()[11], IRT_EQ, 86);

        rel(problem->getHome(), problem->getSolutionArray()[12], IRT_EQ, 88);
        rel(problem->getHome(), problem->getSolutionArray()[13], IRT_EQ, 86);
        rel(problem->getHome(), problem->getSolutionArray()[14], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[15], IRT_EQ, 84);

        rel(problem->getHome(), problem->getSolutionArray()[16], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[17], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[18], IRT_EQ, 79);
        rel(problem->getHome(), problem->getSolutionArray()[19], IRT_EQ, 79);

        rel(problem->getHome(), problem->getSolutionArray()[20], IRT_EQ, 79);
        rel(problem->getHome(), problem->getSolutionArray()[21], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[22], IRT_EQ, 74);
        rel(problem->getHome(), problem->getSolutionArray()[23], IRT_EQ, 84);

        rel(problem->getHome(), problem->getSolutionArray()[24], IRT_EQ, 72);
        rel(problem->getHome(), problem->getSolutionArray()[25], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[26], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[27], IRT_EQ, 84);

        rel(problem->getHome(), problem->getSolutionArray()[28], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[29], IRT_EQ, 84);
        rel(problem->getHome(), problem->getSolutionArray()[30], IRT_EQ, 83);
        rel(problem->getHome(), problem->getSolutionArray()[31], IRT_EQ, 83);

        rel(problem->getHome(), problem->getSolutionArray()[32], IRT_EQ, 79);

        rel(problem->getHome(), problem->getSolutionArray()[33+2], IRT_EQ, 69);
        rel(problem->getHome(), problem->getSolutionArray()[33+3], IRT_EQ, 69);

        rel(problem->getHome(), problem->getSolutionArray()[33+4], IRT_EQ, 69);
        rel(problem->getHome(), problem->getSolutionArray()[33+5], IRT_EQ, 62);
        rel(problem->getHome(), problem->getSolutionArray()[33+6], IRT_EQ, 62);
        rel(problem->getHome(), problem->getSolutionArray()[33+7], IRT_EQ, 62);

        rel(problem->getHome(), problem->getSolutionArray()[33+8], IRT_EQ, 62);
        rel(problem->getHome(), problem->getSolutionArray()[33+9], IRT_EQ, 64);
        rel(problem->getHome(), problem->getSolutionArray()[33+10], IRT_EQ, 69);
        rel(problem->getHome(), problem->getSolutionArray()[33+11], IRT_EQ, 69);

        rel(problem->getHome(), problem->getSolutionArray()[33+12], IRT_EQ, 60);
        rel(problem->getHome(), problem->getSolutionArray()[33+13], IRT_EQ, 60);
        rel(problem->getHome(), problem->getSolutionArray()[33+14], IRT_EQ, 64);
        rel(problem->getHome(), problem->getSolutionArray()[33+15], IRT_EQ, 62);

        rel(problem->getHome(), problem->getSolutionArray()[33+16], IRT_EQ, 60);
        rel(problem->getHome(), problem->getSolutionArray()[33+17], IRT_EQ, 64);
        //rel(problem->getHome(), problem->getSolutionArray()[33+18], IRT_EQ, 65);
        //rel(problem->getHome(), problem->getSolutionArray()[33+19], IRT_EQ, 67);

        rel(problem->getHome(), problem->getSolutionArray()[33+20], IRT_EQ, 62);
        rel(problem->getHome(), problem->getSolutionArray()[33+21], IRT_EQ, 60);
        rel(problem->getHome(), problem->getSolutionArray()[33+22], IRT_EQ, 62);
        rel(problem->getHome(), problem->getSolutionArray()[33+23], IRT_EQ, 62);

        rel(problem->getHome(), problem->getSolutionArray()[33+24], IRT_EQ, 60);
        rel(problem->getHome(), problem->getSolutionArray()[33+25], IRT_EQ, 60);
        rel(problem->getHome(), problem->getSolutionArray()[33+26], IRT_EQ, 64);
        rel(problem->getHome(), problem->getSolutionArray()[33+27], IRT_EQ, 64);

        rel(problem->getHome(), problem->getSolutionArray()[33+28], IRT_EQ, 64);
        rel(problem->getHome(), problem->getSolutionArray()[33+29], IRT_EQ, 64);
        rel(problem->getHome(), problem->getSolutionArray()[33+30], IRT_EQ, 62);
        rel(problem->getHome(), problem->getSolutionArray()[33+31], IRT_EQ, 62);

        rel(problem->getHome(), problem->getSolutionArray()[33+32], IRT_EQ, 64);*/
        BAB<CounterpointProblem> e(problem);
        
        int nb_sol = 0;
        while(CounterpointProblem* pb = e.next()){
            
            nb_sol++;
            cout << "Solution " << nb_sol << ": " << endl;
            cout << pb->to_string() << endl;
            cout << pb->getSize() << endl;
            // cout << int_vector_to_string(cantusFirmus) << endl;

            delete pb;
            //if (nb_sol >= 1)
            //    break;
        }
        cout << "No (more) solutions." << endl;
    } else if(argc==3){
        FuxTest* test = new FuxTest(atoi(argv[1]), atoi(argv[2]));
        vector<Species> species = test->getSpList();
        //la do si re do mi fa mi re do si la
        //57 60 59 62 60 64 65 64 62 60 59 57
        vector<int> cantusFirmus = test->getCf();
        
        int size = cantusFirmus.size();
        vector<int> v_type = test->getVType();

        vector<int> melodic_params = {0, 1, 1, 576, 2, 2, 2, 1};
        //borrow, h-5th, h-octave, succ, variety, triad, direct move, penult rule check
        vector<int> general_params = {4, 1, 1, 2, 2, 2, 8, 1};

        //penult sixth, non-ciambata, con m after skip, h triad 3rd species, m2 eq zero, no syncopation, pref species slider
        vector<int> specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};

        vector<int> importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};

        int borrowMode = test->getBMode();

        // create a new problem
        // auto* problem = new TwoVoiceCounterpoint(cantusFirmus, species[0], C, lower_bound_domain, upper_bound_domain);
        auto* problem = create_problem(cantusFirmus, species, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
        //cout << problem->to_string() << endl;
        // create a new search engine

        if(atoi(argv[1])==125){
            for(int j = 0; j < problem->getSize(); j++){
                if(j!=10){
                    rel(problem->getHome(), problem->getSolutionArray()[j], IRT_EQ, test->getCp()[j]);
                }
            }
        }
        else if(atoi(argv[1])==74){
            for(int j = 0; j < test->getIdx(); j++){
                if(j!=0){
                    rel(problem->getHome(), problem->getSolutionArray()[j], IRT_EQ, test->getCp()[j]);
                }
            }
        }
        else{
            if(species[0]==FIRST_SPECIES || species[0]==THIRD_SPECIES ){
                for(int j = 0; j < test->getIdx(); j++){
                    rel(problem->getHome(), problem->getSolutionArray()[j], IRT_EQ, test->getCp()[j]);
                }
            } else if(species[0]==SECOND_SPECIES || species[0]==FOURTH_SPECIES){
                for(int j = 1; j < test->getIdx(); j++){
                    rel(problem->getHome(), problem->getSolutionArray()[j], IRT_EQ, test->getCp()[j]);
                }
            }
        }

        BAB<CounterpointProblem> e(problem);

        int nb_sol = 0;
        while(CounterpointProblem* pb = e.next()){
            nb_sol++;
            cout << "Solution " << nb_sol << ": " << endl;
            cout << pb->to_string() << endl;
            // cout << int_vector_to_string(cantusFirmus) << endl;

            delete pb;
            if (nb_sol >= 1)
                break;
        }
        cout << "No (more) solutions." << endl;
    } else if(argc==2){
        FuxTest* test = new FuxTest(atoi(argv[1]));
        vector<Species> species = test->getSpList();
        //la do si re do mi fa mi re do si la
        //57 60 59 62 60 64 65 64 62 60 59 57
        vector<int> cantusFirmus = test->getCf();
        
        int size = cantusFirmus.size();
        vector<int> v_type = test->getVType();

        vector<int> melodic_params = {0, 1, 1, 576, 2, 2, 2, 1};
        //borrow, h-5th, h-octave, succ, variety, triad, direct move, penult rule check
        vector<int> general_params = {4, 1, 1, 2, 2, 2, 8, 1};

        //penult sixth, non-ciambata, con m after skip, h triad 3rd species, m2 eq zero, no syncopation, pref species slider
        vector<int> specific_params = {8 , 4 , 0 , 2 , 1 , 8 , 50};

        vector<int> importance = {8,7,5,2,9,3,14,12,6,11,4,10,1,13};

        int borrowMode = test->getBMode();

        // create a new problem
        // auto* problem = new TwoVoiceCounterpoint(cantusFirmus, species[0], C, lower_bound_domain, upper_bound_domain);
        auto* problem = create_problem(cantusFirmus, species, v_type, melodic_params, general_params, specific_params,
            importance, borrowMode);
        //cout << problem->to_string() << endl;
        // create a new search engine
        if(atoi(argv[1])==125){
            cout << "HERE" << endl;
            for(int j = 0; j < problem->getSize(); j++){
                if(j!=10){
                    rel(problem->getHome(), problem->getSolutionArray()[j], IRT_EQ, test->getCp()[j]);
                }
            }
        }
        else{
            if(species[0]==FIRST_SPECIES || species[0]==THIRD_SPECIES){
                for(int j = 0; j < problem->getSize(); j++){
                    rel(problem->getHome(), problem->getSolutionArray()[j], IRT_EQ, test->getCp()[j]);
                }
            } else if(species[0]==SECOND_SPECIES || species[0]==FOURTH_SPECIES){
                for(int j = 1; j < problem->getSize(); j++){
                    rel(problem->getHome(), problem->getSolutionArray()[j], IRT_EQ, test->getCp()[j]);
                }
            }
        }

        BAB<CounterpointProblem> e(problem);

        int nb_sol = 0;
        while(CounterpointProblem* pb = e.next()){
            nb_sol++;
            
            // cout << int_vector_to_string(cantusFirmus) << endl;
            cout << "Solution " << nb_sol << ": " << endl;
            cout << pb->to_string() << endl;
            delete pb;
            if (nb_sol >= 1)
                break;
        }
        if(nb_sol!=1){
            cout << "This test did not pass. An error was found!" << endl;
        } else {
            cout << "This test passed successfully!" << endl;
        }
    } else {
        throw std::invalid_argument("Wrong amount of arguments!");
    }

    return 0;
}

