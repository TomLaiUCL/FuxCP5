#ifndef MIDI_HPP
#define MIDI_HPP

#include <vector>
#include <string>
#include "Utilities.hpp"
#include "CounterpointProblems/CounterpointProblem.hpp"

// Utils
std::vector<int> extract_last_voice_notes(CounterpointProblem* best,
        vector<Species> spList,
        size_t cfSize);


/**
 * Génère un fichier MIDI à partir d'un Cantus Firmus et d'une solution.
 * @param filename Nom du fichier de sortie (ex: "resultat.mid")
 * @param cantusFirmus Vecteur de notes MIDI
 * @param solution Vecteur de notes MIDI (contrepoint)
 * @param species L'espèce utilisée pour déterminer le rythme
 */
void saveMidi(const std::string& filename, 
              const std::vector<int>& cantusFirmus, 
              const std::vector<int>& solution, 
              Species species);

#endif