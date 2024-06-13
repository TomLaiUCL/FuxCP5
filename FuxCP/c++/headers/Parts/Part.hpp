//
// Created by Damien Sprockeels on 11/06/2024.
//

#ifndef FUXCP_BASE_COUNTERPOINT_HPP
#define FUXCP_BASE_COUNTERPOINT_HPP

#include "../Utilities.hpp"

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
class Part : public Space{
    protected:
        int nMeasures;
        int size;
        int species;
        int key;

        int lowerBound;
        int upperBound;

        IntVarArray cp;
        IntVarArray hIntervalsCpCf;
//        IntVarArray mIntervalsCp;
//        IntVarArray motionsCfCp;

        vector<int> cantusFirmus;
        vector<int> melodicIntervalsCf;

        /// Auxiliary variables, not representing Cp but useful for constraints

    public:
        Part(int nMes, int sp, vector<int> cf, int lb, int ub, int k);

        Part(Part& s);

        int getNMeasures() { return nMeasures; }
        int getSize() { return size; }
        int getSpecies() { return species; }
        int getKey() { return key; }

        int getLowerBound() { return lowerBound; }
        int getUpperBound() { return upperBound; }

        vector<int> getCantusFirmus() { return cantusFirmus; }
        vector<int> getMelodicIntervalsCf() { return melodicIntervalsCf; }

        //@todo general melodic rules should go here
        /// G6 from Thibault This rule is general so it is defined here, but must be called in the child classes because the cp array is not initialised in this class
        void noChromaticMelodies(IntVarArray mIntervals);

        /// must be implemented in the child classes, returns the variables to branch on
        virtual IntVarArray getBranchingNotes();

        virtual Space *copy();

        virtual string to_string();
};


#endif //FUXCP_BASE_COUNTERPOINT_HPP
