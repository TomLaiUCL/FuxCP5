#ifndef MYPROJECT_THIRDSPECIESCOUNTERPOINT_HPP
#define MYPROJECT_THIRDSPECIESCOUNTERPOINT_HPP


#include "FirstSpeciesCounterpoint.hpp"
#include "../Utilities.hpp"

class ThirdSpeciesCounterpoint : public FirstSpeciesCounterpoint {
protected :
    IntVarArray thirdSpeciesNotesCp;               /// The notes of the counterpoint that have to follow the rules for the 2nd species
    IntVarArray thirdSpeciesHarmonicIntervals;     /// The harmonic intervals between the notes that have to follow the 2nd species rules and the cantus firmus
    IntVarArray thirdSpeciesMelodicIntervals;      /// The melodic intervals between the notes that have to follow the 2nd species rules
    IntVarArray thirdSpeciesMotions;               /// This array is the array of REAL motions
    IntVarArray thirdSpeciesMotionCosts;
    BoolVarArray is5QNArray;
    BoolVarArray isDiminution;
    IntVarArray cambiataCostArray;
    IntVarArray m2ZeroArray;

public:

    ThirdSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, int mSpec, Stratum* low, CantusFirmus* c, int v_type, vector<int> m_costs
    , vector<int> g_costs, vector<int> s_costs, int bm, int nV);

    ThirdSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type, vector<int> m_costs
    , vector<int> g_costs, vector<int> s_costs, int bm, int nV);

    ThirdSpeciesCounterpoint(Home home, int size, vector<int> cf,int lb, int ub, int k, Stratum* low, CantusFirmus* c, int v_type, vector<int> m_costs
    , vector<int> g_costs, vector<int> s_costs, int bm, int nV1, int nV2);

    string to_string() const override;

    ThirdSpeciesCounterpoint(Home home, ThirdSpeciesCounterpoint &s); // clone constructor

    ThirdSpeciesCounterpoint* clone(Home home) override;

    IntVarArray getFirstHInterval() override;

    IntVarArray getMotions() override;

    IntVarArray getFirstMInterval() override;

    IntVarArray getBranchingNotes() override;

    int getHIntervalSize() override;
};

#endif