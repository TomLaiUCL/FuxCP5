//
// Created by Damien Sprockeels on 11/06/2024.
//

#ifndef FUXCP_BASE_COUNTERPOINT_HPP
#define FUXCP_BASE_COUNTERPOINT_HPP

#include "../Utilities.hpp"
#include "../Voice.hpp"
#include "../Stratum.hpp"

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
        int nVoices;
        int borrowMode;
        //Stratum* lowest;
        IntVarArray melodicDegreeCost;
        IntVarArray fifthCostArray;
        IntVarArray octaveCostArray;
        BoolVarArray is_off;
        IntVarArray offCostArray;
        IntVarArray costs;
        IntVarArray varietyCostArray;
        IntVarArray directCostArray;
        BoolVarArray isConsonance;

        vector<int> borrowed_scale;
        vector<int> scale;
        vector<int> chromatic_scale;

        vector<int> cp_range;

        vector<int> extended_domain;
        vector<int> off_domain;

        int secondCost;
        int thirdCost;
        int fourthCost;
        int tritoneCost;
        int fifthCost;
        int sixthCost;
        int seventhCost;
        int octaveCost;

        int borrowCost;
        int h_fifthCost;
        int h_octaveCost;
        int succCost;
        int varietyCost;
        int triadCost;
        int directMoveCost;
        int penultCost;

        int directCost;
        int obliqueCost;
        int contraryCost;
    public:
        Part(Home home, int nMes, int sp, vector<int> cf, int lb, int ub, int k, int v_type, vector<int> m_costs, vector<int> g_costs, int nV, int bm);

        // Part(Part& s); (no longer copy constructor since not a space anymore. Now just a clone constructor to deep copy the object (called by the Space's copy constructor))
        Part(Home home, Part& s);  // clone constructor

        int getSpecies() { return species; }
        int getKey() { return key; }

        /// must be implemented in the child classes, returns the variables to branch on
        // virtual IntVarArray getBranchingNotes();

        virtual Part* clone(Home home) override;

        virtual string to_string() const;

        virtual IntVarArray getBranchingNotes();

        virtual IntVarArray getFirstHInterval();

        virtual IntVarArray getFirstMInterval();

        virtual IntVarArray getMotions();

        IntVarArray getPartNotes();

        IntVarArray getCosts();

        int getSuccCost();

        int getVarietyCost();

        IntVar getVarietyArray(int idx);

        virtual int getHIntervalSize();
};


#endif //FUXCP_BASE_COUNTERPOINT_HPP
