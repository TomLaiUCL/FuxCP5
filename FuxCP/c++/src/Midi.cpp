#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <utility>
#include "../headers/Midi.hpp"


struct MidiEvent {
    uint32_t tick;
    uint8_t status;
    uint8_t data1;
    uint8_t data2;

    bool operator<(const MidiEvent& other) const {
        if (tick != other.tick) return tick < other.tick;
        // NoteOff (0x8x) avant NoteOn (0x9x) au même tick
        if ((status & 0xF0) != (other.status & 0xF0)) return (status & 0xF0) < (other.status & 0xF0);
        return status < other.status;
    }
};

void writeVLQ(std::vector<uint8_t>& buffer, uint32_t value) {
    uint32_t buffer_val = value & 0x7F;
    while ((value >>= 7) > 0) {
        buffer_val <<= 8;
        buffer_val |= 0x80;
        buffer_val += (value & 0x7F);
    }
    while (true) {
        buffer.push_back(buffer_val & 0xFF);
        if (buffer_val & 0x80) buffer_val >>= 8;
        else break;
    }
}

static std::vector<int> smoothen4th(const std::vector<int>& sol) {
    // Deduplicate pairs
    std::vector<int> out;
    out.reserve((sol.size() + 1) / 2);

    for (size_t i = 0; i < sol.size(); ) {
        int v = sol[i];
        out.push_back(v);
        i += 1;
        // skip one if duplicated
        if (i < sol.size() && sol[i] == v) i += 1;
    }
    return out;
}

std::vector<int> extract_notes(CounterpointProblem* best,
                                            vector<Species> spList,
                                            size_t cfSize) {
    auto arr = best->getSolutionArray();
    std::vector<int> out;
    out.reserve(arr.size());
    for (int i = 0; i < arr.size(); ++i) out.push_back(arr[i].val());
    return out;
}

int branchingNotesSize(Species sp, int cfSize) {
    switch(sp) {
        case FIRST_SPECIES:  return cfSize;
        case SECOND_SPECIES: return 2 * cfSize - 1;
        case THIRD_SPECIES:  return 4 * cfSize - 3;
        case FOURTH_SPECIES: return 2 * cfSize - 2;
        case FIFTH_SPECIES:  return 4 * cfSize - 3;
        default:             return cfSize;
    }
}

// Ajoute les événements MIDI d'une voix à la liste
static void addVoiceEvents(std::vector<MidiEvent>& events,
                           const std::vector<int>& raw_notes,
                           Species species,
                           uint8_t channel,
                           uint8_t velocity,
                           uint32_t rondeDur) {
    std::vector<int> notes = raw_notes;
    uint32_t cpDur;

    switch(species) {
        case SECOND_SPECIES: cpDur = rondeDur / 2; break;
        case THIRD_SPECIES:  cpDur = rondeDur / 4; break;
        case FOURTH_SPECIES:
            cpDur = rondeDur;
            notes = smoothen4th(raw_notes);
            break;
        case FIFTH_SPECIES:  cpDur = rondeDur / 4; break;
        case CANTUS_FIRMUS:  cpDur = rondeDur;     break;
        default:             cpDur = rondeDur;     break; // FIRST_SPECIES
    }

    uint8_t noteOn  = 0x90 | (channel & 0x0F);
    uint8_t noteOff = 0x80 | (channel & 0x0F);

    if (species == FOURTH_SPECIES && notes.size() > 1) {
        size_t i;
        for (i = 0; i < notes.size() - 1; ++i) {
            uint32_t start = i * cpDur + cpDur / 2;
            events.push_back({start, noteOn, (uint8_t)notes[i], velocity});
            events.push_back({start + cpDur, noteOff, (uint8_t)notes[i], 0});
        }
        // Dernière note sans décalage
        uint32_t start = i * cpDur;
        events.push_back({start, noteOn, (uint8_t)notes[i], velocity});
        events.push_back({start + cpDur, noteOff, (uint8_t)notes[i], 0});
    } else {
        for (size_t i = 0; i < notes.size(); ++i) {
            uint32_t start = i * cpDur;
            events.push_back({start, noteOn, (uint8_t)notes[i], velocity});
            events.push_back({start + cpDur, noteOff, (uint8_t)notes[i], 0});
        }
    }
}

// Construit les données binaires d'une piste MIDI à partir de ses événements
static std::vector<uint8_t> buildTrackData(std::vector<MidiEvent>& events) {
    std::sort(events.begin(), events.end());

    std::vector<uint8_t> trackData;
    uint32_t lastTick = 0;

    for (const auto& e : events) {
        writeVLQ(trackData, e.tick - lastTick);
        trackData.push_back(e.status);
        trackData.push_back(e.data1);
        trackData.push_back(e.data2);
        lastTick = e.tick;
    }

    // End of Track
    trackData.push_back(0x00);
    trackData.push_back(0xFF); trackData.push_back(0x2F); trackData.push_back(0x00);
    return trackData;
}

// Écrit un fichier MIDI Format 1 (une piste par voix)
static void writeMidiFile(const std::string& filename,
                          std::vector<std::vector<MidiEvent>>& tracks,
                          uint16_t PPQ) {
    std::ofstream file(filename, std::ios::binary);

    file << "MThd";
    uint32_t hLen = __builtin_bswap32(6);
    file.write((char*)&hLen, 4);
    uint16_t format = __builtin_bswap16(1);  // Format 1 : pistes séparées
    file.write((char*)&format, 2);
    uint16_t ntrks = __builtin_bswap16((uint16_t)tracks.size());
    file.write((char*)&ntrks, 2);
    uint16_t division = __builtin_bswap16(PPQ);
    file.write((char*)&division, 2);

    for (auto& track : tracks) {
        auto data = buildTrackData(track);
        file << "MTrk";
        uint32_t tLen = __builtin_bswap32(data.size());
        file.write((char*)&tLen, 4);
        file.write((char*)data.data(), data.size());
    }

    file.close();
}

void saveMidiMultiVoice(const std::string& filename,
                        const std::vector<int>& cantusFirmus,
                        const std::vector<std::pair<std::vector<int>, Species>>& voices) {
    const uint32_t PPQ = 480;
    uint32_t rondeDur = PPQ * 4;

    // Une piste par voix : piste 0 = CF, pistes 1..n = contrepoint
    std::vector<std::vector<MidiEvent>> tracks(1 + std::min(voices.size(), (size_t)3));

    // Piste 0 : Cantus Firmus
    addVoiceEvents(tracks[0], cantusFirmus, CANTUS_FIRMUS, 0, 64, rondeDur);

    // Pistes 1, 2, 3 : voix de contrepoint
    for (size_t v = 0; v < voices.size() && v < 3; ++v) {
        addVoiceEvents(tracks[v + 1], voices[v].first, voices[v].second,
                       (uint8_t)(v + 1), 80, rondeDur);
    }

    writeMidiFile(filename, tracks, PPQ);
}

void saveMidi(const std::string& filename, 
              const std::vector<int>& cantusFirmus, 
              const std::vector<int>& raw_solution,
              Species species) {
    
    const uint32_t PPQ = 480;
    uint32_t rondeDur = PPQ * 4;

    std::vector<std::vector<MidiEvent>> tracks(2);

    // Piste 0 : Cantus Firmus
    addVoiceEvents(tracks[0], cantusFirmus, CANTUS_FIRMUS, 0, 64, rondeDur);

    // Piste 1 : Contrepoint
    addVoiceEvents(tracks[1], raw_solution, species, 1, 80, rondeDur);

    writeMidiFile(filename, tracks, PPQ);
}



void saveMidiGeneral(const std::string& filename,
                        const std::vector<int>& cantusFirmus,
                        const std::vector<int>& raw_solution,
                        const std::vector<Species>& spList){

    vector<pair<vector<int>, Species>> voices;

    int n_counterpoints = spList.size();
    int cf_size = cantusFirmus.size();
    int offset = 0;
    for (int voice = 0; voice < n_counterpoints; ++voice) {
        Species species = spList.at(voice);
        int sz = branchingNotesSize(species, cf_size);
        vector<int> voice_notes(raw_solution.begin() + offset, raw_solution.begin() + offset + sz);
        voices.push_back({voice_notes, species});
        offset += sz;
    }

    saveMidiMultiVoice(filename, cantusFirmus, voices);
}