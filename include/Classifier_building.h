#ifndef CLASSIFIER_BUILDING_H
#define CLASSIFIER_BUILDING_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <bitset>
#include <stdio.h>
#include <stdexcept>

#include "RFC_structure.h"

void Read_filter_file(const std::string& filename, FilterSet& filter_set);

void Load_packets(const std::string& filename, PacketSet& packet_set);

void Set_cbm(std::uint32_t CBM_SIZE, std::vector<std::uint32_t>* tempcbm, 
    unsigned int filter_index, bool value, unsigned int filter_num);

bool Compare_CBM(std::vector<std::uint32_t>* cbm_a, std::vector<std::uint32_t>* cbm_b);

int Search_CBM(EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>* tempcbm);

int Add_cbm_EqIDTable(EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>* tempcbm, std::uint32_t SIZE);

int Find_rule(std::uint32_t CBM_SIZE, std::vector<std::uint32_t>* tempcbm, unsigned int filter_num);

std::vector<ChunkTable> Creat_Phase0(std::uint32_t CBM_SIZE, FilterSet& filter_set);

std::vector<ChunkTable> Creat_Phase1(std::uint32_t CBM_SIZE, std::vector<ChunkTable> phase0_chunks);

ChunkTable Creat_phasefinal(std::uint32_t CBM_SIZE, std::vector<ChunkTable> phase1_chunks, FilterSet& filter_set);

RFC_Chunks Building_Classfier(std::uint32_t CBM_SIZE, FilterSet& filter_set);

#endif // CLASSIFIER_BUILDING_H