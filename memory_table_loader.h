#pragma once
#include <vector>
#include <string>

struct memoryPage {
	int page_number;
	bool present;
	int frame_number;
};

std::vector<memoryPage> read_CSV(const std::string&);