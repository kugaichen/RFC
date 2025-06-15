#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <bitset>
#include <stdio.h>
#include <stdexcept>

#include "../include/RFC_structure.h" 

AddressRange Read_address_range(const std::string& address_field){
    if (address_field.empty()){
        throw std::invalid_argument("address field is NUll");
    }

    std::regex address_regex(R"((\d+)\.(\d+)\.(\d+)\.(\d+)(?:/(\d+))?)");
    std::smatch match_result;

    if (!std::regex_match(address_field, match_result, address_regex)){
        throw std::invalid_argument("address match error");
    }

    if (match_result.size() != 6) {
        throw std::invalid_argument("unexpected number of matches");
    }

    // copy the match content to address_part for next dealing 
    unsigned short address_parts[5] = {0};
    for (size_t i = 1; i < match_result.size(); ++i){

        if (!match_result[i].str().empty()){
            address_parts[i-1] = static_cast<unsigned short>(std::stoi(match_result[i].str()));
        }
    }

    int  mask = 32 - address_parts[4];
    int  mask_8 = mask / 8;
    int  mask_less8 = mask % 8;

    unsigned short addr_range[4] = {0};
    for (int i = 0; i < 4; ++i){
        addr_range[i] = (i < 4 - mask_8) ? address_parts[i] : 0;
    }

    if (mask_less8 != 0){
        int mask_index = (1 << mask_less8) - 1;
        int mask_start = ~mask_index;
        addr_range[4 - mask_8 - 1] &= mask_start; 
    }

    //calculate the start_address
    std::vector<unsigned short> start_address(3,0);

    start_address[0] = (addr_range[0] << 8) | addr_range[1];
    start_address[1] = (addr_range[2] << 8) | addr_range[3];
    start_address[2] = mask;

    //calculate the end_address
    for (int i = 0; i < 4; ++i){
        addr_range[i] = (i < 4 - mask_8) ? address_parts[i] : 255;
    }

    if (mask_less8 != 0){
        int mask_index = (1 << mask_less8) - 1;
        addr_range[4 - mask_8 - 1] |= mask_index;
    }
    std::vector<unsigned short> end_address(2,0);
    end_address[0] = (addr_range[0] << 8) | addr_range[1];
    end_address[1] = (addr_range[2] << 8) | addr_range[3];

    return {start_address,end_address};

}


std::vector<unsigned short> Read_port_range(const std::string& port_field){
    if (port_field.empty()){
        throw std::invalid_argument("port field is NUll");
    }

    std::regex port_regex(R"((\d+)\s*:\s*(\d+))");
    std::smatch match_result;

    if (!std::regex_match(port_field, match_result, port_regex)){
        throw std::invalid_argument("port match error");
    }

    if (match_result.size() != 3) {
        throw std::invalid_argument("unexpected number of matches");
    }

    unsigned short port_parts[2] = {0};
    for (size_t i = 1; i < match_result.size(); ++i){

        if (!match_result[i].str().empty()){
            port_parts[i-1] = std::stoi(match_result[i].str());
        }
    }

    unsigned short start_port = port_parts[0];
    unsigned short end_port = port_parts[1];

    return {start_port,end_port};

}


void FilterLine::fields_extract(){
    AddressRange src_range = Read_address_range(fields[0]);
    AddressRange dst_range = Read_address_range(fields[1]);
    std::vector<unsigned short> src_port_range = Read_port_range(fields[2]);
    std::vector<unsigned short> dst_port_range = Read_port_range(fields[3]);
    
    dim[0][0] = src_range.start_address[0];
    dim[0][1] = src_range.end_address[0];
    dim[1][2] = src_range.start_address[2];
    dim[1][0] = src_range.start_address[1];
    dim[1][1] = src_range.end_address[1];

    dim[2][0] = dst_range.start_address[0];
    dim[2][1] = dst_range.end_address[0];
    dim[3][2] = dst_range.start_address[2];
    dim[3][0] = dst_range.start_address[1];
    dim[3][1] = dst_range.end_address[1];

    dim[4] = src_port_range;
    dim[5] = dst_port_range;
}



void FilterSet::add_filter(std::unique_ptr<FilterLine> filter_line){  
    filters_arr.push_back(std::move(filter_line));
    num_filters++;
    
}



// Reading filter rules
void Read_filter_file(const std::string& filename, FilterSet& filter_set){
    std::ifstream file(filename);
    if (!file){
        std::cerr << "could not read filter file.\n";
        return;
    }
    std::string line;
    unsigned int line_num = 0;
    while (std::getline(file,line)){
        //test...
        // std::cout << line << std::endl;
        // read the filters' information line by line
        if (line.empty() || line[0] != '@') continue;           
        line = line.substr(1);  //remove the flag'@'
        //read line from file to fields
        std::vector<std::string> fields;
        size_t pos = 0;
        while((pos = line.find('\t')) != std::string::npos){               
            fields.push_back(line.substr(0,pos));
            line.erase(0, pos+1);
        }
        auto filter_line = std::make_unique<FilterLine>(line_num,fields);
        filter_line->fields_extract();

        //test...
        // for (const auto& item : filter_line->dim){
        //     std::cout << item << " ";
        // }


        filter_set.add_filter(std::move(filter_line));
        
        line_num ++;   
    }

    std::cout<<"Read Filters successfully! the number of Filters is: "<<filter_set.num_filters<<std::endl;

}


// transform the interger to 32-bit binary
std::string bin_8(int value, int bits = 32){
    return std::bitset<32>(value).to_string();
}

std::vector<std::string> convert_binary(std::string content){
    unsigned long num = std::stoul(content);
    std::string binary_content = std::bitset<32>(num).to_string();
    std::string H_binary_content = binary_content.substr(0,16);
    std::string L_binary_content = binary_content.substr(16,16);

    // test...
    // std::cout << binary_content << std::endl;
    // std::cout << H_binary_content << std::endl;
    // std::cout << L_binary_content << std::endl;
    
    return {H_binary_content,L_binary_content};
}

unsigned short convert_to_ushort(const std::string& binary_str){
    std::bitset<16> bits(binary_str);
    unsigned long long ull_value = bits.to_ullong();

    return static_cast<unsigned short>(ull_value);
}


void PacketLine::fields_extract(){
    std::vector<std::string> src_addr = convert_binary(fields[0]);
    std::vector<std::string> dst_addr = convert_binary(fields[1]);

    std::vector<std::string*> real_fields = {&src_addr[0], &src_addr[1], &dst_addr[0], &dst_addr[1]};

    for(size_t i = 0; i<real_fields.size(); ++i){
        dim[i] = convert_to_ushort(*real_fields[i]);
    }

    dim[4] = static_cast<unsigned int>(std::stoi(fields[2]));
    dim[5] = static_cast<unsigned int>(std::stoi(fields[3]));

    except_rule =fields[6];

}


void PacketSet::add_packets(std::unique_ptr<PacketLine> packet_line){

    packets_arr.push_back(std::move(packet_line));
    num_packets++;
}

// Reading filter rules
void Load_packets(const std::string& filename, PacketSet& packet_set){ 
    std::ifstream file(filename);
    if (!file){
        std::cerr << "could not read packet file.\n";
        return;
    }

    std::string line;

    while (std::getline(file,line)){
        //read the packets' information line by line 

        //test...
        // std::cout << line << std::endl;
        
        std::vector<std::string> fields;
        size_t pos = 0;
        while((pos = line.find('\t')) != std::string::npos){               
            fields.push_back(line.substr(0,pos));
            line.erase(0, pos+1);
        }

        fields.push_back(line);

        auto packet_line = std::make_unique<PacketLine>(fields);
        packet_line->fields_extract();


        //test...
        // for (const auto& item : packet_line->dim){
        //     std::cout << item << " ";
        // }

        packet_set.add_packets(std::move(packet_line));

    }

    std::cout<<"Read Packets successfully! the number of Packets is: "<<packet_set.num_packets<<std::endl;

}

void Set_cbm(std::uint32_t CBM_SIZE, std::vector<std::uint32_t>* tempcbm, 
    unsigned int filter_index, bool value, unsigned int filter_num){

    unsigned int group,pos;
    unsigned int k = 1;

    group = CBM_SIZE - 1 - (filter_num -1 - filter_index)/32;
    pos = (filter_num -1 - filter_index) % 32 ;
    k <<= pos;
    if (value == true){
        (*tempcbm)[group] |= k;
    }
    else{
        k = ~k;
        (*tempcbm)[group] &= k;
    }
}

bool Compare_CBM(std::vector<std::uint32_t>* cbm_a, std::vector<std::uint32_t>* cbm_b){
    if( (cbm_a == nullptr) || (cbm_b == nullptr)){
        std::cerr<<"cbm_a or cbm_b Null error"<<std::endl;
        return false;
    }

    if (cbm_a->size() != cbm_b->size()){
        std::cerr<<"cbm_a or cbm_b size error"<<std::endl;
        return false;
    }
    
    for (size_t i=0; i< cbm_a->size(); ++i){
        if (((*cbm_a)[i]) != ((*cbm_b)[i])) return false;
    }

    return true;

}

int Search_CBM(EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>* tempcbm){
    EqID* tempEqID = temp_EqIDTable->head;
    if(tempEqID == nullptr){
        return -1;
    }
    else{
        while (tempEqID != nullptr){
            if (Compare_CBM(&tempEqID->cbm, tempcbm)) return tempEqID->eqid;
            else{
                tempEqID = tempEqID->next;
            }
        }
        return -1;
    }
}

int Add_cbm_EqIDTable(EqIDTable* temp_EqIDTable, std::vector<std::uint32_t>* tempcbm, std::uint32_t CBM_SIZE){
    EqID* new_EqID = new EqID(temp_EqIDTable->item_num, CBM_SIZE);
    for (size_t i=0; i<CBM_SIZE; i++){
            new_EqID->cbm[i] = (*tempcbm)[i];
        }

    temp_EqIDTable->add_EqID(new_EqID);

    return temp_EqIDTable->item_num -1 ;

}

int Find_rule(std::uint32_t CBM_SIZE, std::vector<std::uint32_t>* tempcbm, unsigned int filter_num){
    std::uint32_t flag;
    std::uint32_t result;

    for (size_t k=0; k<CBM_SIZE; k++){
        flag = 1 << 31;
        for (size_t pos=1; pos < 32; pos++){

            result = (*tempcbm)[k] & flag;
            if (result) return (32*k + pos - 1) - (32 - filter_num % 32);
            else flag >>= 1;
        }
    }

    return -1;
}

// Creating the phase0 of RFC classifier
std::vector<ChunkTable> Creat_Phase0(std::uint32_t CBM_SIZE, FilterSet& filter_set){
    std::vector<ChunkTable> phase0_chunks(6,ChunkTable(65536));

    for (size_t i = 0; i < phase0_chunks.size(); ++i){
        // Initialize data structure
        EqIDTable* temp_EqIDTable = new EqIDTable();
        phase0_chunks[i].match_EqIDTable(temp_EqIDTable);


        // Initialize cbm
        std::vector<std::uint32_t> tempcbm(CBM_SIZE,0);


        for (unsigned int n = 0; n < 65536; n++){
 
            unsigned short tempstart,tempend;
            int tempEqID;

            for (unsigned int m = 0; m < filter_set.num_filters; m++){
                
                tempstart = filter_set.filters_arr[m]->dim[i][0];
                tempend = filter_set.filters_arr[m]->dim[i][1];


                // obtain the cbm according to the value of each dim
                if (tempstart == n) Set_cbm(CBM_SIZE,&tempcbm,m,true,filter_set.num_filters);
                if ((tempend+1) == n) Set_cbm(CBM_SIZE,&tempcbm,m,false,filter_set.num_filters);
            }

            tempEqID = Search_CBM(temp_EqIDTable,&tempcbm);

            if (tempEqID == -1){
                tempEqID = Add_cbm_EqIDTable(temp_EqIDTable, &tempcbm, CBM_SIZE);
            }

            phase0_chunks[i].item[n] = static_cast<std::uint16_t>(tempEqID);
            

        }
    }

    return phase0_chunks;
}

// Creating the phase1 of RFC classifier
std::vector<ChunkTable> Creat_Phase1(std::uint32_t CBM_SIZE, std::vector<ChunkTable> phase0_chunks){
    std::vector<ChunkTable> phase1_chunks(2);

    ChunkTable* temp1;
    ChunkTable* temp2;
    ChunkTable* temp3;

    for (size_t i=0; i<phase1_chunks.size(); ++i){
        switch (i){
            case 0:
                temp1 = &phase0_chunks[0];
                temp2 = &phase0_chunks[1];
                temp3 = &phase0_chunks[5];
                break;
            case 1:
                temp1 = &phase0_chunks[2];
                temp2 = &phase0_chunks[3];
                temp3 = &phase0_chunks[4];
                break;
            
            default:
                break;
        }

        
        unsigned int temp_chunk_num = static_cast<unsigned int>(temp1->matched_EqIDTable->item_num) * 
            static_cast<unsigned int>(temp2->matched_EqIDTable->item_num) * 
            static_cast<unsigned int>(temp3->matched_EqIDTable->item_num);
        
        ChunkTable temp_chunk = ChunkTable(temp_chunk_num);
        EqIDTable* temp_EqIDTable = new EqIDTable();
        temp_chunk.match_EqIDTable(temp_EqIDTable);
        phase1_chunks[i] = temp_chunk;

        std::vector<std::uint32_t> tempcbm(CBM_SIZE,0);
        unsigned int index = 0;

        for (auto* eq1 = temp1->matched_EqIDTable->head; eq1 != nullptr; eq1 = eq1->next){
            for (auto* eq2 = temp2->matched_EqIDTable->head; eq2 != nullptr; eq2 = eq2->next){
                for (auto* eq3 = temp3->matched_EqIDTable->head; eq3 != nullptr; eq3 = eq3->next){

                    for(size_t j = 0; j<CBM_SIZE; ++j){
                        tempcbm[j] = eq1->cbm[j] & eq2->cbm[j] & eq3->cbm[j];
                    }

                    int tempEqID = Search_CBM(phase1_chunks[i].matched_EqIDTable, &tempcbm);
                    if (tempEqID == -1){
                        tempEqID = Add_cbm_EqIDTable(phase1_chunks[i].matched_EqIDTable, &tempcbm, CBM_SIZE);

                    }

                    phase1_chunks[i].item[index] = (static_cast<std::uint16_t>(tempEqID));
                    
                    index ++;

                }
            }
        }
    }
    return phase1_chunks;

}

// Creating the phasefinal of RFC classifier
ChunkTable Creat_phasefinal(std::uint32_t CBM_SIZE, std::vector<ChunkTable> phase1_chunks, FilterSet& filter_set){
    unsigned int temp_chunk_num = static_cast<unsigned int>(phase1_chunks[0].matched_EqIDTable->item_num)*
    static_cast<unsigned int>(phase1_chunks[1].matched_EqIDTable->item_num);

    ChunkTable final_phase = ChunkTable(temp_chunk_num);

    std::vector<std::uint32_t> tempcbm(CBM_SIZE,0);
    unsigned int index = 0;

    for (auto* eq1 = phase1_chunks[0].matched_EqIDTable->head; eq1 != nullptr; eq1 = eq1->next){
        for(auto* eq2 = phase1_chunks[1].matched_EqIDTable->head; eq2 != nullptr; eq2 = eq2->next){

            for (size_t j=0; j<CBM_SIZE; ++j){
                tempcbm[j] = eq1->cbm[j] & eq2->cbm[j];
            }

            int matched_rule = Find_rule(CBM_SIZE, &tempcbm, filter_set.num_filters);
            final_phase.item[index] = matched_rule;
            index ++;
        }
    }

    return final_phase;
}



RFC_Chunks Building_Classfier(std::uint32_t CBM_SIZE, FilterSet& filter_set){
    std::vector<ChunkTable> phase0_chunks = Creat_Phase0(CBM_SIZE,filter_set);
    std::vector<ChunkTable> phase1_chunks = Creat_Phase1(CBM_SIZE,phase0_chunks); 
    ChunkTable final_chunks = Creat_phasefinal(CBM_SIZE,phase1_chunks,filter_set);

    RFC_Chunks result;
    result.phase0_chunks = phase0_chunks;
    result.phase1_chunks = phase1_chunks;
    result.final_chunks = final_chunks;

    return result;
}