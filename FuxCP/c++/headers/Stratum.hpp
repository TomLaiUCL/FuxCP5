#ifndef STRATUM_HPP
#define STRATUM_HPP

#include "Utilities.hpp"
#include "Voice.hpp"

#include "gecode/kernel.hh"
#include "gecode/int.hh"
#include "gecode/search.hh"
#include "gecode/minimodel.hh"
#include "gecode/set.hh"

using namespace Gecode;
using namespace Gecode::Search;
using namespace std;

/// This class represents a Stratum
class Stratum : public Voice{
    protected:
        // intvararray with species of each measure?

    public:
        Stratum(Home home, int nMes, int lb, int ub, int v_type);

        // /**
        //  * These functions are the real constructors of Stratum. The base constructor should not be used. 
        //  * They will create
        // */
        // Stratum* CreateTwoVoiceStratum(Home home, int nMes , int lb, int ub, int index);
        // Stratum* CreateThreeVoiceStratum(Home home, int nMes , int lb, int ub, int index);
        // Stratum* CreateFourVoiceStratum(Home home, int nMes , int lb, int ub, int index);

        Stratum(Stratum& s);  // clone constructor

        // IntVarArray getBranchingNotes() override;

        Stratum* copy() override;

        string to_string() const override;



        void setNote(Home home, int index, IntVar note);
        void setMInterval(int index);   // these two functions will be called from the create strata part of the Problem files. The create strata code is done index by index. 
};


#endif //STRATUM_HPP
