#ifndef FUXCP_BASE_VOICE_HPP
#define FUXCP_BASE_VOICE_HPP

#include "Utilities.hpp"

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

using namespace Gecode;
using namespace Gecode::Search;
using namespace std;

/// abstract class!!! Should not be instanciated
/// This class represents a Voice, so it creates all the variables associated to that part and posts the constraints that are species independent
class Voice{
    protected:
        int nMeasures;
        int size;

        int lowerBound;
        int upperBound;
        int species;

        int voice_type;

        IntVarArray notes;
        IntVarArray h_intervals;  // with respect to lowest stratum. TODO set to 0 if isLowest
        IntVarArray m_intervals_brut;
        IntVarArray motions;

        BoolVarArray isNotLowest;
        BoolVarArray isHighest;
        // motions ? (w/r to lowest)

        // Stratum lowestStratum;


    public:
        Voice(Home home, int nMes, int lb, int ub, int v_type, int sp);

        Voice(Home home, Voice& s);  // clone constructor

        int getNMeasures() { return nMeasures; }
        int getSize() { return size; }

        int getLowerBound() { return lowerBound; }
        int getUpperBound() { return upperBound; }
        int getSpecies() { return species; }

        IntVarArray getNotes() { return notes; }
        IntVarArray getMelodicIntervals();

        /// must be implemented in the child classes, returns the variables to branch on
        // virtual IntVarArray getBranchingNotes();

        virtual Voice* clone(Home home);

        virtual string to_string() const;

        virtual IntVarArgs getFirstNotes();

        BoolVar getIsNotLowestIdx(int idx);

        BoolVarArray getIsHighest();

        IntVarArray getHInterval();

        IntVarArgs getSecondHInterval();
};


#endif //FUXCP_BASE_VOICE_HPP
