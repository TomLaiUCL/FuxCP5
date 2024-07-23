#include "../headers/constraints.hpp"

/**
 * G6 : no chromatic melodies
 */
void no_chromatic_melodies(Home home, Part part){
    for(int i = 0; i < part.getMelodicIntervals().size()-1; i+=4/notesPerMeasure.at(part.getSpecies())){
        rel(home, expr(home, part.getMelodicIntervals()[i]==1), BOT_AND, expr(home, part.getMelodicIntervals()[i+1]==1), 0);
        rel(home, expr(home, part.getMelodicIntervals()[i]==-1), BOT_AND, expr(home, part.getMelodicIntervals()[i+1]==-1), 0);
    }
}

/**
 * G7 : melodic intervals should be small
 */
void G7_melodic_intervals_should_be_small(Home home, Part part){
    int idx = 0;
    for(int i = 0; i < part.getMelodicIntervals().size(); i+=4/notesPerMeasure.at(part.getSpecies())){
        rel(home, (abs(part.getMelodicIntervals()[i])<MINOR_THIRD) >> (part.getMelodicDegreeCost()[i]==part.getSecondCost()));
        rel(home, (abs(part.getMelodicIntervals()[i])==MINOR_THIRD || abs(part.getMelodicIntervals()[i])==MAJOR_THIRD) >> (part.getMelodicDegreeCost()[i]==part.getThirdCost()));
        rel(home, (abs(part.getMelodicIntervals()[i])==PERFECT_FOURTH) >> (part.getMelodicDegreeCost()[i]==part.getFourthCost()));
        rel(home, (abs(part.getMelodicIntervals()[i])==TRITONE) >> (part.getMelodicDegreeCost()[i]==part.getTritoneCost()));
        rel(home, (abs(part.getMelodicIntervals()[i])==PERFECT_FIFTH) >> (part.getMelodicDegreeCost()[i]==part.getFifthCost()));
        rel(home, (abs(part.getMelodicIntervals()[i])==MINOR_SIXTH || abs(part.getMelodicIntervals()[i])==MAJOR_SIXTH) >> (part.getMelodicDegreeCost()[i]==part.getSixthCost()));
        rel(home, (abs(part.getMelodicIntervals()[i])==MINOR_SEVENTH || abs(part.getMelodicIntervals()[i])==MAJOR_SEVENTH) >> (part.getMelodicDegreeCost()[i]==part.getSeventhCost()));
        rel(home, (abs(part.getMelodicIntervals()[i])==PERFECT_OCTAVE) >> (part.getMelodicDegreeCost()[i]==part.getOctaveCost()));
        idx++;
    }
}