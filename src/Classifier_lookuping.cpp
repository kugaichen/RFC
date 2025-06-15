#include <cstdint>
#include <vector>
#include "../include/RFC_structure.h"

std::vector<int> Lookup(std::vector<ChunkTable> phase0_chunks, std::vector<ChunkTable> phase1_chunks,
    ChunkTable final_phase, PacketSet& packet_set){
        unsigned int value_phase0[6];
        unsigned int key_phase1[2];
        unsigned int value_phase1[2];
        unsigned int key_final;

        std::vector<int> LookupResult(packet_set.num_packets);

        for (int pkt = 0; pkt < packet_set.num_packets; pkt++){

            for (size_t c1=0; c1<6; c1++){
                value_phase0[c1] = phase0_chunks[c1].item[packet_set.packets_arr[pkt]->dim[c1]];  
            }

            key_phase1[0] = value_phase0[0]*phase0_chunks[1].matched_EqIDTable->item_num * phase0_chunks[5].matched_EqIDTable->item_num
                + value_phase0[1]*phase0_chunks[5].matched_EqIDTable->item_num
                + value_phase0[5];
            
            key_phase1[1] = value_phase0[2]*phase0_chunks[3].matched_EqIDTable->item_num * phase0_chunks[4].matched_EqIDTable->item_num
                + value_phase0[3]*phase0_chunks[4].matched_EqIDTable->item_num
                + value_phase0[4];

            value_phase1[0] = phase1_chunks[0].item[key_phase1[0]];
            value_phase1[1] = phase1_chunks[1].item[key_phase1[1]];

            key_final = value_phase1[0] * phase1_chunks[1].matched_EqIDTable->item_num + value_phase1[1];
            LookupResult[pkt] = final_phase.item[key_final];
        }

    return LookupResult;

}