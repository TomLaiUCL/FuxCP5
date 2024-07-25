#include "../headers/Stratum.hpp"

Stratum::Stratum(Home home, int nMes, int lb, int ub, int v_type) : Voice(home, nMes, lb, ub, v_type, FIRST_SPECIES){
    // add here any variable initialization that can't be done in Voice constructor and that is independent of the number of voices
    
}

//if the stratum is in the upper strata, this constructor will create the h_intervals to the lowest stratum
Stratum::Stratum(Home home, int nMes, int lb, int ub, int v_type, IntVarArray lowestNotes) : Stratum(home, nMes, lb, ub, v_type){
    for(int i = 0; i < h_intervals.size(); i++){
        rel(home, h_intervals[i]==((notes[i]-lowestNotes[i])%12));
    }
}

//this constructor applies 3rd voice specific constraints which apply to the upper strata
Stratum::Stratum(Home home, int nMes, int lb, int ub, int v_type, IntVarArray lowestNotes, int nV) : Stratum(home, nMes, lb, ub, v_type, lowestNotes){

    //G8 Last chord can only consist of notes of the harmonic triad
    dom(home, h_intervals[h_intervals.size()-1], IntSet(IntArgs(TRIAD)));

    //H10 No tenths in last chord
    rel(home, ((notes[notes.size()-4]-lowestNotes[lowestNotes.size()-1])>12) >> (h_intervals[h_intervals.size()-1]!=MINOR_THIRD && 
        h_intervals[h_intervals.size()-4]!=MAJOR_THIRD));

    //H12 Last chord cannot include a minor third
    rel(home, h_intervals[h_intervals.size()-1], IRT_NQ, 3);

}

Stratum::Stratum(Home home, int nMes, int lb, int ub, int v_type, IntVarArray lowestNotes, int nV1, int nV2) : Stratum(home, nMes, lb, ub, v_type, lowestNotes){
    //G8 Last chord can only consist of notes of the harmonic triad
    dom(home, h_intervals[h_intervals.size()-1], IntSet(IntArgs(TRIAD)));

    //H12 Last chord cannot include a minor third
    rel(home, h_intervals[h_intervals.size()-1], IRT_NQ, 3);
}


string Stratum::to_string() const {
    string text = "Notes : ";
    text += intVarArray_to_string(notes);
    text += "\n";
    text += "M intervals : ";
    text += intVarArray_to_string(m_intervals_brut);
    text += "\n";
    return text;
}


// clone constructor
Stratum::Stratum(Home home,  Stratum &s) : Voice(home, s){
    // update/clone here all variables that are not in voice
    //rel(home, notes[1], IRT_EQ, 64);
    cantus = s.cantus;
    cp1 = s.cp1;
    cp2 = s.cp2;
    cp3 = s.cp3;
    upper1 = s.upper1;
    upper2 = s.upper2;
    upper3 = s.upper3;
}

Stratum* Stratum::clone(Home home){
    return new Stratum(home, *this);
}

void Stratum::setNote(Home home, int index, IntVar note){
    rel(home, notes[index*4], IRT_EQ, note);
}

void Stratum::setCantusPointer(Voice* cf){
    cantus = cf;
}


void Stratum::setCpPointer(Home home, Voice* counter1, Voice* counter2, Voice* counter3){
    cp1 = counter1;
    cp2 = counter2;
    cp3 = counter3;
}

void Stratum::setLowest(Home home, Stratum* up1, Stratum* up2, Stratum* up3){
    upper1 = up1;
    upper2 = up2;
    upper3 = up3;
    int nVoices = 2;
    if(upper2!=nullptr){
        nVoices ++;
    }
    if(upper3!=nullptr){
        nVoices++;
    }
    cout << nVoices << endl;
    int size = cp1->getNMeasures();
    vector<IntVarArray> sorted_voices = {};
    for(int i = 0; i < size; i++){
        IntVarArray voices = IntVarArray(home, nVoices, 0, 127);
        rel(home, voices[0], IRT_EQ, cantus->getNotes()[i]);
        rel(home, voices[1], IRT_EQ, cp1->getFirstNotes()[i]);
        if(nVoices>=3){
            rel(home, voices[2], IRT_EQ, cp2->getFirstNotes()[i]);
        }
        if(nVoices>=4){
            rel(home, voices[3], IRT_EQ, cp3->getFirstNotes()[i]);
        }
        IntVarArray order = IntVarArray(home, nVoices, 0, nVoices-1);
        sorted_voices.push_back(IntVarArray(home, nVoices, 0, 127));

        sorted(home, voices, sorted_voices[i], order);

        this->setNote(home, i, sorted_voices[i][0]);

        upper1->setNote(home, i, sorted_voices[i][1]);
        if(nVoices>=3){
            upper2->setNote(home, i, sorted_voices[i][2]);
        }
        if(nVoices>=4){
            upper3->setNote(home, i, sorted_voices[i][3]);
        }

        //rel(home, this->getFirstNotes()[i], IRT_NQ, cantus->getNotes()[i], Reify(cantus->getIsLowestIdx(i), RM_IMP));
        rel(home, (this->getFirstNotes()[i]==cantus->getNotes()[i]) >> (cantus->getIsLowestIdx(i)==0));
        rel(home, (this->getFirstNotes()[i]!=cantus->getNotes()[i]) >> (cantus->getIsLowestIdx(i)==1));

        if(nVoices==2){
            rel(home, cantus->getIsLowestIdx(i), IRT_EQ, 0, Reify(cp1->getIsLowestIdx(i)));
        } else if(nVoices==3){
            //rel(home, cantus->getIsLowestIdx(i), IRT_EQ, 0, Reify(cp1->getIsLowestIdx(i)));
            rel(home, ((cantus->getIsLowestIdx(i)==1)&&(this->getFirstNotes()[i]==cp1->getFirstNotes()[i])) >> (cp1->getIsLowestIdx(i)==0));
            rel(home, ((cantus->getIsLowestIdx(i)==0)||(this->getFirstNotes()[i]!=cp1->getFirstNotes()[i])) >> (cp1->getIsLowestIdx(i)==1));

            rel(home, ((cantus->getIsLowestIdx(i)==1)&&(cp1->getIsLowestIdx(i)==1)) >> (cp2->getIsLowestIdx(i)==0));
            rel(home, ((cantus->getIsLowestIdx(i)==0)||(cp1->getIsLowestIdx(i)==0)) >> (cp2->getIsLowestIdx(i)==1));
        } else {

            rel(home, ((cantus->getIsLowestIdx(i)==1)&&(this->getFirstNotes()[i]==cp1->getFirstNotes()[i])) >> (cp1->getIsLowestIdx(i)==0));
            rel(home, ((cantus->getIsLowestIdx(i)==0)||(this->getFirstNotes()[i]!=cp1->getFirstNotes()[i])) >> (cp1->getIsLowestIdx(i)==1));

            rel(home, ((cantus->getIsLowestIdx(i)==1)&&(cp1->getIsLowestIdx(i)==1)&&(this->getFirstNotes()[i]==cp2->getFirstNotes()[i])) >> (cp2->getIsLowestIdx(i)==0));
            rel(home, ((cantus->getIsLowestIdx(i)==0)||(cp1->getIsLowestIdx(i)==0)||(this->getFirstNotes()[i]!=cp2->getFirstNotes()[i])) >> (cp2->getIsLowestIdx(i)==1));

            rel(home, ((cantus->getIsLowestIdx(i)==1)&&(cp1->getIsLowestIdx(i)==1)&&(cp2->getIsLowestIdx(i)==1)) >> (cp3->getIsLowestIdx(i)==0));
            rel(home, ((cantus->getIsLowestIdx(i)==0)||(cp1->getIsLowestIdx(i)==0)||(cp2->getIsLowestIdx(i)==0)) >> (cp3->getIsLowestIdx(i)==1));
        }

        if(i > 0){

            vector<IntVarArray> corresponding_m_intervals;
            BoolVar cf_is_lowest = BoolVar(home, 0, 1);
            BoolVar cp1_is_lowest = BoolVar(home, 0, 1);
            BoolVar cp2_is_lowest = BoolVar(home, 0, 1);
            BoolVar cp3_is_lowest = BoolVar(home, 0, 1);

            rel(home, cf_is_lowest, BOT_XOR, cantus->getIsLowestIdx(i), 1);
            rel(home, cp1_is_lowest, BOT_XOR, cp1->getIsLowestIdx(i), 1);
            if(nVoices>=3){
                rel(home, cp2_is_lowest, BOT_XOR, cp2->getIsLowestIdx(i), 1);
            }
            if(nVoices>=4){
                rel(home, cp3_is_lowest, BOT_XOR, cp3->getIsLowestIdx(i), 1);
            }

            corresponding_m_intervals.push_back(cantus->getMelodicIntervals());
            for(int j = 0; j < nVoices-1; j++){
                Voice* curr_cp;

                if(j==0) curr_cp=cp1;
                else curr_cp=cp2;

                if(curr_cp->getSpecies()==FIRST_SPECIES){
                    corresponding_m_intervals.push_back(IntVarArray(home, curr_cp->getMelodicIntervals().slice(0, 4, curr_cp->getMelodicIntervals().size())));
                } else if(curr_cp->getSpecies()==SECOND_SPECIES){
                    corresponding_m_intervals.push_back(IntVarArray(home, curr_cp->getMelodicIntervals().slice(2, 4, curr_cp->getMelodicIntervals().size())));
                } else if(curr_cp->getSpecies()==THIRD_SPECIES){
                    corresponding_m_intervals.push_back(IntVarArray(home, curr_cp->getMelodicIntervals().slice(3, 4, curr_cp->getMelodicIntervals().size())));
                } else if(curr_cp->getSpecies()==FOURTH_SPECIES){
                    corresponding_m_intervals.push_back(IntVarArray(home, curr_cp->getMelodicIntervals().slice(2, 4, curr_cp->getMelodicIntervals().size())));
                } else{
                    corresponding_m_intervals.push_back(IntVarArray(home, curr_cp->getMelodicIntervals().slice(2, 4, curr_cp->getMelodicIntervals().size())));
                }
            }

            rel(home, corresponding_m_intervals[0][i-1], IRT_EQ, m_intervals_brut[i-1], Reify(cf_is_lowest,RM_IMP));
            rel(home, corresponding_m_intervals[1][i-1], IRT_EQ, m_intervals_brut[i-1], Reify(cp1_is_lowest,RM_IMP));
            if(nVoices>=3){
                rel(home, corresponding_m_intervals[2][i-1], IRT_EQ, m_intervals_brut[i-1], Reify(cp2_is_lowest,RM_IMP));
            }
            if(nVoices>=4){
                rel(home, corresponding_m_intervals[3][i-1], IRT_EQ, m_intervals_brut[i-1], Reify(cp3_is_lowest,RM_IMP));
            }

        }
    }
}