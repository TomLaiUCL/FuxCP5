#include "../headers/constraints.hpp"

void G6_noChromaticMelodies(Home home, Part* part, int mSpec){
    for(int i = 0; i < part->getMelodicIntervals().size()-1; i+=4/notesPerMeasure.at(mSpec)){
        rel(home, expr(home, part->getMelodicIntervals()[i]==1), BOT_AND, expr(home, part->getMelodicIntervals()[i+1]==1), 0);
        rel(home, expr(home, part->getMelodicIntervals()[i]==-1), BOT_AND, expr(home, part->getMelodicIntervals()[i+1]==-1), 0);
    }
}

void G7_melodicIntervalsShouldBeSmall(Home home, Part* part, int mSpec){
    int idx = 0;
    for(int i = 0; i < part->getMelodicIntervals().size(); i+=4/notesPerMeasure.at(mSpec)){
        rel(home, (abs(part->getMelodicIntervals()[i])<MINOR_THIRD) >> (part->getMelodicDegreeCost()[i]==part->getSecondCost()));
        rel(home, (abs(part->getMelodicIntervals()[i])==MINOR_THIRD || abs(part->getMelodicIntervals()[i])==MAJOR_THIRD) >> (part->getMelodicDegreeCost()[i]==part->getThirdCost()));
        rel(home, (abs(part->getMelodicIntervals()[i])==PERFECT_FOURTH) >> (part->getMelodicDegreeCost()[i]==part->getFourthCost()));
        rel(home, (abs(part->getMelodicIntervals()[i])==TRITONE) >> (part->getMelodicDegreeCost()[i]==part->getTritoneCost()));
        rel(home, (abs(part->getMelodicIntervals()[i])==PERFECT_FIFTH) >> (part->getMelodicDegreeCost()[i]==part->getFifthCost()));
        rel(home, (abs(part->getMelodicIntervals()[i])==MINOR_SIXTH || abs(part->getMelodicIntervals()[i])==MAJOR_SIXTH) >> (part->getMelodicDegreeCost()[i]==part->getSixthCost()));
        rel(home, (abs(part->getMelodicIntervals()[i])==MINOR_SEVENTH || abs(part->getMelodicIntervals()[i])==MAJOR_SEVENTH) >> (part->getMelodicDegreeCost()[i]==part->getSeventhCost()));
        rel(home, (abs(part->getMelodicIntervals()[i])==PERFECT_OCTAVE) >> (part->getMelodicDegreeCost()[i]==part->getOctaveCost()));
        idx++;
    }
}

void H1_1_harmonicIntrvalsAreConsonances(Home home, Part* part){
    dom(home, part->getFirstSpeciesHIntervals(), IntSet(IntArgs(CONSONANCES)));
}

void H4_1_keyToneIsTuned(Home home, Part* part){
    rel(home, (part->getIsLowest()[0]==0) >> (part->getFirstHInterval()[0]==0));
    rel(home, (part->getIsLowest()[part->getIsLowest().size()-1]==0) >> (part->getFirstHInterval()[part->getFirstHInterval().size()-1]==0));
}

void H5_1_cpAndCfDifferentNotes(Home home, Part* part, Part* cf){
    for(int i = 1; i < part->getFirstSpeciesNotes().size()-1; i++){
        rel(home, part->getFirstSpeciesNotes()[i], IRT_NQ, cf->getNotes()[i]);
    }
}

void H6_1_preferImperfectConsonances(Home home, Part* part){
    for(int i = 0; i < part->getHInterval().size(); i++){
        rel(home, part->getOctaveCostArray()[i], IRT_EQ, part->getHOctaveCost(), Reify(expr(home, part->getHInterval()[i]==UNISSON), RM_PMI));
        rel(home, part->getOctaveCostArray()[i], IRT_EQ, 0, Reify(expr(home, part->getHInterval()[i]!=UNISSON), RM_PMI));

        rel(home, part->getFifthCostArray()[i], IRT_EQ, part->getHFifthCost(), Reify(expr(home, part->getHInterval()[i]==PERFECT_FIFTH), RM_PMI));
        rel(home, part->getFifthCostArray()[i], IRT_EQ, 0, Reify(expr(home, part->getHInterval()[i]!=PERFECT_FIFTH), RM_PMI));
    }
}

void H7_1_2v_penultimateSixthOrThird(Home home, Part* part){
    int p = part->getFirstSpeciesNotes().size()-2;
    rel(home, part->getFirstSpeciesHIntervals()[part->getFirstSpeciesHIntervals().size()-2], IRT_EQ, MAJOR_SIXTH, 
        Reify(part->getIsLowest()[part->getIsLowest().size()-2], RM_IMP));
}

void M2_1_melodicIntervalsNotExceedMinorSixth(Home home, Part* part){
    rel(home, part->getFirstSpeciesMIntervals(), IRT_LQ, MINOR_SIXTH);
    rel(home, part->getFirstSpeciesMIntervals(), IRT_GQ, -MINOR_SIXTH);
}

void P1_1_2v_noDirectMotionFromPerfectConsonance(Home home, Part* part){
    for(int j = 0; j < part->getFirstSpeciesMotions().size(); j++){

        rel(home, ((part->getFirstSpeciesHIntervals()[j+1]==UNISSON || part->getFirstSpeciesHIntervals()[j+1]==PERFECT_FIFTH)&&part->getIsLowest()[j+1]==1) >>
            (part->getFirstSpeciesMotions()[j]!=PARALLEL_MOTION));

    }
}

void P3_1_noBattuta(Home home, Part* part){
    for(int j = 0; j < part->getFirstSpeciesMotions().size(); j++){
        rel(home, expr(home, part->getFirstSpeciesMotions()[j]==CONTRARY_MOTION && part->getFirstSpeciesHIntervals()[j+1]==0 && 
            part->getFirstSpeciesMIntervals()[j]<-4), BOT_AND, part->getIsLowest()[j], 0);
    }
}