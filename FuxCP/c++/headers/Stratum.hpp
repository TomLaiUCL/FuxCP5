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
        Voice* cantus;
        Voice* cp1;
        Voice* cp2;
        Stratum* upper1;
        Stratum* upper2;
    public:
        Stratum(Home home, int nMes, int lb, int ub, int v_type);

        Stratum(Home home, int nMes, int lb, int ub, int v_type, IntVarArray lowestNotes);

        Stratum(Home home, int nMes, int lb, int ub, int v_type, IntVarArray lowestNotes, int nV);

        // /**
        //  * These functions are the real constructors of Stratum. The base constructor should not be used. 
        //  * They will create
        // */
        // Stratum* CreateTwoVoiceStratum(Home home, int nMes , int lb, int ub, int index);
        // Stratum* CreateThreeVoiceStratum(Home home, int nMes , int lb, int ub, int index);
        // Stratum* CreateFourVoiceStratum(Home home, int nMes , int lb, int ub, int index);

        Stratum(Home home, Stratum& s);  // clone constructor

        // IntVarArray getBranchingNotes() override;

        Stratum* clone(Home home) override;

        string to_string() const override;

        void setNote(Home home, int index, IntVar note);
        void setMInterval(int index);   // these two functions will be called from the create strata part of the Problem files. The create strata code is done index by index.
        void setCantusPointer(Voice* cf);
        void setCpPointer(Home home, Voice* counter1, Voice* counter2=nullptr);

        void setLowest(Home home, Stratum* up1, Stratum* up2=nullptr);

        void setHIntervals(Home home);
};


#endif //STRATUM_HPP
