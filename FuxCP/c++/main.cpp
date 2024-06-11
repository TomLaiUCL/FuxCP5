#include "headers/gecode_problem.hpp"
#include "headers/Utilities.hpp"
#include "headers/Counterpoint.hpp"
#include "headers/FirstSpeciesCounterpoint.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    int size = 3;
    int upper_bound_domain = 5;
    int lower_bound_domain = 1;
    int species = 0;
    vector<int> cantusFirmus = {1, 2, 3};
    // create a new problem
    Counterpoint *pb = new FirstSpeciesCounterpoint({60, 67, 65, 64, 62, 60}, 60,72, C, FIRST_SPECIES);

    // create a new search engine
    BAB<Counterpoint> e(pb);
    delete pb;

    int nb_sol = 0;

    while(Counterpoint* pb = e.next()){
        nb_sol++;
        cout << "Solution " << nb_sol << ": " << endl;
        cout << pb->to_string() << endl;
        delete pb;
    }
    cout << "No (more) solutions." << endl;

    return 0;
}

