#include "../include/log_creating.h" 
#include "../include/RFC_structure.h"


// Corrected constructor implementation
Logger::Logger(const std::string& log_filepath_str)
    : log_filepath_(log_filepath_str), is_file_open_(false) { // Initialize members

    // Open the member log_stream_
    // std::ios::out ensures the file is created if it doesn't exist.
    // std::ios::app ensures that if the file exists, output is appended to the end.
    log_stream_.open(log_filepath_, std::ios::out | std::ios::trunc);

    if (log_stream_.is_open()) {
        is_file_open_ = true;
    } else {
        is_file_open_ = false; // Explicitly set to false
        std::cerr << "Error: Could not open log file for writing at: " << log_filepath_ << std::endl;
        std::cerr << "Please ensure the path is valid and you have write permissions." << std::endl;
    }
}

// ... (Implementations for ~Logger(), write(), write_with_timestamp(), is_open(), get_stream() are needed here) ...

// Example for is_open()
bool Logger::is_open() const {
    return is_file_open_;
}

// Example for write() - you need to complete this and others
void Logger::write(const std::string& message) {
    if (is_file_open_) {
        log_stream_ << message << std::endl;
    }
}

void Logger::write_inline(const std::string& message) {
    if (is_file_open_) {
        log_stream_ << message; // 不使用 std::endl
    }
}

// Destructor implementation
Logger::~Logger() {
    if (log_stream_.is_open()) {
        // Optionally write a closing message
        // write_with_timestamp("--- Log Session Ended ---"); // Be careful if write_with_timestamp uses log_stream_
        log_stream_.close();
    }
}

// You need to implement write_with_timestamp and get_stream as well.
// For example:
void Logger::write_with_timestamp(const std::string& message) {
    if (is_file_open_) {
        time_t now = time(0);
        char time_buf[26];
        #if defined(_WIN32) || defined(_WIN64)
            ctime_s(time_buf, sizeof(time_buf), &now);
            std::string time_str(time_buf);
            if (!time_str.empty() && time_str.back() == '\n') {
                time_str.pop_back();
            }
        #else
            strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&now));
            std::string time_str(time_buf);
        #endif
        log_stream_ << "[" << time_str << "] " << message << std::endl;
    }
}

std::ofstream& Logger::get_stream() {
    return log_stream_;
}



double ChunkTable_Memeory_Calcualte(const ChunkTable& chunktable){
    double memory = 0.0;
    memory += chunktable.chunk_num * sizeof(chunktable.item[1]);

    return memory;
}

void classifier_message_push(const RFC_Chunks rfc_chunks, Logger& logger){
    double total_memory = 0.0;
    double memory0 = 0.0;
    double memory1 = 0.0;
    double memory2 = 0.0;

    total_memory += sizeof(rfc_chunks);

    for (const auto& Ct : rfc_chunks.phase0_chunks){
        memory0 += ChunkTable_Memeory_Calcualte(Ct);
    }


    for (const auto& Ct: rfc_chunks.phase1_chunks){
        memory1 += ChunkTable_Memeory_Calcualte(Ct);
    }

    memory2 += ChunkTable_Memeory_Calcualte(rfc_chunks.final_chunks);
    total_memory = memory0 + memory1 + memory2;

    logger.write("The memory consumption of phase0 in RFC: " + std::to_string(memory0/1024) + " KB");
    logger.write("The memory consumption of phase1 in RFC: " + std::to_string(memory1/1024) + " KB");
    logger.write("The memory consumption of finalphase in RFC: " + std::to_string(memory2/1024) + " KB");
    logger.write("---> The memory consumption of RFC: " + std::to_string(total_memory/1024) + " KB");
}