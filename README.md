# FuxCP — Génération de contrepoints par contraintes

Système de génération automatique de contrepoints (style Fux) basé sur **Gecode**.
Tout est piloté par 3 fichiers CSV de configuration et une CLI bash interactive.

---

## 1. Arborescence

```
FuxCP/
├── fuxcp                ← interface CLI interactive (menu + sous-commandes)
├── compiled/            ← binaire généré (GenerateCounterpoint)
├── config/              ← configuration (3 CSV — source unique de vérité)
│   ├── cantus_firmus.csv
│   ├── presets.csv
│   └── campaigns.csv
├── c++/
│   ├── Makefile         ← commandes de compilation
│   ├── headers/         ← headers du moteur
│   └── src/             ← sources du moteur (GenerateCounterpoint.cpp = entrée)
└── results/             ← sorties + logs
    ├── 2voices/, 3voices/, 4voices/   ← générations par défaut
    ├── <subdir>/        ← sorties de campagnes (ex: courts/2v/, longs/2v/)
    └── cli_logs/        ← logs de la CLI (.log)
```

---

## 2. Compilation (Makefile)

Toutes les commandes se lancent depuis `FuxCP/c++/`.

| Commande | Effet |
|---|---|
| `make` ou `make compile_generate_counterpoint` | Compile le binaire `../compiled/GenerateCounterpoint` |
| `make generate COMBO="2 1"` | Génère un contrepoint (CF=1 par défaut) |
| `make generate COMBO="2 1" CF=3` | Idem avec CF=3 |
| `make generate COMBO="4 1 2 3" CF=2 TIMEOUT=300000 STAGNATION=120000` | Avec timeout |
| `make so` | Compile la lib dynamique `libgecode.so` |
| `make clean` | Supprime binaires |
| `make clean_results` | Supprime `2voices/`, `3voices/`, `4voices/` |
| `make clean_all` | Tout supprimer (binaires + résultats) |

---

## 3. Interface CLI interactive (`fuxcp`)

Depuis `FuxCP/`.

### Menu interactif

```
./fuxcp
```

11 options : génération précise, lancement de campagne, génération en masse,
surveillance temps réel, exploration des résultats, inspection des logs, etc.

### Sous-commandes directes

| Commande | Effet |
|---|---|
| `./fuxcp list-cf` | Liste les 6 cantus firmus disponibles |
| `./fuxcp list-presets` | Liste les presets de paramètres |
| `./fuxcp list-camps` | Liste les campagnes disponibles |
| `./fuxcp campaign <id>` | Lance une campagne (propose mode arrière-plan) |
| `./fuxcp generate 2 1 -c 4 -t 60000 -s 30000 --subdir test` | Forward direct au binaire |
| `./fuxcp tail` | Suit en direct le dernier log |

### Suivre une campagne en cours

Une fois une campagne lancée en arrière-plan via le menu :

```
./fuxcp        # menu → option 4 (Surveiller)
```

ou directement :

```
tail -f results/cli_logs/campagne_<id>_*.log
```

Filtres utiles :

```
grep -E "STAGNATION|TIMEOUT|EXHAUSTIVE|Coût=|Solutions=" results/cli_logs/campagne_*.log
```

---

## 4. Cantus firmus disponibles (`config/cantus_firmus.csv`)

6 CF couvrant tonalités majeures, mineures et 2 modes ecclésiastiques.

| ID | Nom | Notes | Gamme indicative |
|---|---|---|---|
| 1 | Ré Dorien | 11 | Dorien |
| 2 | Mi Phrygien | 10 | Phrygien |
| 3 | Fa Lydien | 12 | Lydien |
| 4 | Sol Mixolydien | 14 | Mixolydien |
| 5 | La Éolien | 12 | Mineur naturel |
| 6 | Do Ionien | 12 | Majeur |

Pour ajouter un CF : éditer [`FuxCP/config/cantus_firmus.csv`](FuxCP/config/cantus_firmus.csv), ajouter une ligne
`id,notes,name,scale,description` (notes MIDI séparées par des espaces).
**Aucun fichier .txt à créer**, tout est dans le CSV.

> La colonne `scale` est purement indicative : la gamme effective est toujours
> détectée automatiquement par le moteur à partir des notes du CF.

---

## 5. Gammes supportées (détection automatique)

Le moteur détecte la gamme du CF en cherchant celle qui couvre le plus de notes
(fonction `detect_scale_for_cf` dans [`FuxCP/c++/src/Utilities.cpp`](FuxCP/c++/src/Utilities.cpp)).

13 gammes candidates :

| Famille | Gammes |
|---|---|
| **Tonalités classiques** | Majeur, Mineur naturel, Mineur harmonique, Mineur mélodique |
| **Modes ecclésiastiques** | Dorien, Phrygien, Lydien, Mixolydien, Locrien |
| **Pentatoniques** | Pentatonique majeure, Pentatonique mineure |
| **Blues** | Blues majeure, Blues mineure |

> Note : « Majeur » et « Ionien » ont les mêmes intervalles ; idem pour
> « Mineur naturel » et « Aeolien ». La CLI utilise les noms tonals (Majeur/Mineur)
> pour la lisibilité.

---

## 6. Campagnes prédéfinies (`config/campaigns.csv`)

| ID | Configurations | Description |
|---|---|---|
| `demo_rapide` | 1 | Démo 30s sur CF1 |
| `courts_2v` | 5 (CF 1-3 × 5 espèces) | CF courts en 2 voix |
| `longs_2v` | 5 (CF 4-6 × 5 espèces) | CF longs en 2 voix |
| `gammes_3v` | 5 | Tous les CF en 3 voix, espèces 1ère + cible |
| `gammes_4v` | 3 | Tous les CF en 4 voix |

Lancer : `./fuxcp campaign <id>`

Pour ajouter une campagne : éditer [`FuxCP/config/campaigns.csv`](FuxCP/config/campaigns.csv).

---

## 7. Presets de paramètres (`config/presets.csv`)

Chaque preset définit les listes `melodic_params`, `general_params`,
`specific_params`, `importance` et le `borrow_mode`.

Presets disponibles : `default`, `strict`, `musique`, `variete_haute`, `bryce`.

Lister : `./fuxcp list-presets`

---

## 8. Voir les résultats

Chaque génération produit 3 fichiers + un CSV récapitulatif :

```
FuxCP/results/<subdir>/<nv>voices/cf<id>/
├── cantus_firmus.txt               ← CF lu (vérification)
├── txt/<nv>voices_1_<sp>.txt       ← partition lisible
├── csv/<nv>voices_1_<sp>.csv       ← coûts détaillés
└── midi/<nv>voices_1_<sp>.mid      ← export MIDI
```

Naviguer : `./fuxcp` → option 5 (Explorer les résultats).

---

## 9. Logs et statistiques

| Emplacement | Contenu |
|---|---|
| `results/cli_logs/precise_*.log` | Logs des générations précises (option menu 1) |
| `results/cli_logs/campagne_<id>_*.log` | Logs des campagnes (option menu 2) |
| `results/cli_logs/campagne_<id>.pid` | PID des campagnes lancées en arrière-plan |

Surveillance/inspection : options 4 et 6 du menu `./fuxcp`.

Stats clés affichées par génération :
- `Solutions=N` — nombre de solutions trouvées
- `Améliorations=K` — nombre d'améliorations sur le coût
- `Coût=X` — meilleur coût atteint
- Terminaison : `EXHAUSTIVE` / `STAGNATION` / `TIMEOUT_MAX`

---

## 10. Exemple complet

```bash
# 1. Compilation
cd FuxCP/c++ && make
cd ..

# 2. Vérifier les CF
./fuxcp list-cf

# 3. Lancer une démo rapide (30s)
./fuxcp campaign demo_rapide

# 4. Génération précise : 3 voix, espèces 1+5 sur CF Mi Phrygien
./fuxcp generate 3 1 5 -c 6 -t 60000 -s 30000 --subdir mon_test

# 5. Voir les résultats
ls results/mon_test/3voices/cf6/
```
