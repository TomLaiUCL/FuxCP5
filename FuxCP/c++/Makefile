# =============================================================
# Makefile — FuxCP : génération de contrepoints par contraintes
# =============================================================
# Cibles principales :
#   make                                      → compile GenerateCounterpoint
#   make generate COMBO="2 1"                 → génère un contrepoint (CF=1 par défaut)
#   make generate COMBO="2 1" CF=3            → avec cantus firmus n°3
#   make generate COMBO="3 2 5" CF=2 TIMEOUT=300000 STAGNATION=120000
#   make generate_all                         → toutes combinaisons (espèces 1-5, CF 1-4)
#   make generate_all TIMEOUT=300000 STAGNATION=120000 → avec timeout personnalisé
#   make test                                 → compile + lance la suite de tests
#   make clean                                → supprime les binaires compilés
#   make clean_results                        → supprime résultats (.txt/.csv/.mid)
#   make clean_all                            → tout supprimer
# =============================================================

# === DOSSIERS ===
COMPILED_DIR = ../compiled
RESULTS_DIR  = ../results
OUT_DIR      = ../sources

# === CANTUS FIRMUS (défaut : 1) ===
CF ?= 1

$(shell mkdir -p $(COMPILED_DIR) $(RESULTS_DIR))

# === COMPILATEUR ===
CXX            = g++
CXXFLAGS       = -std=c++11 -O3 -ffast-math -fPIC
CXXFLAGS_DEBUG = -g -std=c++11
LDFLAGS        = -shared

LIBS = -lgecodedriver -lgecodeflatzinc -lgecodefloat -lgecodeint \
       -lgecodekernel -lgecodeminimodel -lgecodesearch -lgecodeset -lgecodesupport

LIBS_MINIMAL = -lgecodekernel -lgecodesearch -lgecodeint \
               -lgecodedriver -lgecodeminimodel -lgecodesupport

# === SOURCES PARTAGÉES ===
SHARED_SOURCES = \
	src/Utilities.cpp src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp \
	src/CounterpointProblems/CounterpointProblem.cpp \
	src/Parts/Part.cpp \
	src/Parts/FirstSpeciesCounterpoint.cpp \
	src/Parts/SecondSpeciesCounterpoint.cpp \
	src/Parts/ThirdSpeciesCounterpoint.cpp \
	src/Parts/FourthSpeciesCounterpoint.cpp \
	src/Parts/FifthSpeciesCounterpoint.cpp \
	src/Parts/CantusFirmus.cpp \
	src/CounterpointProblems/TwoVoiceCounterpoint.cpp \
	src/CounterpointProblems/ThreeVoiceCounterpoint.cpp \
	src/CounterpointProblems/FourVoiceCounterpoint.cpp \
	src/constraints.cpp

# =============================================================
# CIBLE PAR DÉFAUT
# =============================================================

all: compile_generate_counterpoint

# =============================================================
# BIBLIOTHÈQUE DYNAMIQUE LINUX (.so)
# (utilisée si FuxCP est appelé depuis un autre programme ou Lisp)
# =============================================================

SRC_DIR = src
SRCS    = $(shell find $(SRC_DIR) -name '*.cpp')
OBJS    = $(patsubst $(SRC_DIR)/%.cpp, $(COMPILED_DIR)/%.o, $(SRCS))
TARGET  = $(OUT_DIR)/libgecode.so

$(COMPILED_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

so: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

# =============================================================
# SUITE DE TESTS (à lancer après un changement de code)
# =============================================================

SOURCES_TEST = main.cpp $(SHARED_SOURCES) src/Midi.cpp src/generations.cpp src/fuxTest.cpp src/figureTests.cpp

compile_tests:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) $(SOURCES_TEST) -o $(COMPILED_DIR)/Main $(LIBS_MINIMAL)

dylib: src/problem_wrapper.cpp
	$(CXX) src/Utilities.cpp src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp src/CounterpointProblems/CounterpointProblem.cpp \
	    src/Parts/Part.cpp src/Parts/FirstSpeciesCounterpoint.cpp src/Parts/SecondSpeciesCounterpoint.cpp src/Parts/ThirdSpeciesCounterpoint.cpp \
	    src/Parts/FourthSpeciesCounterpoint.cpp src/Parts/FifthSpeciesCounterpoint.cpp src/Parts/CantusFirmus.cpp \
	    src/CounterpointProblems/TwoVoiceCounterpoint.cpp src/CounterpointProblems/ThreeVoiceCounterpoint.cpp \
	    src/CounterpointProblems/FourVoiceCounterpoint.cpp src/fuxTest.cpp src/constraints.cpp src/problem_wrapper.cpp \
	    -std=c++11 -dynamiclib -fPIC -F/Library/Frameworks -framework gecode -o $(OUT_DIR)/libgecode.dylib -ffast-math -O3
	install_name_tool -change gecode.framework/Versions/49/gecode /Library/Frameworks/gecode.framework/Versions/49/gecode $(OUT_DIR)/libgecode.dylib

# === LINUX ===

run_linux:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) -std=c++11 main.cpp -o $(COMPILED_DIR)/Main src/Utilities.cpp src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp \
	    src/CounterpointProblems/CounterpointProblem.cpp src/Parts/Part.cpp src/Parts/FirstSpeciesCounterpoint.cpp \
	    src/Parts/SecondSpeciesCounterpoint.cpp src/Parts/ThirdSpeciesCounterpoint.cpp src/Parts/FourthSpeciesCounterpoint.cpp \
	    src/Parts/FifthSpeciesCounterpoint.cpp src/Parts/CantusFirmus.cpp src/CounterpointProblems/TwoVoiceCounterpoint.cpp \
	    src/CounterpointProblems/ThreeVoiceCounterpoint.cpp src/CounterpointProblems/FourVoiceCounterpoint.cpp \
	    src/fuxTest.cpp src/constraints.cpp $(LIBS_MINIMAL)
	$(COMPILED_DIR)/Main

# === DEBUG ===

SOURCES = main.cpp src/Utilities.cpp src/Midi.cpp src/Generations.cpp  src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp \
	    src/CounterpointProblems/CounterpointProblem.cpp src/Parts/Part.cpp src/Parts/FirstSpeciesCounterpoint.cpp \
	    src/Parts/SecondSpeciesCounterpoint.cpp src/Parts/ThirdSpeciesCounterpoint.cpp src/Parts/FourthSpeciesCounterpoint.cpp \
	    src/Parts/FifthSpeciesCounterpoint.cpp src/Parts/CantusFirmus.cpp src/CounterpointProblems/TwoVoiceCounterpoint.cpp \
	    src/CounterpointProblems/ThreeVoiceCounterpoint.cpp src/CounterpointProblems/FourVoiceCounterpoint.cpp \
	    src/fuxTest.cpp src/constraints.cpp src/figureTests.cpp

compile_for_debug:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) $(SOURCES) -o $(COMPILED_DIR)/Main $(LIBS_MINIMAL)

compile_and_bryce:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) $(SOURCES) -o $(COMPILED_DIR)/Main $(LIBS_MINIMAL)
	$(COMPILED_DIR)/Main bryce $(btest)

compile_and_gen:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) $(SOURCES) -o $(COMPILED_DIR)/Main $(LIBS_MINIMAL)
	$(COMPILED_DIR)/Main gen $(case)

# === TESTS ===

test: compile_tests
	$(COMPILED_DIR)/Main all

# =============================================================
# GÉNÉRATEUR DE CONTREPOINTS
# =============================================================

GENERATE_COUNTERPOINT_SOURCES = src/GenerateCounterpoint.cpp $(SHARED_SOURCES) src/Midi.cpp

compile_generate_counterpoint:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) $(GENERATE_COUNTERPOINT_SOURCES) \
	    -o $(COMPILED_DIR)/GenerateCounterpoint $(LIBS_MINIMAL)

# --- Génération d'un contrepoint précis ---
# Usage : make generate COMBO="<nb_voix> <especes...>" [CF=<n>] [TIMEOUT=<ms>] [STAGNATION=<ms>]
# Exemple : make generate COMBO="2 1"
# Exemple : make generate COMBO="4 1 2 3" CF=2 TIMEOUT=300000 STAGNATION=120000
generate: compile_generate_counterpoint
	$(COMPILED_DIR)/GenerateCounterpoint $(COMBO) -c $(CF) $(if $(TIMEOUT),-t $(TIMEOUT),) $(if $(STAGNATION),-s $(STAGNATION),)

# --- Génération échantillonnée (max 5 combos par CF par nb_voix) ---
# 2 voix : les 5 espèces (1-5)
# 3 voix : 5 combos représentatives
# 4 voix : 5 combos représentatives
# Usage : make generate_sample [TIMEOUT=<ms>] [STAGNATION=<ms>]
generate_sample: compile_generate_counterpoint
	@echo "=== Génération échantillonnée (max 5 par CF par nb_voix) ==="
	@for cf in 1 2 3 4 5; do \
		echo "--- CF $$cf ---"; \
		voices_dir="2voices"; cf_dir="cf$${cf}"; \
		base="$(RESULTS_DIR)/$${voices_dir}/$${cf_dir}"; \
		for sp1 in 1 2 3 4 5; do \
			tag="2voices_1_$${sp1}"; \
			if [ -f "$${base}/txt/$$tag.txt" ] && [ -f "$${base}/csv/$$tag.csv" ] && [ -f "$${base}/midi/$$tag.mid" ]; then \
				echo "  Déjà complet : $$tag (cf$$cf)"; \
			elif [ -f "$${base}/txt/error_$$tag.txt" ]; then \
				echo "  Déjà en erreur : $$tag (cf$$cf)"; \
			else \
				echo "  Génération : $$tag (cf$$cf)"; \
				$(COMPILED_DIR)/GenerateCounterpoint 2 $$sp1 -c $$cf $(if $(TIMEOUT),-t $(TIMEOUT),) $(if $(STAGNATION),-s $(STAGNATION),); \
			fi; \
		done; \
		voices_dir="3voices"; \
		base="$(RESULTS_DIR)/$${voices_dir}/$${cf_dir}"; \
		for combo in "1 1" "1 3" "2 4" "3 5" "5 2"; do \
			set -- $$combo; sp1=$$1; sp2=$$2; \
			tag="3voices_1_$${sp1}_$${sp2}"; \
			if [ -f "$${base}/txt/$$tag.txt" ] && [ -f "$${base}/csv/$$tag.csv" ] && [ -f "$${base}/midi/$$tag.mid" ]; then \
				echo "  Déjà complet : $$tag (cf$$cf)"; \
			elif [ -f "$${base}/txt/error_$$tag.txt" ]; then \
				echo "  Déjà en erreur : $$tag (cf$$cf)"; \
			else \
				echo "  Génération : $$tag (cf$$cf)"; \
				$(COMPILED_DIR)/GenerateCounterpoint 3 $$sp1 $$sp2 -c $$cf $(if $(TIMEOUT),-t $(TIMEOUT),) $(if $(STAGNATION),-s $(STAGNATION),); \
			fi; \
		done; \
		voices_dir="4voices"; \
		base="$(RESULTS_DIR)/$${voices_dir}/$${cf_dir}"; \
		for combo in "1 1 1" "1 2 3" "2 3 4" "3 4 5" "1 3 5"; do \
			set -- $$combo; sp1=$$1; sp2=$$2; sp3=$$3; \
			tag="4voices_1_$${sp1}_$${sp2}_$${sp3}"; \
			if [ -f "$${base}/txt/$$tag.txt" ] && [ -f "$${base}/csv/$$tag.csv" ] && [ -f "$${base}/midi/$$tag.mid" ]; then \
				echo "  Déjà complet : $$tag (cf$$cf)"; \
			elif [ -f "$${base}/txt/error_$$tag.txt" ]; then \
				echo "  Déjà en erreur : $$tag (cf$$cf)"; \
			else \
				echo "  Génération : $$tag (cf$$cf)"; \
				$(COMPILED_DIR)/GenerateCounterpoint 4 $$sp1 $$sp2 $$sp3 -c $$cf $(if $(TIMEOUT),-t $(TIMEOUT),) $(if $(STAGNATION),-s $(STAGNATION),); \
			fi; \
		done; \
	done
	@echo "=== Terminé ==="

# --- Génération de toutes les combinaisons (espèces 1-5, CF 1-4, 2/3/4 voix) ---
# Saute une combinaison si les 3 fichiers (.txt + .csv + .mid) existent déjà.
# Usage : make generate_all [TIMEOUT=<ms>] [STAGNATION=<ms>]
generate_all: compile_generate_counterpoint
	@echo "=== Génération de tous les contrepoints (espèces 1-5, CF 1-5, 2/3/4 voix) ==="
	@for cf in 1 2 3 4 5; do \
		for nb_voix in 2 3 4; do \
			voices_dir="$${nb_voix}voices"; \
			cf_dir="cf$${cf}"; \
			base="$(RESULTS_DIR)/$${voices_dir}/$${cf_dir}"; \
			if [ $$nb_voix -eq 2 ]; then \
				for sp1 in 1 2 3 4 5; do \
					tag="$${nb_voix}voices_1_$${sp1}"; \
					if [ -f "$${base}/txt/$$tag.txt" ] && [ -f "$${base}/csv/$$tag.csv" ] && [ -f "$${base}/midi/$$tag.mid" ]; then \
						echo "  Déjà complet : $$tag (cf$$cf)"; \
					elif [ -f "$${base}/txt/error_$$tag.txt" ]; then \
						echo "  Déjà en erreur : $$tag (cf$$cf)"; \
					else \
						echo "  Génération : $$tag (cf$$cf)"; \
						$(COMPILED_DIR)/GenerateCounterpoint $$nb_voix $$sp1 -c $$cf $(if $(TIMEOUT),-t $(TIMEOUT),) $(if $(STAGNATION),-s $(STAGNATION),); \
					fi; \
				done; \
			elif [ $$nb_voix -eq 3 ]; then \
				for sp1 in 1 2 3 4 5; do \
					for sp2 in 1 2 3 4 5; do \
						tag="$${nb_voix}voices_1_$${sp1}_$${sp2}"; \
						if [ -f "$${base}/txt/$$tag.txt" ] && [ -f "$${base}/csv/$$tag.csv" ] && [ -f "$${base}/midi/$$tag.mid" ]; then \
							echo "  Déjà complet : $$tag (cf$$cf)"; \
						elif [ -f "$${base}/txt/error_$$tag.txt" ]; then \
							echo "  Déjà en erreur : $$tag (cf$$cf)"; \
						else \
							echo "  Génération : $$tag (cf$$cf)"; \
							$(COMPILED_DIR)/GenerateCounterpoint $$nb_voix $$sp1 $$sp2 -c $$cf $(if $(TIMEOUT),-t $(TIMEOUT),) $(if $(STAGNATION),-s $(STAGNATION),); \
						fi; \
					done; \
				done; \
			else \
				for sp1 in 1 2 3 4 5; do \
					for sp2 in 1 2 3 4 5; do \
						for sp3 in 1 2 3 4 5; do \
							tag="$${nb_voix}voices_1_$${sp1}_$${sp2}_$${sp3}"; \
							if [ -f "$${base}/txt/$$tag.txt" ] && [ -f "$${base}/csv/$$tag.csv" ] && [ -f "$${base}/midi/$$tag.mid" ]; then \
								echo "  Déjà complet : $$tag (cf$$cf)"; \
							elif [ -f "$${base}/txt/error_$$tag.txt" ]; then \
								echo "  Déjà en erreur : $$tag (cf$$cf)"; \
							else \
								echo "  Génération : $$tag (cf$$cf)"; \
								$(COMPILED_DIR)/GenerateCounterpoint $$nb_voix $$sp1 $$sp2 $$sp3 -c $$cf $(if $(TIMEOUT),-t $(TIMEOUT),) $(if $(STAGNATION),-s $(STAGNATION),); \
							fi; \
						done; \
					done; \
				done; \
			fi; \
		done; \
	done
	@echo "=== Terminé ==="

# =============================================================
# NETTOYAGE
# =============================================================

clean:
	rm -f $(COMPILED_DIR)/Main $(COMPILED_DIR)/GenerateCounterpoint
	rm -f $(TARGET)

clean_results:
	rm -rf $(RESULTS_DIR)/2voices $(RESULTS_DIR)/3voices $(RESULTS_DIR)/4voices

clean_all: clean clean_results
	rm -rf $(COMPILED_DIR)

.PHONY: all so compile_tests test \
	compile_generate_counterpoint generate generate_sample generate_all \
	clean clean_results clean_all