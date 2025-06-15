#ifndef RFC_STRUCTURE_H
#define RFC_STRUCTURE_H


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <bitset>
#include <stdio.h>
#include <stdexcept>
#include <cstdint>


// Filter Definition

struct AddressRange
{
    std::vector<unsigned short> start_address; //[high16, low16, mask]
    std::vector<unsigned short> end_address;   //[high16, low16]
};

class FilterLine{
public:
    int priority;
    std::vector<std::string> fields;
    std::vector<std::vector<unsigned short>> dim;

    FilterLine(int line_param, const std::vector<std::string>& fields_param):
        priority(line_param),fields(fields_param){
            dim.resize(6);
            dim[0].resize(2);
            dim[1].resize(3);
            dim[2].resize(2);
            dim[3].resize(3);
            dim[4].resize(2);
            dim[5].resize(2);
        }
    
    // extracr fields'content to dim
    void fields_extract();

};

class FilterSet{
public:
    unsigned int  num_filters;
    std::vector<std::shared_ptr<FilterLine>> filters_arr;

    FilterSet():num_filters(0){}

    void add_filter(std::unique_ptr<FilterLine> filter_line);
       
};


class PacketLine{
public:
    std::vector<std::string> fields;
    std::vector<unsigned short> dim;
    std::string except_rule;

    PacketLine(const std::vector<std::string>& fields_param):
        fields(fields_param){
            dim.resize(6);
        }
    
    void fields_extract();
};


class PacketSet{
public:
    int num_packets;
    std::vector<std::shared_ptr<PacketLine>> packets_arr;

    PacketSet():num_packets(0){}

    void add_packets(std::unique_ptr<PacketLine> packet_line);   
};

class ChunkTable;

class EqID {
public:
    std::uint32_t eqid;
    std::vector<std::uint32_t> cbm;
    EqID* next;

    EqID(std::uint32_t eqid_param, std::uint32_t CBM_SIZE):eqid(eqid_param),
        cbm(CBM_SIZE,0),next(nullptr){}

};


class EqIDTable{
public:
    std::uint32_t item_num;
    EqID* head;
    EqID* tail;
    ChunkTable* matched_ChunkTable;

    EqIDTable(std::uint32_t item_num_param = 0): item_num(item_num_param),
        head(nullptr),tail(nullptr),matched_ChunkTable(nullptr){}

    void add_EqID(EqID* real_EqID){
        item_num ++;
        if (head == nullptr){
            head = real_EqID;
            tail = real_EqID;
        }
        else{
            tail->next = real_EqID;
            tail = real_EqID;
        }
    }

};

class ChunkTable{
public:
    std::uint32_t chunk_num;
    std::vector<std::uint32_t> item;
    EqIDTable* matched_EqIDTable;

    ChunkTable(std::uint32_t chunk_num_param = 0):chunk_num(chunk_num_param){
        item.resize(chunk_num);
    }

    void match_EqIDTable(EqIDTable* real_EqIDTable){
        real_EqIDTable->matched_ChunkTable = this;
        matched_EqIDTable = real_EqIDTable;
    }
};

struct RFC_Chunks
{
    std::vector<ChunkTable> phase0_chunks; 
    std::vector<ChunkTable> phase1_chunks; 
    // ChunkTable phase2_chunks; 
    ChunkTable final_chunks;
};



#endif // RFC_STRUCTURE_H