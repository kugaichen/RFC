#include <cstdint>
#include <ctime>
#include <cmath>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <filesystem> // C++17 for std::filesystem::absolute
#include <stdexcept>  // For std::invalid_argument, std::out_of_range

#include "../include/RFC_structure.h"
#include "../include/Classifier_building.h"
#include "../include/Classifier_lookuping.h"
#include "../include/log_creating.h"


std::uint32_t CBM_SIZE;


int main(int argc, char* argv[]){

    std::string filter_filepath_str;
    std::string packet_filepath_str;
    std::string log_filepath_str;

    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <filter_filepath> <packet_filepath> [output_log_filepath]" << std::endl;
        std::cerr << "Error: Missing required filter_filepath and/or packet_filepath." << std::endl;
        return 1; // Indicate an error
    }

    
    filter_filepath_str = argv[1];
    packet_filepath_str = argv[2];

    // 检查日志文件路径参数
    if (argc >= 4) {
        // 如果提供了第四个参数，则用作日志文件路径
        log_filepath_str = argv[3];
    } else {
        // 如果未提供第四个参数，则使用默认日志文件路径
        log_filepath_str = "classifier.log"; // 你可以自定义默认文件名
        std::cout << "Log file path not provided. Using default: " << log_filepath_str << std::endl;
    }



    const std::string filter_filepath = filter_filepath_str;
    const std::string packet_filepath = packet_filepath_str;

    // Check if filter file exists and can be opened
    std::ifstream filter_file_stream(filter_filepath);
    if (!filter_file_stream.is_open()) {
        std::cerr << "Error: Cannot open filter file at path: " << filter_filepath << std::endl;
        std::cerr << "Please ensure the file exists and the path is correct." << std::endl;
        return 1;
    }
    filter_file_stream.close(); // Close the file if it was only for checking

    // Check if packet file exists and can be opened
    std::ifstream packet_file_stream(packet_filepath);
    if (!packet_file_stream.is_open()) {
        std::cerr << "Error: Cannot open packet file at path: " << packet_filepath << std::endl;
        std::cerr << "Please ensure the file exists and the path is correct." << std::endl;
        return 1;
    }
    packet_file_stream.close(); // Close the file if it was only for checking


    std::cout << "Filter file path: " << filter_filepath << std::endl;
    std::cout << "Packet file path: " << packet_filepath << std::endl;

    Logger logger(log_filepath_str);
    
    if (!logger.is_open()) {
        std::cerr << "Critical Error: Logger could not be initialized. Log file might not be writable." << std::endl;
        return -1; 
    }

    logger.write("--- RFC Classifier Log Session Started ---");
    logger.write("Input Parameters:");
    logger.write("  Filter File: " + filter_filepath_str);
    logger.write("  Packet File: " + packet_filepath_str);
    logger.write("  Log File: " + log_filepath_str);
    logger.write(" ");
    logger.write("--- Filters and Packets Reading ---");

    FilterSet filter_set; // Use value from command line
    PacketSet packet_set; // Use value from command line

    Read_filter_file(filter_filepath, filter_set);
    Load_packets(packet_filepath, packet_set);

    logger.write("The num of Filters: " + std::to_string(filter_set.num_filters));
    logger.write("The num of Packets: " + std::to_string(packet_set.num_packets));


    // Ensuring the CBM
    CBM_SIZE = static_cast<std::uint32_t>(std::ceil(static_cast<double>(filter_set.num_filters) / 32));
    
    logger.write("The size of each CBM Group: " + std::to_string(sizeof(std::uint32_t)));
    logger.write("The num of CBM Group: " + std::to_string(CBM_SIZE));

    logger.write(" ");
    logger.write("------- RFC Classifier buidling --------");
    
    auto start_time_building = std::chrono::high_resolution_clock::now();

    RFC_Chunks rfc_chunks =  Building_Classfier(CBM_SIZE,filter_set);
    std::cout << "RFC classifier building successfully!" << std::endl;

    auto end_time_building = std::chrono::high_resolution_clock::now();

    auto duration_building = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_building - start_time_building);

    logger.write("--- RFC Classifier buidling complete! ---");

    logger.write("Time taken for Building_Classfier: " + std::to_string(duration_building.count()/1000) + " s");

    classifier_message_push(rfc_chunks,logger);

    logger.write(" ");
    logger.write("------- RFC Classifier lookuping --------");

    auto start_time_lookuping = std::chrono::high_resolution_clock::now();

    std::vector<int> LookupResult = Lookup(rfc_chunks.phase0_chunks,rfc_chunks.phase1_chunks,rfc_chunks.final_chunks,packet_set);
    std::cout << "RFC classifier lookuping successfully!" << std::endl;

    auto end_time_lookuping = std::chrono::high_resolution_clock::now();

    logger.write("--- RFC Classifier lookuping complete! ---");

    auto duration_lookuping = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_building - start_time_building);

    logger.write("Time taken for Classfier lookuping for " + std::to_string(packet_set.num_packets)  + " packets: "+ std::to_string(duration_lookuping.count()/1000) + " s");

    logger.write("====================================");

    int index = 0;
    for (const auto& pkt : packet_set.packets_arr){
        if (index == 10){
            break;
        }

        logger.write_inline("Lookuping packet " + std::to_string(index+1) + ": fields -> "); 
        for(const auto& info: pkt->fields){
            logger.write_inline(info + " ");
        }
        logger.write(" ");
        logger.write("Excpecting Result: " + pkt->except_rule);
        logger.write("Lookuping Result: " + std::to_string(LookupResult[index]));

        if (std::stoul(pkt->except_rule) == LookupResult[index]){
            logger.write("Lookup correct!");
        }
        else{
            logger.write("Lookup uncorrect.");
        }
        index ++;
        logger.write("-------------------------------");
    }
}