#include "../headers/fuxTest.hpp"

void test_2v_1sp_fig22(Home home, Part* counterpoint){

    //cantusFirmus = {57,60,59,62,60,64,65,64,62,60,59,57}; //1sp 2v cf
    vector<int> cp ={69,64,67,65,64,72,69,71,71,69,68,69};
    for(int i = 0; i < counterpoint->getNMeasures(); i++){
        //la mi so fa mi do la si si la so# la
        //69 64 67 65 64 72 69 71 71 69 68  69
        
        rel(home, counterpoint->getFirstNotes()[i], IRT_EQ, cp[i]);
    }
    
}

void test_3v_1sp_fig109(Home home, Part* counterpoint_1, Part* counterpoint_2){

    //cantusFirmus = {52,48,50,48,45,57,55,52,53,52}; //1sp 3v cf
    vector<int> cp1 = {67,64,65,64,64,64,67,67,62,64};
    vector<int> cp2 = {59,60,57,57,60,60,59,59,57,59};
    for(int i = 0; i < 10; i++){
        //la mi so fa mi do la si si la so# la
        //69 64 67 65 64 72 69 71 71 69 68  69
        
        rel(home, counterpoint_1->getFirstNotes()[i], IRT_EQ, cp1[i]);
        rel(home, counterpoint_2->getFirstNotes()[i], IRT_EQ, cp2[i]);
    }
    
}

void test_3v_1sp_fig110(Home home, Part* counterpoint_1, Part* counterpoint_2){
    
    //v_type = {-1,-3}
    //cantusFirmus = {65,67,69,65,62,64,65,72,69,65,67,65}; //1sp 3v cf
    vector<int> cp1 = {60,60,60,62,58,59,57,69,65,62,64,65};
    vector<int> cp2 = {53,52,53,50,55,55,53,45,50,50,48,41};
    for(int i = 0; i < 12; i++){
        //la mi so fa mi do la si si la so# la
        //69 64 67 65 64 72 69 71 71 69 68  69
        
        rel(home, counterpoint_1->getFirstNotes()[i], IRT_EQ, cp1[i]);
        rel(home, counterpoint_2->getFirstNotes()[i], IRT_EQ, cp2[i]);
    }
    
}