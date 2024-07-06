#include "headers/Utilities.hpp"
#include "headers/CounterpointUtils.hpp"
#include "headers/Parts/Part.hpp"
#include "headers/Parts/FirstSpeciesCounterpoint.hpp"
#include "headers/Parts/SecondSpeciesCounterpoint.hpp"
#include "headers/Parts/CantusFirmus.hpp"
#include "headers/CounterpointProblems/TwoVoiceCounterpoint.hpp"
#include "headers/CounterpointProblems/CounterpointProblem.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    int upper_bound_domain = 72;
    int lower_bound_domain = 60;
    vector<int> species = {FIRST_SPECIES};
    vector<int> cantusFirmus = {60, 67, 65, 64, 62, 60};
    int size = cantusFirmus.size();
    int v_type = 1;

    // create a new problem
    // auto* problem = new TwoVoiceCounterpoint(cantusFirmus, species[0], C, lower_bound_domain, upper_bound_domain);
    auto* problem = create_problem(cantusFirmus, species, C, lower_bound_domain, upper_bound_domain, v_type);
    //cout << problem->to_string() << endl;
    // create a new search engine
    BAB<CounterpointProblem> e(problem);

    int nb_sol = 0;
    while(CounterpointProblem* pb = e.next()){
        nb_sol++;
        cout << "Solution " << nb_sol << ": " << endl;
        cout << pb->to_string() << endl;
        // cout << int_vector_to_string(cantusFirmus) << endl;

        delete pb;
        if (nb_sol >= 10)
            break;
    }
    cout << "No (more) solutions." << endl;

    return 0;
}

