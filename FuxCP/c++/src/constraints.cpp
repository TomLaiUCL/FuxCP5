#include "../headers/constraints.hpp"

void no_chromatic_melodies(Home home, IntVarArray m_intervals){
    for(int i = 0; i < m_intervals.size()-1; i++){
        rel(home, expr(home, m_intervals[i]==1), BOT_AND, expr(home, m_intervals[i+1]==1), 0);
        rel(home, expr(home, m_intervals[i]==-1), BOT_AND, expr(home, m_intervals[i+1]==-1), 0);
    }
}