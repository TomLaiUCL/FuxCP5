#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include "../headers/Midi.hpp"


struct MidiEvent {
    uint32_t tick;
    uint8_t status;
    uint8_t data1;
    uint8_t data2;

    bool operator<(const MidiEvent& other) const {
        if (tick != other.tick) return tick < other.tick;
        return status < other.status; // NoteOff avant NoteOn au même tick
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

std::vector<int> extract_last_voice_notes(CounterpointProblem* best,
                                            vector<Species> spList,
                                            size_t cfSize) {
    auto arr = best->getSolutionArray();
    std::vector<int> out;
    int n_voices = spList.size();
    vector<size_t> voiceSize = {cfSize, cfSize*2, cfSize*4, cfSize, cfSize*4};

    int start = 0;
    if (n_voices == 3) start = voiceSize.at(spList.at(0));
    else if (n_voices == 4) start = voiceSize.at(spList.at(0)) + voiceSize.at(spList.at(1));

    out.reserve(arr.size() - start);
    for (int i = start; i < arr.size(); ++i) out.push_back(arr[i].val());
    return out;
}

void saveMidi(const std::string& filename, 
              const std::vector<int>& cantusFirmus, 
              const std::vector<int>& raw_solution,
              Species species) {
    
    std::vector<int> solution = raw_solution;
    const uint32_t PPQ = 480; // Pulses Per Quarter Note
    uint32_t rondeDur = PPQ * 4;
    uint32_t cpDur;

    switch(species) {
        case SECOND_SPECIES: cpDur = rondeDur / 2; break;
        case THIRD_SPECIES:  cpDur = rondeDur / 4; break;
        case FOURTH_SPECIES: 
            cpDur = rondeDur;
            solution = smoothen4th(raw_solution);
            break;
        case FIFTH_SPECIES:  cpDur = rondeDur / 4; break;
        default:             cpDur = rondeDur;     break;
    }

    std::vector<MidiEvent> events;

    for (size_t i = 0; i < cantusFirmus.size(); ++i) {
        uint32_t start = i * rondeDur;
        events.push_back({start, 0x90, (uint8_t)cantusFirmus[i], 64});
        events.push_back({start + rondeDur, 0x80, (uint8_t)cantusFirmus[i], 0});
    }

    if (species == FOURTH_SPECIES) {
        size_t i;
        for (i = 0; i < solution.size()-1; ++i) {
            uint32_t start = i * cpDur + cpDur/2;
            events.push_back({start, 0x90, (uint8_t)solution[i], 80});
            events.push_back({start + cpDur, 0x80, (uint8_t)solution[i], 0});
        }
        // last note
        uint32_t start = i * cpDur;
        events.push_back({start, 0x90, (uint8_t)solution[i], 80});
        events.push_back({start + cpDur, 0x80, (uint8_t)solution[i], 0});
    }
    else {
        for (size_t i = 0; i < solution.size(); ++i) {
            uint32_t start = i * cpDur;
            events.push_back({start, 0x90, (uint8_t)solution[i], 80});
            events.push_back({start + cpDur, 0x80, (uint8_t)solution[i], 0});
        }
    }

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

    trackData.push_back(0x00);
    trackData.push_back(0xFF); trackData.push_back(0x2F); trackData.push_back(0x00);

    std::ofstream file(filename, std::ios::binary);
    
    file << "MThd";
    uint32_t hLen = __builtin_bswap32(6);
    file.write((char*)&hLen, 4);
    uint16_t format = __builtin_bswap16(0);
    file.write((char*)&format, 2);
    uint16_t ntrks = __builtin_bswap16(1);
    file.write((char*)&ntrks, 2);
    uint16_t division = __builtin_bswap16(PPQ);
    file.write((char*)&division, 2);

    file << "MTrk";
    uint32_t tLen = __builtin_bswap32(trackData.size());
    file.write((char*)&tLen, 4);
    file.write((char*)trackData.data(), trackData.size());

    file.close();
}