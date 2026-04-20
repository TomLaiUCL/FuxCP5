#ifndef MIDI_HPP
#define MIDI_HPP

#include <vector>
#include <string>
#include <utility>
#include "../Utilities.hpp"
#include "../CounterpointProblems/CounterpointProblem.hpp"

// Utils
std::vector<int> extract_last_voice_notes(CounterpointProblem* best,
        vector<Species> spList,
        size_t cfSize);

/**
 * Calcule la taille du tableau de branching notes pour une espèce et une taille de CF données.
 */
int branchingNotesSize(Species sp, int cfSize);

/**
 * Génère un fichier MIDI à partir d'un Cantus Firmus et d'une solution (2 voix).
 * @param filename Nom du fichier de sortie (ex: "resultat.mid")
 * @param cantusFirmus Vecteur de notes MIDI
 * @param solution Vecteur de notes MIDI (contrepoint)
 * @param species L'espèce utilisée pour déterminer le rythme
 */
void saveMidi(const std::string& filename, 
              const std::vector<int>& cantusFirmus, 
              const std::vector<int>& solution, 
              Species species);

/**
 * Génère un fichier MIDI multi-voix Format 1 (une piste par voix).
 * Piste 0 = Cantus Firmus, pistes suivantes = voix de contrepoint.
 * @param filename Nom du fichier de sortie
 * @param cantusFirmus Vecteur de notes MIDI du cantus firmus
 * @param voices Vecteur de paires (notes, espèce) pour chaque voix de contrepoint
 */
void saveMidiMultiVoice(const std::string& filename,
                        const std::vector<int>& cantusFirmus,
                        const std::vector<std::pair<std::vector<int>, Species>>& voices);

#endif