#!/bin/bash
# =============================================================
# Script de réorganisation des résultats existants
# Déplace les fichiers vers la nouvelle arborescence :
#   results/<Nvoices>/cf<N>/{txt,csv,midi}/
# =============================================================

RESULTS_DIR="$(dirname "$0")/results"
MIDI_DIR="$(dirname "$0")/../MIDIfiles"

# Mapping des CF connus (notes MIDI → numéro CF)
declare -A CF_MAP
CF_MAP["60 64 65 67 62 65 64 62 60"]=1   # Do Majeur
CF_MAP["69 67 65 69 62 64 65 68 69"]=2   # La Mineur
CF_MAP["65 69 70 72 67 69 67 65 65"]=3   # Fa Majeur
CF_MAP["62 65 67 62 64 60 62 61 62"]=4   # Ré Dorien

# Noms de tonalité
declare -A CF_TONALITY
CF_TONALITY[1]="Do Majeur"
CF_TONALITY[2]="La Mineur"
CF_TONALITY[3]="Fa Majeur"
CF_TONALITY[4]="Ré Dorien"

# Notes MIDI → noms français
midi_to_french() {
    local note=$1
    local names=("Do" "Do#" "Ré" "Mib" "Mi" "Fa" "Fa#" "Sol" "Lab" "La" "Sib" "Si")
    local octave=$(( note / 12 - 1 ))
    local idx=$(( note % 12 ))
    echo "${names[$idx]}${octave}"
}

# Crée le fichier cantus_firmus.txt pour un dossier CF
create_cf_descriptor() {
    local dest_dir="$1"
    local cf_num="$2"
    local cf_notes="$3"

    local desc_file="${dest_dir}/cantus_firmus.txt"
    if [ -f "$desc_file" ]; then
        return
    fi

    # Ligne 1 : notes MIDI
    echo "$cf_notes" > "$desc_file"

    # Ligne 2 : noms français
    local french=""
    for note in $cf_notes; do
        if [ -n "$french" ]; then
            french="$french "
        fi
        french="${french}$(midi_to_french "$note")"
    done
    echo "$french" >> "$desc_file"

    # Ligne 3 : tonalité
    echo "${CF_TONALITY[$cf_num]}" >> "$desc_file"
    echo "  Créé : $desc_file"
}

# Extrait le CF depuis un fichier .txt de résultat
extract_cf() {
    local txt_file="$1"
    grep "^Cantus Firmus : " "$txt_file" | head -1 | sed 's/^Cantus Firmus : //' | sed 's/ *$//'
}

# Détermine le nombre de voix depuis le nom de fichier
get_nb_voices() {
    echo "$1" | grep -oP '^\d+' 
}

# Déplace un ensemble de fichiers (txt/csv/mid) vers la nouvelle structure
move_result_files() {
    local src_dir="$1"
    local basename="$2"  # ex: 2voices_1_1
    local cf_num="$3"
    local cf_notes="$4"

    local nb_voices
    nb_voices=$(get_nb_voices "$basename")
    local voices_dir="${nb_voices}voices"
    local cf_dir="cf${cf_num}"
    local dest_base="${RESULTS_DIR}/${voices_dir}/${cf_dir}"

    mkdir -p "${dest_base}/txt" "${dest_base}/csv" "${dest_base}/midi"
    create_cf_descriptor "$dest_base" "$cf_num" "$cf_notes"

    # Déplacer .txt
    if [ -f "${src_dir}/${basename}.txt" ]; then
        mv "${src_dir}/${basename}.txt" "${dest_base}/txt/"
        echo "  Déplacé : ${basename}.txt → ${voices_dir}/${cf_dir}/txt/"
    fi
    # Déplacer .csv
    if [ -f "${src_dir}/${basename}.csv" ]; then
        mv "${src_dir}/${basename}.csv" "${dest_base}/csv/"
        echo "  Déplacé : ${basename}.csv → ${voices_dir}/${cf_dir}/csv/"
    fi
    # Déplacer .mid
    if [ -f "${src_dir}/${basename}.mid" ]; then
        mv "${src_dir}/${basename}.mid" "${dest_base}/midi/"
        echo "  Déplacé : ${basename}.mid → ${voices_dir}/${cf_dir}/midi/"
    fi
}

echo "=== Réorganisation des résultats ==="

# 1. Traiter les sous-dossiers CF existants (cf1_do_majeur, cf2_la_mineur, etc.)
for cf_subdir in "${RESULTS_DIR}"/cf*_*/; do
    [ -d "$cf_subdir" ] || continue
    dirname=$(basename "$cf_subdir")
    # Extraire le numéro CF du nom de dossier (cf1_do_majeur → 1)
    cf_num=$(echo "$dirname" | grep -oP '(?<=cf)\d+')
    echo ""
    echo "--- Traitement du dossier $dirname (CF n°$cf_num) ---"

    for txt_file in "${cf_subdir}"/*.txt; do
        [ -f "$txt_file" ] || continue
        basename_noext=$(basename "$txt_file" .txt)
        cf_notes=$(extract_cf "$txt_file")
        move_result_files "$cf_subdir" "$basename_noext" "$cf_num" "$cf_notes"
    done

    # Déplacer les .mid et .csv restants (sans .txt correspondant)
    for mid_file in "${cf_subdir}"/*.mid; do
        [ -f "$mid_file" ] || continue
        basename_noext=$(basename "$mid_file" .mid)
        nb_voices=$(get_nb_voices "$basename_noext")
        dest_base="${RESULTS_DIR}/${nb_voices}voices/cf${cf_num}/midi"
        mkdir -p "$dest_base"
        mv "$mid_file" "$dest_base/"
        echo "  Déplacé : ${basename_noext}.mid → ${nb_voices}voices/cf${cf_num}/midi/"
    done
    for csv_file in "${cf_subdir}"/*.csv; do
        [ -f "$csv_file" ] || continue
        basename_noext=$(basename "$csv_file" .csv)
        nb_voices=$(get_nb_voices "$basename_noext")
        dest_base="${RESULTS_DIR}/${nb_voices}voices/cf${cf_num}/csv"
        mkdir -p "$dest_base"
        mv "$csv_file" "$dest_base/"
        echo "  Déplacé : ${basename_noext}.csv → ${nb_voices}voices/cf${cf_num}/csv/"
    done

    # Supprimer le dossier s'il est vide
    rmdir "$cf_subdir" 2>/dev/null && echo "  Supprimé dossier vide : $dirname"
done

# 2. Traiter les fichiers à la racine de results/
echo ""
echo "--- Traitement des fichiers à la racine de results/ ---"
for txt_file in "${RESULTS_DIR}"/*.txt; do
    [ -f "$txt_file" ] || continue
    basename_noext=$(basename "$txt_file" .txt)
    cf_notes=$(extract_cf "$txt_file")
    
    # Identifier le CF
    cf_num=""
    for key in "${!CF_MAP[@]}"; do
        if [ "$key" = "$cf_notes" ]; then
            cf_num="${CF_MAP[$key]}"
            break
        fi
    done

    if [ -z "$cf_num" ]; then
        echo "  ATTENTION : CF inconnu pour ${basename_noext} ($cf_notes) — ignoré"
        continue
    fi

    move_result_files "$RESULTS_DIR" "$basename_noext" "$cf_num" "$cf_notes"
done

# Déplacer les .mid et .csv orphelins à la racine (même CF que le .txt correspondant)
for mid_file in "${RESULTS_DIR}"/*.mid; do
    [ -f "$mid_file" ] || continue
    basename_noext=$(basename "$mid_file" .mid)
    nb_voices=$(get_nb_voices "$basename_noext")
    # Par défaut cf4 pour les fichiers racine (comme déterminé par analyse)
    dest_base="${RESULTS_DIR}/${nb_voices}voices/cf4/midi"
    mkdir -p "$dest_base"
    mv "$mid_file" "$dest_base/"
    echo "  Déplacé : ${basename_noext}.mid → ${nb_voices}voices/cf4/midi/"
done
for csv_file in "${RESULTS_DIR}"/*.csv; do
    [ -f "$csv_file" ] || continue
    basename_noext=$(basename "$csv_file" .csv)
    nb_voices=$(get_nb_voices "$basename_noext")
    dest_base="${RESULTS_DIR}/${nb_voices}voices/cf4/csv"
    mkdir -p "$dest_base"
    mv "$csv_file" "$dest_base/"
    echo "  Déplacé : ${basename_noext}.csv → ${nb_voices}voices/cf4/csv/"
done

# 3. Traiter les fichiers MIDI dans MIDIfiles/
echo ""
echo "--- Traitement des fichiers dans MIDIfiles/ ---"
if [ -d "$MIDI_DIR" ]; then
    for mid_file in "${MIDI_DIR}"/*.mid; do
        [ -f "$mid_file" ] || continue
        basename_noext=$(basename "$mid_file" .mid)
        nb_voices=$(get_nb_voices "$basename_noext")

        # Chercher le CF dans le fichier txt correspondant (dans la nouvelle structure)
        found=0
        for cf_num in 1 2 3 4; do
            if [ -f "${RESULTS_DIR}/${nb_voices}voices/cf${cf_num}/txt/${basename_noext}.txt" ]; then
                dest_base="${RESULTS_DIR}/${nb_voices}voices/cf${cf_num}/midi"
                mkdir -p "$dest_base"
                if [ ! -f "${dest_base}/${basename_noext}.mid" ]; then
                    mv "$mid_file" "$dest_base/"
                    echo "  Déplacé : ${basename_noext}.mid → ${nb_voices}voices/cf${cf_num}/midi/"
                else
                    echo "  Existe déjà : ${nb_voices}voices/cf${cf_num}/midi/${basename_noext}.mid — suppression source"
                    rm "$mid_file"
                fi
                found=1
                break
            fi
        done

        if [ $found -eq 0 ]; then
            # Fallback : cf4 pour les fichiers racine
            dest_base="${RESULTS_DIR}/${nb_voices}voices/cf4/midi"
            mkdir -p "$dest_base"
            mv "$mid_file" "$dest_base/"
            echo "  Déplacé (fallback cf4) : ${basename_noext}.mid → ${nb_voices}voices/cf4/midi/"
        fi
    done
fi

echo ""
echo "=== Réorganisation terminée ==="
echo ""
echo "Nouvelle structure :"
find "$RESULTS_DIR" -maxdepth 3 -type d | sort | head -30
