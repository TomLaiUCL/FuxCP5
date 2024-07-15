//
// Created by Damien Sprockeels on 12/06/2024.
//

#ifndef MYPROJECT_CANTUSFIRMUS_HPP
#define MYPROJECT_CANTUSFIRMUS_HPP

#include "../Utilities.hpp"
#include "Part.hpp"
class CantusFirmus : public Part {
    protected:
        IntVarArray active_notes;
        vector<int> cf_vector;
        //IntVarArray melodicIntervals; not used for now
    public:
        CantusFirmus(Home home, int size, vector<int> cf, int k, Stratum* low, int v_type, vector<int> m_costs, vector<int> g_costs, int nV);

        // CantusFirmus(CantusFirmus &s);

        // IntVarArray getBranchingNotes() override;

        // Space *copy() override;


        string to_string() const override;

        CantusFirmus(Home home, CantusFirmus& s); // clone constructor
        CantusFirmus* clone(Home home) override;

        IntVarArray getFirstHInterval() override;

        IntVarArray getFirstMInterval() override;

        IntVarArray getMotions() override;

};


#endif //MYPROJECT_CANTUSFIRMUS_HPP
