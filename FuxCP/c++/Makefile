# === DOSSIERS DE SORTIE ===
# Dossier pour les fichiers objets compilés
COMPILED_DIR = ../compiled
# Dossier pour les résultats
RESULTS_DIR = ../results
# Dossier pour les bibliothèques
OUT_DIR = ../sources

# Créer les dossiers si nécessaire
$(shell mkdir -p $(COMPILED_DIR))
$(shell mkdir -p $(RESULTS_DIR))

# Source directory
SRC_DIR = src

# Find all .cpp files recursively in the SRC_DIR
SRCS = $(shell find $(SRC_DIR) -name '*.cpp')

# Object files - placés dans COMPILED_DIR en préservant la structure
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(COMPILED_DIR)/%.o, $(SRCS))

# Compiler flags
CXX = g++
CXXFLAGS = -std=c++11 -O3 -ffast-math -fPIC
CXXFLAGS_DEBUG = -g -std=c++11
LDFLAGS = -shared
LIBS = -lgecodedriver -lgecodeflatzinc -lgecodefloat -lgecodeint -lgecodekernel -lgecodeminimodel -lgecodesearch -lgecodeset -lgecodesupport
LIBS_MINIMAL = -lgecodekernel -lgecodesearch -lgecodeint -lgecodedriver -lgecodeminimodel -lgecodesupport

# Target
TARGET = $(OUT_DIR)/libgecode.so

# Default target
all: so dylib

# === COMPILATION DES OBJETS ===

# Créer les sous-dossiers dans Compiled si nécessaire
$(COMPILED_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(COMPILED_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === BIBLIOTHÈQUE DYNAMIQUE LINUX ===
so: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LIBS)

# === MACOS ===

run_mac_os:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) -std=c++11 -F/Library/Frameworks -c main.cpp -o $(COMPILED_DIR)/main.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Utilities.cpp -o $(COMPILED_DIR)/Utilities.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Voice.cpp -o $(COMPILED_DIR)/Voice.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Stratum.cpp -o $(COMPILED_DIR)/Stratum.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/CounterpointUtils.cpp -o $(COMPILED_DIR)/CounterpointUtils.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/CounterpointProblems/CounterpointProblem.cpp -o $(COMPILED_DIR)/CounterpointProblem.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Parts/Part.cpp -o $(COMPILED_DIR)/Part.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Parts/FirstSpeciesCounterpoint.cpp -o $(COMPILED_DIR)/FirstSpeciesCounterpoint.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Parts/SecondSpeciesCounterpoint.cpp -o $(COMPILED_DIR)/SecondSpeciesCounterpoint.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Parts/ThirdSpeciesCounterpoint.cpp -o $(COMPILED_DIR)/ThirdSpeciesCounterpoint.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Parts/FourthSpeciesCounterpoint.cpp -o $(COMPILED_DIR)/FourthSpeciesCounterpoint.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Parts/FifthSpeciesCounterpoint.cpp -o $(COMPILED_DIR)/FifthSpeciesCounterpoint.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/Parts/CantusFirmus.cpp -o $(COMPILED_DIR)/CantusFirmus.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/CounterpointProblems/TwoVoiceCounterpoint.cpp -o $(COMPILED_DIR)/TwoVoiceCounterpoint.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/CounterpointProblems/ThreeVoiceCounterpoint.cpp -o $(COMPILED_DIR)/ThreeVoiceCounterpoint.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/CounterpointProblems/FourVoiceCounterpoint.cpp -o $(COMPILED_DIR)/FourVoiceCounterpoint.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/fuxTest.cpp -o $(COMPILED_DIR)/fuxTest.o
	$(CXX) -std=c++11 -F/Library/Frameworks -c src/constraints.cpp -o $(COMPILED_DIR)/constraints.o
	$(CXX) -std=c++11 -F/Library/Frameworks -framework gecode -o $(COMPILED_DIR)/Main $(COMPILED_DIR)/*.o
	$(COMPILED_DIR)/Main

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

SOURCES = main.cpp src/Utilities.cpp src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp \
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

compile_and_sacha:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) main.cpp src/Utilities.cpp src/Parts/Midi.cpp src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp \
	    src/CounterpointProblems/CounterpointProblem.cpp src/Parts/Part.cpp src/Parts/FirstSpeciesCounterpoint.cpp \
	    src/Parts/SecondSpeciesCounterpoint.cpp src/Parts/ThirdSpeciesCounterpoint.cpp src/Parts/FourthSpeciesCounterpoint.cpp \
	    src/Parts/FifthSpeciesCounterpoint.cpp src/Parts/CantusFirmus.cpp src/CounterpointProblems/TwoVoiceCounterpoint.cpp \
	    src/CounterpointProblems/ThreeVoiceCounterpoint.cpp src/CounterpointProblems/FourVoiceCounterpoint.cpp \
	    src/fuxTest.cpp src/constraints.cpp src/figureTests.cpp -o $(COMPILED_DIR)/Main $(LIBS_MINIMAL)
	$(COMPILED_DIR)/Main sacha

# === TESTS ===

test: compile_for_debug
	$(COMPILED_DIR)/Main all

# === TEST 3ÈME ESPÈCE ===

THIRD_SPECIES_TEST_SOURCES = src/ThirdSpeciesTest.cpp src/Utilities.cpp src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp \
	    src/CounterpointProblems/CounterpointProblem.cpp src/Parts/Part.cpp src/Parts/FirstSpeciesCounterpoint.cpp \
	    src/Parts/SecondSpeciesCounterpoint.cpp src/Parts/ThirdSpeciesCounterpoint.cpp src/Parts/FourthSpeciesCounterpoint.cpp \
	    src/Parts/FifthSpeciesCounterpoint.cpp src/Parts/CantusFirmus.cpp src/CounterpointProblems/TwoVoiceCounterpoint.cpp \
	    src/CounterpointProblems/ThreeVoiceCounterpoint.cpp src/CounterpointProblems/FourVoiceCounterpoint.cpp \
	    src/constraints.cpp

compile_3sp_test:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) $(THIRD_SPECIES_TEST_SOURCES) -o $(COMPILED_DIR)/TestTroisiemeEspece $(LIBS_MINIMAL)

test_3sp_all: compile_3sp_test
	$(COMPILED_DIR)/TestTroisiemeEspece all

test_3sp_quick: compile_3sp_test
	$(COMPILED_DIR)/TestTroisiemeEspece quick

test_3sp_full: compile_3sp_test
	$(COMPILED_DIR)/TestTroisiemeEspece full3sp

test_3sp_mixed: compile_3sp_test
	$(COMPILED_DIR)/TestTroisiemeEspece mixed

test_3sp_specific: compile_3sp_test
	$(COMPILED_DIR)/TestTroisiemeEspece $(COMBO)

# === TEST DIAGNOSTIC ===

DIAGNOSTIC_TEST_SOURCES = src/DiagnosticTest.cpp src/Utilities.cpp src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp \
	    src/CounterpointProblems/CounterpointProblem.cpp src/Parts/Part.cpp src/Parts/FirstSpeciesCounterpoint.cpp \
	    src/Parts/SecondSpeciesCounterpoint.cpp src/Parts/ThirdSpeciesCounterpoint.cpp src/Parts/FourthSpeciesCounterpoint.cpp \
	    src/Parts/FifthSpeciesCounterpoint.cpp src/Parts/CantusFirmus.cpp src/CounterpointProblems/TwoVoiceCounterpoint.cpp \
	    src/CounterpointProblems/ThreeVoiceCounterpoint.cpp src/CounterpointProblems/FourVoiceCounterpoint.cpp \
	    src/constraints.cpp

compile_diag:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) $(DIAGNOSTIC_TEST_SOURCES) -o $(COMPILED_DIR)/TestDiagnostic $(LIBS_MINIMAL)

diag_all: compile_diag
	$(COMPILED_DIR)/TestDiagnostic all

diag_screen: compile_diag
	$(COMPILED_DIR)/TestDiagnostic screen

diag: compile_diag
	$(COMPILED_DIR)/TestDiagnostic $(COMBO)


# === GENERATEUR CONTREPOINT N VOIX ===

GENERATE_COUNTERPOINT_SOURCES = src/GenerateCounterpoint.cpp src/Utilities.cpp src/Voice.cpp src/Stratum.cpp src/CounterpointUtils.cpp \
	src/CounterpointProblems/CounterpointProblem.cpp src/Parts/Part.cpp src/Parts/FirstSpeciesCounterpoint.cpp \
	src/Parts/SecondSpeciesCounterpoint.cpp src/Parts/ThirdSpeciesCounterpoint.cpp src/Parts/FourthSpeciesCounterpoint.cpp \
	src/Parts/FifthSpeciesCounterpoint.cpp src/Parts/CantusFirmus.cpp src/CounterpointProblems/TwoVoiceCounterpoint.cpp \
	src/CounterpointProblems/ThreeVoiceCounterpoint.cpp src/CounterpointProblems/FourVoiceCounterpoint.cpp \
	src/Parts/Midi.cpp src/constraints.cpp

compile_generate_counterpoint:
	@mkdir -p $(COMPILED_DIR)
	$(CXX) $(CXXFLAGS_DEBUG) $(GENERATE_COUNTERPOINT_SOURCES) -o $(COMPILED_DIR)/GenerateCounterpoint $(LIBS_MINIMAL)

generate_counterpoint: compile_generate_counterpoint
	$(COMPILED_DIR)/GenerateCounterpoint $(COMBO)

generate_all_counterpoints: compile_generate_counterpoint
	@for nb_voix in 2 3 4; do \
		if [ $$nb_voix -eq 2 ]; then \
			for sp1 in 1 2 3; do \
				file="$(RESULTS_DIR)/$${nb_voix}voices_1_$${sp1}.txt"; \
				if [ ! -f $$file ]; then \
					echo "Génération pour $$nb_voix voix, espèce $$sp1"; \
					$(COMPILED_DIR)/GenerateCounterpoint $$nb_voix $$sp1; \
				else \
					echo "Déjà existant: $$file"; \
				fi; \
			done; \
		elif [ $$nb_voix -eq 3 ]; then \
			for sp1 in 1 2 3; do \
				for sp2 in 1 2 3; do \
					file="$(RESULTS_DIR)/$${nb_voix}voices_1_$${sp1}_$${sp2}.txt"; \
					if [ ! -f $$file ]; then \
						echo "Génération pour $$nb_voix voix, espèces $$sp1 $$sp2"; \
						$(COMPILED_DIR)/GenerateCounterpoint $$nb_voix $$sp1 $$sp2; \
					else \
						echo "Déjà existant: $$file"; \
					fi; \
				done; \
			done; \
		else \
			for sp1 in 1 2 3; do \
				for sp2 in 1 2 3; do \
					for sp3 in 1 2 3; do \
						file="$(RESULTS_DIR)/$${nb_voix}voices_1_$${sp1}_$${sp2}_$${sp3}.txt"; \
						if [ ! -f $$file ]; then \
							echo "Génération pour $$nb_voix voix, espèces $$sp1 $$sp2 $$sp3"; \
							$(COMPILED_DIR)/GenerateCounterpoint $$nb_voix $$sp1 $$sp2 $$sp3; \
						else \
							echo "Déjà existant: $$file"; \
						fi; \
					done; \
				done; \
			done; \
		fi; \
	done

# === NETTOYAGE ===

clean:
	rm -rf $(COMPILED_DIR)/*.o $(COMPILED_DIR)/*/
	rm -f $(COMPILED_DIR)/Main $(COMPILED_DIR)/TestTroisiemeEspece $(COMPILED_DIR)/TestDiagnostic $(COMPILED_DIR)/SolveurAdaptatif
	rm -f $(TARGET) $(OUT_DIR)/libgecode.dylib

clean_all: clean
	rm -rf $(COMPILED_DIR)

.PHONY: all so dylib run_mac_os run_linux compile_for_debug compile_and_bryce compile_and_sacha test \
	    compile_3sp_test test_3sp_all test_3sp_quick test_3sp_full test_3sp_mixed test_3sp_specific \
	    compile_diag diag_all diag_screen diag compile_adaptive adaptive_all adaptive clean clean_all \
	    compile_generate_counterpoint generate_counterpoint generate_all_counterpoints