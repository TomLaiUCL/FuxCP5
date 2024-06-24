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
        CantusFirmus(Home home, int size, vector<int> cf, int k);

        // CantusFirmus(CantusFirmus &s);

        IntVarArray getNotes() { return notes; }

        // IntVarArray getBranchingNotes() override;

        // Space *copy() override;


        string to_string() const override;

        CantusFirmus(Home home, CantusFirmus& s); // clone constructor
        CantusFirmus* clone(Home home) override;
};


#endif //MYPROJECT_CANTUSFIRMUS_HPP
