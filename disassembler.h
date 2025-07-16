#pragma once
#include "memory_table_loader.h"

std::string decode_commands(const std::vector<uint8_t>&, const std::vector<memoryPage>&);
void runDisassembler(const std::string& str, const std::vector<memoryPage>&);
void runDisassembler(std::ifstream& file, const std::vector<memoryPage>&);
std::vector<uint8_t> parse_hex_string(const std::string&);
std::vector<uint8_t> read_hex_bytes(std::ifstream&);