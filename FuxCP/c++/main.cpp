#include "headers/gecode_problem.hpp"
#include "headers/Utilities.hpp"
#include "headers/Part.hpp"
#include "headers/FirstSpeciesCounterpoint.hpp"
#include "headers/SecondSpeciesCounterpoint.hpp"

using namespace Gecode;
using namespace std;

int main(int argc, char* argv[]) {
    int size = 3;
    int upper_bound_domain = 72;
    int lower_bound_domain = 60;
    int species = 0;
    vector<int> cantusFirmus = {60, 67, 65, 64, 62, 60};

    // create a new problem
    Part *p = new FirstSpeciesCounterpoint(cantusFirmus.size(), cantusFirmus, lower_bound_domain, upper_bound_domain, C);

    // create a new search engine
    BAB<Part> e(p);
    delete p;

    int nb_sol = 0;

    while(Part* pb = e.next()){
        nb_sol++;
        cout << "Solution " << nb_sol << ": " << endl;
        cout << pb->to_string() << endl;
        cout << int_vector_to_string(cantusFirmus) << endl;

        delete pb;
    }
    cout << "No (more) solutions." << endl;

    return 0;
}

