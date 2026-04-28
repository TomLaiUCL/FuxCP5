// ConfigLoader.hpp
// Chargement des configurations CSV (cantus_firmus, presets, campaigns).
// Toutes les valeurs autrefois hardcodées dans GenerateCounterpoint.cpp / Generations.cpp
// sont désormais externalisées dans FuxCP/config/*.csv.

#ifndef FUXCP_CONFIG_LOADER_HPP
#define FUXCP_CONFIG_LOADER_HPP

#include <string>
#include <vector>
#include <map>

struct CFEntry {
    int id;
    std::vector<int> notes;  // notes MIDI du cantus firmus (consolidées dans le CSV)
    std::string mode;        // ionien|dorien|...|auto
    std::string tonalite;
    std::string description;
};

struct PresetEntry {
    std::string name;
    std::vector<int> melodic_params;   // 8 entiers
    std::vector<int> general_params;   // 8 entiers
    std::vector<int> specific_params;  // 7 entiers
    std::vector<int> importance;       // 14 entiers
    int borrow_mode;
    std::string description;
};

struct CampaignRow {
    std::string campaign_id;
    std::vector<int> cf_ids;
    int n_voices;
    std::vector<int> species;          // espèces de contrepoint (sans le CF)
    std::vector<int> v_types;          // peut être vide
    std::string preset;
    int timeout_ms;
    int stagnation_ms;
    std::string output_subdir;
    std::string description;
};

class ConfigLoader {
public:
    // Localise le dossier config/ (cherche ../config/ depuis le CWD courant).
    static std::string find_config_dir();

    // Lecture des trois CSV (chemin = dossier config).
    static std::map<int, CFEntry>     load_cantus_firmus(const std::string& config_dir);
    static std::map<std::string, PresetEntry> load_presets(const std::string& config_dir);
    static std::vector<CampaignRow>   load_campaigns(const std::string& config_dir);

    // Helpers
    static std::vector<int> parse_int_list(const std::string& s, char sep = ' ');
    static std::vector<std::string> split(const std::string& s, char sep);
    static std::string trim(const std::string& s);
};

#endif
