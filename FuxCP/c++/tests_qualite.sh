#!/bin/bash
# ===========================================================================
# Script de tests de qualité musicale — FuxCP
# Teste des cantus firmus longs avec contraintes actives et haute variété
# Résultats dans ../results/tests_qualite/ et dans les dossiers standards
# ===========================================================================

set -e

BINARY="../compiled/GenerateCounterpoint"
OUT_DIR="../results/tests_qualite"
LOG="$OUT_DIR/rapport_tests_qualite.txt"
TIMEOUT=180000    # 3 min max
STAGNATION=90000  # 1.5 min stagnation

mkdir -p "$OUT_DIR"

# Fonction pour exécuter un test et extraire les résultats
run_test() {
    local desc="$1"
    local nb_voix="$2"
    local species="$3"
    local cf="$4"
    local variety="$5"
    
    echo "--- $desc ---"
    echo "" >> "$LOG"
    echo "--- $desc ---" >> "$LOG"
    
    local cmd="$BINARY $nb_voix $species -c $cf -t $TIMEOUT -s $STAGNATION"
    if [ "$variety" != "2" ]; then
        cmd="$cmd -v $variety"
    fi
    
    echo "  Commande : $cmd" | tee -a "$LOG"
    
    local output
    output=$($cmd 2>&1) || true
    
    # Extraire les métriques
    local solutions=$(echo "$output" | grep -oP 'Solutions=\K[0-9]+' || echo "?")
    local cost=$(echo "$output" | grep -oP 'Coût=\K[0-9.]+' || echo "?")
    local reason=$(echo "$output" | grep -oP '(EXHAUSTIVE|STAGNATION|TIMEOUT_MAX)' | tail -1 || echo "?")
    local time=$(echo "$output" | grep -oP '[0-9]+(?= ms \|)' | tail -1 || echo "?")
    
    echo "  -> Solutions=$solutions | Cout=$cost | ${time}ms | $reason" | tee -a "$LOG"
    
    # Extraire et afficher les notes de la solution
    local result_file
    result_file=$(echo "$output" | grep -oP '(?<=sauvegardés : ).*?\.txt' || echo "")
    if [ -n "$result_file" ] && [ -f "$result_file" ]; then
        local cf_notes=$(grep "^Cantus Firmus" "$result_file" | tail -1)
        local voice_notes=$(grep "^Voix" "$result_file")
        echo "  $cf_notes" >> "$LOG"
        while IFS= read -r line; do
            echo "  $line" >> "$LOG"
        done <<< "$voice_notes"
    fi
    echo "" >> "$LOG"
}

# ============================= EN-TETE ==============================
cat > "$LOG" <<'HEADER'
========================================================================
  RAPPORT DE TESTS DE QUALITE MUSICALE - FuxCP
  Date : 20 avril 2026
  Objectif : compositions plus riches et melodieuses
========================================================================

Problemes identifies et corriges :
  1. activeConstraints n'etait PAS active dans GenerateCounterpoint.cpp
     -> Toutes les contraintes musicales (consonances, gamme, mouvement)
       etaient desactivees ! Resultats sans aucune logique musicale.
  2. Le cout de variete manquait dans les constructeurs 2 voix
     -> Aucune penalite pour notes repetitives en mode 2 voix.

Corrections appliquees :
  - fill(activeConstraints..., true) ajoute dans GenerateCounterpoint.cpp
  - varietyCostArray + add_cost ajoutes dans tous les constructeurs 2 voix
  - M2_1_varietyCost appele depuis TwoVoiceCounterpoint.cpp

Nouveaux cantus firmus (plus longs, en forme d'arche melodique) :
  CF6  : Do Majeur long    - 13 notes (60 62 65 64 67 69 67 64 65 62 64 62 60)
  CF7  : Fa Majeur express  - 14 notes (65 67 69 67 70 72 70 69 67 65 64 62 64 65)
  CF8  : Sol Majeur         - 13 notes (55 57 59 60 62 64 62 60 59 57 59 57 55)
  CF9  : Re Majeur long    - 13 notes (62 66 64 67 69 71 69 67 64 66 62 64 62)
  CF10 : Sib Majeur         - 11 notes (58 62 60 63 65 67 65 62 63 60 58)

Parametres : timeout=180s, stagnation=90s
========================================================================

HEADER

echo "=== Tests de qualite musicale FuxCP ==="
echo "Corrections : contraintes actives + variete 2 voix"
echo "Resultats dans : $OUT_DIR"
echo ""

# =================== PARTIE 1 : 2 voix, 1re espece ===================

echo "==============================================" | tee -a "$LOG"
echo "  PARTIE 1 : 2 voix, 1re espece" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"

for cf in 6 7 8 9 10; do
    run_test "2v 1re espece CF$cf variete=2" 2 1 $cf 2
done
for cf in 6 7 8; do
    run_test "2v 1re espece CF$cf variete=8" 2 1 $cf 8
done

# =================== PARTIE 2 : 2 voix, 3e espece ===================

echo "" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"
echo "  PARTIE 2 : 2 voix, 3e espece" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"

for cf in 6 7 8 9 10; do
    run_test "2v 3e espece CF$cf variete=2" 2 3 $cf 2
done
for cf in 6 7 8; do
    run_test "2v 3e espece CF$cf variete=8" 2 3 $cf 8
done

# =================== PARTIE 3 : 2 voix, 5e espece ===================

echo "" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"
echo "  PARTIE 3 : 2 voix, 5e espece" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"

for cf in 6 7; do
    run_test "2v 5e espece CF$cf variete=2" 2 5 $cf 2
    run_test "2v 5e espece CF$cf variete=8" 2 5 $cf 8
done

# =================== PARTIE 4 : 3 voix ===================

echo "" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"
echo "  PARTIE 4 : 3 voix sur CF longs" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"

run_test "3v especes 1+3 CF6 variete=8"  3 "1 3" 6 8
run_test "3v especes 1+3 CF8 variete=8"  3 "1 3" 8 8
run_test "3v especes 1+3 CF10 variete=8" 3 "1 3" 10 8

# =========== PARTIE 5 : Comparaison CF courts vs longs =============

echo "" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"
echo "  PARTIE 5 : Comparaison CF courts (9 notes)" | tee -a "$LOG"
echo "==============================================" | tee -a "$LOG"

run_test "2v 1re espece CF1 variete=8 (court)" 2 1 1 8
run_test "2v 3e espece CF1 variete=8 (court)"  2 3 1 8
run_test "2v 1re espece CF5 variete=8 (court)" 2 1 5 8
run_test "2v 3e espece CF5 variete=8 (court)"  2 3 5 8

# ============================= FIN ================================
echo "" >> "$LOG"
echo "========================================================================" >> "$LOG"
echo "  FIN DES TESTS" >> "$LOG"
echo "========================================================================" >> "$LOG"
echo ""
echo "=== Tests termines ==="
echo "Rapport complet : $LOG"
echo "Fichiers MIDI : ../results/{2,3}voices/cf{6..10}/midi/"
