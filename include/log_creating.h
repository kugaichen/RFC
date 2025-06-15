#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream> // For std::cerr in case of errors
#include <ctime>    // For timestamp

#include "../include/RFC_structure.h"

class Logger {
public:
    // Constructor: Opens the log file.
    // Takes the log file path as an argument.
    Logger(const std::string& log_filepath_str); // Corrected constructor name

    // Destructor: Closes the log file.
    ~Logger();

    // Writes a message to the log file.
    void write(const std::string& message);

    void write_inline(const std::string& message); // 写入不换行

    // Overload to write a message with a timestamp.
    void write_with_timestamp(const std::string& message);

    // Checks if the log file is open and ready for writing.
    bool is_open() const;

    // Provides access to the underlying ofstream object if needed by other functions.
    std::ofstream& get_stream();

private:
    std::ofstream log_stream_; // Member variable for the file stream
    std::string log_filepath_;
    bool is_file_open_;
};


double ChunkTable_Memeory_Calcualte(const ChunkTable& chunktable);

void classifier_message_push(const RFC_Chunks rfc_chunks, Logger& logger);

#endif // LOGGER_H