#ifndef CLASSIFIER_LOOKUPING_H
#define CLASSIFIER_LOOKUPING_H

#include <cstdint>
#include <vector>
#include "../include/RFC_structure.h"

std::vector<int> Lookup(std::vector<ChunkTable> phase0_chunks, std::vector<ChunkTable> phase1_chunks,
    ChunkTable final_phase, PacketSet& packet_set);


#endif // CLASSIFIER_LOOKUPING_H