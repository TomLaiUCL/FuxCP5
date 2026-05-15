# =============================================================
# Makefile — FuxCP : génération de contrepoints par contraintes
# =============================================================
# Cibles principales :
#   make                                      → compile GenerateCounterpoint
#   make generate COMBO="2 1"                 → génère un contrepoint (CF=1 par défaut)
#   make generate COMBO="2 1" CF=3            → avec cantus firmus n°3
#   make generate COMBO="3 2 5" CF=2 TIMEOUT=300000 STAGNATION=120000
#   make so                                   → compile la lib dynamique libgecode.so
#   make clean                                → supprime les binaires compilés
#   make clean_results                        → supprime résultats (2voices/3voices/4voices)
#   make clean_all                            → tout supprimer (binaires + résultats)
#
# Pour lancer des campagnes ou utiliser l'interface interactive :
#   ../fuxcp                                  → menu interactif (depuis FuxCP/)
#   ../fuxcp campaign <id>                    → lance une campagne (config/campaigns.csv)
# =============================================================

# === DOSSIERS ===
COMPILED_DIR = ../compiled
RESULTS_DIR  = ../../results
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
	src/ConfigLoader.cpp \
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
	@mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

# =============================================================
# GÉNÉRATEUR DE CONTREPOINTS (binaire principal)
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

# =============================================================
# NETTOYAGE
# =============================================================

clean:
	rm -f $(COMPILED_DIR)/GenerateCounterpoint
	rm -rf $(COMPILED_DIR)/CounterpointProblems $(COMPILED_DIR)/Parts
	rm -f $(COMPILED_DIR)/*.o
	rm -f $(TARGET)

clean_results:
	rm -rf $(RESULTS_DIR)/2voices $(RESULTS_DIR)/3voices $(RESULTS_DIR)/4voices

clean_all: clean clean_results
	rm -rf $(COMPILED_DIR)

.PHONY: all so compile_generate_counterpoint generate \
	clean clean_results clean_all
