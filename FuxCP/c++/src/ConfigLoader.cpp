// ConfigLoader.cpp
// Implémentation du chargement des CSV de configuration.

#include "../headers/ConfigLoader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <cstdlib>

using namespace std;

// ---------------- Helpers ----------------

string ConfigLoader::trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

vector<string> ConfigLoader::split(const string& s, char sep) {
    vector<string> out;
    string cur;
    for (char c : s) {
        if (c == sep) { out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    out.push_back(cur);
    return out;
}

vector<int> ConfigLoader::parse_int_list(const string& s, char sep) {
    vector<int> out;
    string t = trim(s);
    if (t.empty()) return out;
    for (auto& part : split(t, sep)) {
        string p = trim(part);
        if (!p.empty()) out.push_back(atoi(p.c_str()));
    }
    return out;
}

static bool file_exists(const string& p) {
    struct stat st;
    return stat(p.c_str(), &st) == 0;
}

string ConfigLoader::find_config_dir() {
    // Cherche le dossier config/ dans plusieurs emplacements probables
    // (selon le CWD : FuxCP/c++/, FuxCP/, ou ailleurs).
    const vector<string> candidates = {
        "../config", "config", "../../config",
        "FuxCP/config", "../FuxCP/config"
    };
    for (auto& c : candidates) {
        if (file_exists(c + "/cantus_firmus.csv")) return c;
    }
    cerr << "[ConfigLoader] ATTENTION : dossier config/ introuvable, fallback ../config" << endl;
    return "../config";
}

// Renvoie true si la ligne doit être ignorée (commentaire ou vide)
static bool is_skippable(const string& line) {
    string t = ConfigLoader::trim(line);
    return t.empty() || t[0] == '#';
}

// Lit la 1re ligne non-skip (= header) puis renvoie chaque ligne CSV split par ','
// Note : pas de support des virgules intra-champ (les listes utilisent ' ' ou '+').
static vector<vector<string>> read_csv_rows(const string& path) {
    vector<vector<string>> rows;
    ifstream f(path);
    if (!f.is_open()) {
        cerr << "[ConfigLoader] Impossible d'ouvrir " << path << endl;
        return rows;
    }
    string line;
    bool header_seen = false;
    while (getline(f, line)) {
        if (is_skippable(line)) continue;
        if (!header_seen) { header_seen = true; continue; } // skip header
        rows.push_back(ConfigLoader::split(line, ','));
    }
    return rows;
}

// ---------------- Cantus firmus ----------------

map<int, CFEntry> ConfigLoader::load_cantus_firmus(const string& dir) {
    map<int, CFEntry> out;
    auto rows = read_csv_rows(dir + "/cantus_firmus.csv");
    for (auto& r : rows) {
        if (r.size() < 5) continue;
        CFEntry e;
        e.id          = atoi(trim(r[0]).c_str());
        e.notes       = parse_int_list(r[1], ' ');
        e.mode        = trim(r[2]);
        e.tonalite    = trim(r[3]);
        e.description = trim(r[4]);
        out[e.id] = e;
    }
    return out;
}

// ---------------- Presets ----------------

map<string, PresetEntry> ConfigLoader::load_presets(const string& dir) {
    map<string, PresetEntry> out;
    auto rows = read_csv_rows(dir + "/presets.csv");
    for (auto& r : rows) {
        if (r.size() < 7) continue;
        PresetEntry p;
        p.name             = trim(r[0]);
        p.melodic_params   = parse_int_list(r[1], ' ');
        p.general_params   = parse_int_list(r[2], ' ');
        p.specific_params  = parse_int_list(r[3], ' ');
        p.importance       = parse_int_list(r[4], ' ');
        p.borrow_mode      = atoi(trim(r[5]).c_str());
        p.description      = trim(r[6]);
        out[p.name] = p;
    }
    return out;
}

// ---------------- Campaigns ----------------

vector<CampaignRow> ConfigLoader::load_campaigns(const string& dir) {
    vector<CampaignRow> out;
    auto rows = read_csv_rows(dir + "/campaigns.csv");
    for (auto& r : rows) {
        if (r.size() < 10) continue;
        CampaignRow c;
        c.campaign_id   = trim(r[0]);
        c.cf_ids        = parse_int_list(r[1], '+');
        c.n_voices      = atoi(trim(r[2]).c_str());
        c.species       = parse_int_list(r[3], '+');
        c.v_types       = parse_int_list(r[4], '+');
        c.preset        = trim(r[5]);
        c.timeout_ms    = atoi(trim(r[6]).c_str());
        c.stagnation_ms = atoi(trim(r[7]).c_str());
        c.output_subdir = trim(r[8]);
        c.description   = trim(r[9]);
        out.push_back(c);
    }
    return out;
}
