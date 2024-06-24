//
// Created by Damien Sprockeels on 11/06/2024.
//

#ifndef FUXCP_BASE_COUNTERPOINT_HPP
#define FUXCP_BASE_COUNTERPOINT_HPP

#include "../Utilities.hpp"
#include "../Voice.hpp"

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

using namespace Gecode;
using namespace Gecode::Search;
using namespace std;

/// abstract class!!! Should not be instanciated
/// This class represents a part, so it creates all the variables associated to that part and posts the constraints that are species independent
class Part : public Voice {
    protected:
        int species;
        int key;


    public:
        Part(Home home, int nMes, int sp, vector<int> cf, int lb, int ub, int k);

        // Part(Part& s); (no longer copy constructor since not a space anymore. Now just a clone constructor to deep copy the object (called by the Space's copy constructor))
        Part(Home home, Part& s);  // clone constructor

        int getSpecies() { return species; }
        int getKey() { return key; }

        /// must be implemented in the child classes, returns the variables to branch on
        // virtual IntVarArray getBranchingNotes();

        virtual Part* clone(Home home);

        virtual string to_string() const;
};


#endif //FUXCP_BASE_COUNTERPOINT_HPP
