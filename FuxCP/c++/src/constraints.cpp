#include "../headers/constraints.hpp"

void initializeIsOffArray(Home home, Part* part){
    for(int i = 0; i < part->getIsOffArray().size(); i++){
        IntVarArray res = IntVarArray(home, part->getOffDomain().size(), 0, 1);
        IntVar sm = IntVar(home, 0, part->getOffDomain().size());
        for(int l = 0; l < part->getOffDomain().size(); l++){      // TODO il y a d'office une meilleure manière de faire que double boucle for
            BoolVar b1 = BoolVar(home, 0, 1);
            rel(home, part->getNotes()[i], IRT_EQ, part->getOffDomain()[l], Reify(b1));   // REIFY RM_PMI?
            ite(home, b1, IntVar(home, 1, 1), IntVar(home, 0, 0), res[l]);
        }
        IntVarArgs x(res.size());
        for(int t = 0; t < part->getOffDomain().size(); t++){
            x[t] = res[t];
        }
        rel(home, sm, IRT_EQ, expr(home, sum(x)));
        rel(home, sm, IRT_GR, 0, Reify(part->getIsOffArray()[i]));  // REIFY RM_PMI?*/
    }
}

void G4_counterpointMustBeInTheSameKey(Home home, Part* part){
    for(int i = 0; i < part->getIsOffArray().size(); i++){
        rel(home, (part->getIsOffArray()[i]==0) >> (part->getOffCostArray()[i]==0));
        rel(home, (part->getIsOffArray()[i]==1) >> (part->getOffCostArray()[i]==part->getBorrowCost()));
    }
}

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

void G9_lastChordSameAsFundamental(Home home, Stratum* lowest, Part* cantusFirmus){
    rel(home, expr(home, lowest->getNotes()[lowest->getNotes().size()-1]%12), IRT_EQ, expr(home, cantusFirmus->getNotes()[cantusFirmus->getNotes().size()-1]%12));
}   

void H1_1_harmonicIntrvalsAreConsonances(Home home, Part* part){
    dom(home, part->getFirstSpeciesHIntervals(), IntSet(IntArgs(CONSONANCES)));
}

void H2_1_startWithPerfectConsonance(Home home, Part* part){
    dom(home, part->getHInterval()[0], IntSet(IntArgs(PERFECT_CONSONANCES)));
}

void H2_2_arsisHarmoniesCannotBeDisonnant(Home home, Part* part){
    for(int i = 0; i < part->getNMeasures()-1; i++){
        rel(home, part->getConsonance()[(i*2)+1], IRT_EQ, 0, Reify(part->getIsDiminution()[i]));
    }
}

void H3_1_endWithPerfectConsonance(Home home, Part* part){
    dom(home, part->getHInterval()[part->getHInterval().size()-1], IntSet(IntArgs(PERFECT_CONSONANCES)));
}

void H3_2_penultimateNoteDomain(Home home, Part* part){
    //dom(home, part->getHInterval()[part->getHInterval().size()-3], IntSet({UNISSON, PERFECT_FIFTH, MINOR_SIXTH, MAJOR_SIXTH}));

    rel(home, (part->getHInterval()[part->getHInterval().size()-3]!=PERFECT_FIFTH) >> (part->getPenultCostArray()[0]==part->getPenultCost()));
    rel(home, (part->getHInterval()[part->getHInterval().size()-3]==PERFECT_FIFTH) >> (part->getPenultCostArray()[0]==0));
}

void H4_1_keyToneIsTuned(Home home, Part* part){
    rel(home, (part->getIsNotLowest()[0]==0) >> (part->getFirstHInterval()[0]==0));
    rel(home, (part->getIsNotLowest()[part->getIsNotLowest().size()-1]==0) >> (part->getFirstHInterval()[part->getFirstHInterval().size()-1]==0));
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
        Reify(part->getIsNotLowest()[part->getIsNotLowest().size()-2], RM_IMP));
}

void H8_3v_preferHarmonicTriad(Home home, Part* part, IntVarArray triadCostArray, Stratum* upper1, Stratum* upper2){
    for(int i = 0; i < triadCostArray.size(); i++){
        BoolVar h1_3 = BoolVar(home, 0 ,1);
        BoolVar h1_4 = BoolVar(home, 0 ,1);
        BoolVar h1_third = BoolVar(home, 0 ,1);
        BoolVar h1_7 = BoolVar(home, 0 ,1);

        BoolVar h2_3 = BoolVar(home, 0 ,1);
        BoolVar h2_4 = BoolVar(home, 0 ,1);
        BoolVar h2_third = BoolVar(home, 0 ,1);
        BoolVar h2_7 = BoolVar(home, 0 ,1);

        BoolVar h_firstPoss = BoolVar(home, 0, 1);
        BoolVar h_secondPoss = BoolVar(home, 0, 1);
        BoolVar triad = BoolVar(home, 0, 1);
        BoolVar not_triad = BoolVar(home, 0, 1);

        rel(home, upper1->getHInterval()[i*4], IRT_EQ, 3, Reify(h1_3));
        rel(home, upper1->getHInterval()[i*4], IRT_EQ, 4, Reify(h1_4));
        rel(home, upper2->getHInterval()[i*4], IRT_EQ, 7, Reify(h2_7));
        rel(home, h1_3, BOT_OR, h1_4, h1_third);
        rel(home, h1_third, BOT_AND, h2_7, h_firstPoss);

        rel(home, upper2->getHInterval()[i*4], IRT_EQ, 3, Reify(h2_3));
        rel(home, upper2->getHInterval()[i*4], IRT_EQ, 4, Reify(h2_4));
        rel(home, upper1->getHInterval()[i*4], IRT_EQ, 7, Reify(h1_7));
        rel(home, h2_3, BOT_OR, h2_4, h2_third);
        rel(home, h2_third, BOT_AND, h1_7, h_secondPoss);

        rel(home, h_firstPoss, BOT_OR, h_secondPoss, triad);
        rel(home, triad, BOT_XOR, not_triad, 1);
        rel(home, triadCostArray[i], IRT_EQ, 0, Reify(triad, RM_IMP));
        rel(home, triadCostArray[i], IRT_EQ, part->getTriadCost(), Reify(not_triad, RM_IMP));
    }
}

void H8_4v_preferHarmonicTriad(Home home, IntVarArray triadCostArray, Stratum* upper1, Stratum* upper2, Stratum* upper3){
    for(int i = 0; i < triadCostArray.size(); i++){

        IntVar H_b = upper1->getHInterval()[i*4];
        IntVar H_c = upper2->getHInterval()[i*4];
        IntVar H_d = upper3->getHInterval()[i*4];


        BoolVar H_b_is_third    = expr(home, H_b==MINOR_THIRD || H_b==MAJOR_THIRD);
        BoolVar H_b_is_fifth    = expr(home, H_b==PERFECT_FIFTH);
        BoolVar H_b_is_octave   = expr(home, H_b==UNISSON);

        BoolVar H_c_is_third    = expr(home, H_c==MINOR_THIRD || H_c==MAJOR_THIRD);
        BoolVar H_c_is_fifth    = expr(home, H_c==PERFECT_FIFTH);
        BoolVar H_c_is_octave   = expr(home, H_c==UNISSON);

        BoolVar H_d_is_third    = expr(home, H_d==MINOR_THIRD || H_d==MAJOR_THIRD);
        BoolVar H_d_is_fifth    = expr(home, H_d==PERFECT_FIFTH);
        BoolVar H_d_is_octave   = expr(home, H_d==UNISSON);


        // worst case : not a harmonic triad with at least a third and a fifth
        BoolVar no_fifth_or_no_third = expr(home, H_b_is_third + H_c_is_third + H_d_is_third == 0 || H_b_is_fifth + H_c_is_fifth + H_d_is_fifth == 0);
        BoolVar note_outside_harmonic_triad = expr(home, H_b_is_third + H_b_is_fifth + H_b_is_octave == 0 || H_c_is_third + H_c_is_fifth + H_c_is_octave == 0 || H_d_is_third + H_d_is_fifth + H_d_is_octave == 0);

        rel(home, (note_outside_harmonic_triad || no_fifth_or_no_third) >> (triadCostArray[i] == not_harmonic_triad_cost));

        // now we are left with only combinations with at a third, a fifth, and another note of the harmonic triad. 
        // Doubling the fifth
        rel(home, expr(home, H_b_is_fifth + H_c_is_fifth + H_d_is_fifth == 2) >> (triadCostArray[i] == double_fifths_cost));

        // Doubling the third, and ensure it is the same type of third (not a major and a minor)
        rel(home, expr(home, H_b_is_third + H_c_is_third + H_d_is_third == 2) >> (triadCostArray[i] == double_thirds_cost));
        rel(home, (H_b_is_third && H_c_is_third) >> (H_b == H_c));
        rel(home, (H_b_is_third && H_d_is_third) >> (H_b == H_d));
        rel(home, (H_c_is_third && H_d_is_third) >> (H_c == H_d));

        // triad with octave
        rel(home, (H_b_is_fifth && H_c_is_third && H_d_is_octave) >> (triadCostArray[i] == triad_with_octave_cost)); // 5 3 8
        rel(home, (H_b_is_third && H_c_is_octave && H_d_is_fifth) >> (triadCostArray[i] == triad_with_octave_cost)); // 3 8 5
        rel(home, (H_b_is_third && H_c_is_fifth && H_d_is_octave) >> (triadCostArray[i] == triad_with_octave_cost)); // 3 5 8
        rel(home, (H_b_is_octave && H_c_is_third && H_d_is_fifth) >> (triadCostArray[i] == triad_with_octave_cost)); // 8 3 5
        rel(home, (H_b_is_octave && H_c_is_fifth && H_d_is_third) >> (triadCostArray[i] == triad_with_octave_cost)); // 8 5 3

        // Best case : 5 8 3
        rel(home, (H_b_is_fifth && H_c_is_octave && H_d_is_third) >> (triadCostArray[i] == 0)); 
    
    }
}

void M2_1_2v_melodicIntervalsNotExceedMinorSixth(Home home, Part* part){
    rel(home, part->getFirstSpeciesMIntervals(), IRT_LQ, MINOR_SIXTH);
    rel(home, part->getFirstSpeciesMIntervals(), IRT_GQ, -MINOR_SIXTH);
}

void M2_2_2v_twoConsecutiveNotesAreNotTheSame(Home home, Part* part){
    rel(home, part->getSecondSpeciesMIntervals(), IRT_NQ, 0); /// plus efficace que de faire cp[i] /= cp[i+1]
}

void M2_2_3v_melodicIntervalsNotExceedMinorSixth(Home home, vector<Part*> parts, bool containsThirdSpecies){
    for(int i = 1; i < parts.size(); i++){
        if(parts[i]->getSpecies()==THIRD_SPECIES){
            containsThirdSpecies=1;
            break;
        }
    }
    for(int p1 = 1; p1 < parts.size(); p1++){
        for(int p2 = 1; p2 < parts.size(); p2++){
            if(p1!=p2 && parts[p1]->getSpecies()==SECOND_SPECIES){
                if(!containsThirdSpecies){
                    for(int i = 0; i < parts[p1]->getBranchingNotes().size()-4; i++){
                        rel(home, parts[p1]->getMelodicIntervals().slice(0, notesPerMeasure.at(SECOND_SPECIES), parts[p1]->getMelodicIntervals().size())[i], 
                            IRT_NQ, 0);
                    }
                } else {
                    for(int i = 0; i < parts[p1]->getBranchingNotes().size()-4; i++){
                        rel(home, parts[p1]->getMelodicIntervals().slice(0, notesPerMeasure.at(SECOND_SPECIES), parts[p1]->getMelodicIntervals().size())[i], 
                            IRT_NQ, 0);
                    }
                    //no unison in the two last notes
                    rel(home, parts[p1]->getBranchingNotes()[parts[p1]->getBranchingNotes().size()-2], IRT_NQ, parts[p1]->getBranchingNotes()[parts[p1]->getBranchingNotes().size()-1]);
                }
            }
        }
    }
}

void M4_varietyCost(Home home, vector<Part*> parts){
    for(int i = 1; i < parts.size(); i++){
        Part* p = parts[i];
        int temp = 0;
        IntVarArray notes = p->getBranchingNotes();
        for(int j = 0; j < p->getHIntervalSize()-1; j++){
            int upbnd = 0;
            if(j+3<p->getHIntervalSize()){
                upbnd = j+4;
            } else {upbnd = p->getHIntervalSize();}
            for(int k = j+1; k < upbnd;k++){
                //setting a cost if notes inside a window are the same in a part
                rel(home, (notes[j]==notes[k])>>(p->getVarietyArray(temp)==p->getVarietyCost()));
                rel(home, (notes[j]!=notes[k])>>(p->getVarietyArray(temp)==0));
                temp++;
            }
            
        }
    }
}

void P1_1_2v_noDirectMotionFromPerfectConsonance(Home home, Part* part){
    for(int j = 0; j < part->getFirstSpeciesMotions().size(); j++){

        rel(home, ((part->getFirstSpeciesHIntervals()[j+1]==UNISSON || part->getFirstSpeciesHIntervals()[j+1]==PERFECT_FIFTH)&&part->getIsNotLowest()[j+1]==1) >>
            (part->getFirstSpeciesMotions()[j]!=PARALLEL_MOTION));

    }
}

void P1_2_2v_noDirectMotionFromPerfectConsonance(Home home, Part* part){
    for(int j = 0; j < part->getSecondSpeciesRealMotions().size(); j++){
        rel(home, ((part->getFirstSpeciesHIntervals()[j+1]==UNISSON || part->getFirstSpeciesHIntervals()[j+1]==PERFECT_FIFTH)&&part->getIsNotLowest()[j+1]==1) >>
            (part->getSecondSpeciesRealMotions()[j]!=PARALLEL_MOTION));
    }
}

void P1_2_3v_noDirectMotionFromPerfectConsonance(Home home, Part* part){
    for(int j = 0; j < part->getFirstSpeciesMotions().size()-1; j++){
        //set a cost when it is reached through direct motion, it is 0 when not
        rel(home, (part->getSecondSpeciesRealMotions()[j]==2&&(part->getFirstSpeciesHIntervals()[j+1]==0||part->getFirstSpeciesHIntervals()[j+1]==7))>>
            (part->getDirectCostArray()[j]==part->getDirectMoveCost()));
        rel(home, (part->getSecondSpeciesRealMotions()[j]!=2||(part->getFirstSpeciesHIntervals()[j+1]!=0&&part->getFirstSpeciesHIntervals()[j+1]!=7))>>
            (part->getDirectCostArray()[j]==0));
    }
}

void P3_1_noBattuta(Home home, Part* part){
    for(int j = 0; j < part->getFirstSpeciesMotions().size(); j++){
        rel(home, expr(home, part->getFirstSpeciesMotions()[j]==CONTRARY_MOTION && part->getFirstSpeciesHIntervals()[j+1]==0 && 
            part->getFirstSpeciesMIntervals()[j]<-4), BOT_AND, part->getIsNotLowest()[j], 0);
    }
}

void P3_2_noBattuta(Home home, Part* part){
    for(int j = 0; j < part->getSecondSpeciesMotions().size(); j++){
        rel(home, expr(home, part->getSecondSpeciesMotions()[j]==CONTRARY_MOTION && part->getFirstSpeciesHIntervals()[j+1]==0 && 
            part->getFirstSpeciesMIntervals()[j]<-4), BOT_AND, part->getIsNotLowest()[j], 0);
    }
}

void P4_successiveCost(Home home, vector<Part*> parts, int scc_cz, IntVarArray successiveCostArray, vector<Species> species){
    int idx = 0;
   
    // cout << "SUCC SIZE :" + to_string(succ_cost.size()) << endl;
    for(int p1 = 1; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            if(species[p1-1]!=SECOND_SPECIES && species[p2-1]!=SECOND_SPECIES){
                
                for(int i = 0; i < parts[p1]->getFirstHInterval().size()-1; i++){
                    // cout << "IDX : " + to_string(idx) << endl;
                    rel(home, successiveCostArray[idx], IRT_EQ, parts[p1]->getSuccCost(), 
                        Reify(expr(home, (parts[p1]->getFirstHInterval()[i]==0 || parts[p1]->getFirstHInterval()[i]==7) &&
                            (parts[p2]->getFirstHInterval()[i]==0 || parts[p2]->getFirstHInterval()[i]==7))));
                    idx++;
                }        
            }
            else if(species[p1-1]==SECOND_SPECIES){
                
                for(int i = 0; i < parts[p1]->getFirstHInterval().size()-1; i++){
                    BoolVar case1 = expr(home, ((parts[p1]->getFirstHInterval()[i]==UNISSON || parts[p1]->getFirstHInterval()[i]==PERFECT_FIFTH) && 
                    (parts[p2]->getFirstHInterval()[i]==UNISSON || parts[p2]->getFirstHInterval()[i]==PERFECT_FIFTH)) && 
                        ((parts[p1]->getFirstHInterval()[i+1]==UNISSON || parts[p1]->getFirstHInterval()[i+1]==PERFECT_FIFTH) && 
                    (parts[p2]->getFirstHInterval()[i+1]==UNISSON || parts[p2]->getFirstHInterval()[i+1]==PERFECT_FIFTH)));
                    BoolVar case2 = expr(home, (parts[p1]->getFirstHInterval()[i]!=PERFECT_FIFTH || parts[p2]->getFirstHInterval()[i]!=PERFECT_FIFTH) || 
                        (parts[p1]->getFirstHInterval()[i+1]!=PERFECT_FIFTH || parts[p2]->getFirstHInterval()[i+1]!=PERFECT_FIFTH) || 
                        ((parts[p1]->getSecondHInterval()[i]==3 || parts[p2]->getSecondHInterval()[i]==3) && 
                        (parts[p1]->getSecondHInterval()[i]==4 || parts[p2]->getSecondHInterval()[i]==4)));
                    //first expression states that the melodic succ interval is not a third, second that we have successive fifths
                    rel(home, successiveCostArray[idx], IRT_EQ, parts[p1]->getSuccCost(), Reify(expr(home, (case1==1 || case2==1)), RM_IMP));
                    idx++;
                }
            }
            else if(species[p2-1]==SECOND_SPECIES){
                for(int i = 0; i < parts[p1]->getFirstHInterval().size()-1; i++){
                    BoolVar case1 = expr(home, ((parts[p1]->getFirstHInterval()[i]==UNISSON || parts[p1]->getFirstHInterval()[i]==PERFECT_FIFTH) && 
                    (parts[p2]->getFirstHInterval()[i]==UNISSON || parts[p2]->getFirstHInterval()[i]==PERFECT_FIFTH)) && 
                        ((parts[p1]->getFirstHInterval()[i+1]==UNISSON || parts[p1]->getFirstHInterval()[i+1]==PERFECT_FIFTH) && 
                    (parts[p2]->getFirstHInterval()[i+1]==UNISSON || parts[p2]->getFirstHInterval()[i+1]==PERFECT_FIFTH)));
                    BoolVar case2 = expr(home, (parts[p1]->getFirstHInterval()[i]!=PERFECT_FIFTH || parts[p2]->getFirstHInterval()[i]!=PERFECT_FIFTH) || 
                        (parts[p1]->getFirstHInterval()[i+1]!=PERFECT_FIFTH || parts[p2]->getFirstHInterval()[i+1]!=PERFECT_FIFTH) || 
                        ((parts[p1]->getSecondHInterval()[i]==3 || parts[p2]->getSecondHInterval()[i]==3) && 
                        (parts[p1]->getSecondHInterval()[i]==4 || parts[p2]->getSecondHInterval()[i]==4)));
                    //first expression states that the melodic succ interval is not a third, second that we have successive fifths
                    rel(home, successiveCostArray[idx], IRT_EQ, parts[p1]->getSuccCost(), Reify(expr(home, (case1==1 || case2==1)), RM_IMP));
                    idx++;
                }
            }
        }
    }
}

void P6_noMoveInSameDirection(Home home, vector<Part*> parts){
    for(int i = 0; i < parts[0]->getMotions().size(); i++){
        rel(home, expr(home, parts[0]->getMotions()[i]==2 && parts[1]->getMotions()[i]==2), BOT_AND, expr(home, parts[2]->getMotions()[i]==2), 0);
    }
}

void P7_noSuccessiveAscendingSixths(Home home, vector<Part*> parts){
    for(int p1 = 0; p1 < parts.size(); p1++){
        for(int p2 = p1+1; p2 < parts.size(); p2++){
            for(int j = 1; j < parts[p1]->getFirstHInterval().size()-1; j++){
                rel(home, ((parts[p1]->getFirstHInterval()[j-1]!=MINOR_SIXTH && parts[p1]->getFirstHInterval()[j-1]!=MAJOR_SIXTH) && 
                    (parts[p2]->getFirstHInterval()[j-1]!=MINOR_SIXTH && parts[p2]->getFirstHInterval()[j-1]!=MAJOR_SIXTH)) || (
                        (parts[p1]->getFirstHInterval()[j]!=-MINOR_SIXTH && parts[p1]->getFirstHInterval()[j]!=-MAJOR_SIXTH) && 
                    (parts[p2]->getFirstHInterval()[j]!=- MINOR_SIXTH && parts[p2]->getFirstHInterval()[j]!=-MAJOR_SIXTH)) || (
                        parts[p1]->getFirstMInterval()[j]>0 || parts[p2]->getFirstMInterval()[j] > 0));
            }
        }
    }
}

void M2_1_3v_melodicIntervalsNotExceedMinorSixth(Home home, Part* part){
    dom(home, part->getMelodicIntervals(), IntSet({-UNISSON, -MINOR_SECOND, -MAJOR_SECOND, -MINOR_THIRD, -MAJOR_THIRD, -PERFECT_FOURTH, -TRITONE,
            -PERFECT_FIFTH, -MINOR_SIXTH, -PERFECT_OCTAVE, UNISSON, MINOR_SECOND, MAJOR_SECOND, MINOR_THIRD, MAJOR_THIRD, PERFECT_FOURTH, TRITONE,
            PERFECT_FIFTH, MINOR_SIXTH, PERFECT_OCTAVE}));
}

void P1_1_3v_noDirectMotionFromPerfectConsonance(Home home, Part* part){
    for(int j = 0; j < part->getFirstSpeciesMotions().size()-1; j++){
        //set a cost when it is reached through direct motion, it is 0 when not
        rel(home, (part->getFirstSpeciesMotions()[j]==2&&(part->getFirstSpeciesHIntervals()[j+1]==0||part->getFirstSpeciesHIntervals()[j+1]==7))>>
            (part->getDirectCostArray()[j]==part->getDirectCost()));
        rel(home, (part->getFirstSpeciesMotions()[j]!=2||(part->getFirstSpeciesHIntervals()[j+1]!=0&&part->getFirstSpeciesHIntervals()[j+1]!=7))>>
            (part->getDirectCostArray()[j]==0));
    }
}

void H7_1_4v_penultimateSixthOrThird(Home home, Part* part){
    dom(home, part->getFirstSpeciesHIntervals()[part->getFirstSpeciesHIntervals().size()-2], IntSet({UNISSON, MINOR_THIRD, PERFECT_FIFTH, MAJOR_SIXTH}));
}