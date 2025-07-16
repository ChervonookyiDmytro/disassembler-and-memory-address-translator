#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <set>

struct memoryPage {
	int page_number;
	bool present;
	int frame_number;
};

std::vector<memoryPage> read_CSV(const std::string& fileName) {
	std::vector<memoryPage> pages;
	std::ifstream file(fileName);
	std::string line;
	std::set<int> seen_pages;
	std::set<int> seen_frames;

	if (!file) {
		std::cout << "Can not open the file" << fileName << "!\n";
		return{};
	}

	while (std::getline(file, line)) {
		std::istringstream ss(line);
		std::string token;
		memoryPage entry;

		std::getline(ss, token, ',');
		try {
			int page_number = std::stoi(token);
			if (page_number >= 0 && page_number <= 2'097'151)
				if (!seen_pages.count(page_number)) {
					entry.page_number = page_number;
					seen_pages.insert(page_number);
				} else {
					std::cout << "Duplicate of page number in CSV: " << token << "\n";
					continue;
				}
			else {
				std::cout << "Invalid page number in CSV: " << token << "\n";
				continue;
			}
		}
		catch (const std::invalid_argument&) {
			std::cout << "Invalid page number (not a number) in CSV: " << token << "\n";
			continue;
		}
		catch (const std::out_of_range&) {
			std::cout << "Invalid page number (too large) in CSV: " << token << "\n";
			continue;
		}


		std::getline(ss, token, ',');
		try {
			bool present_flag = std::stoi(token);
			entry.present = (present_flag == 1);
		}
		catch (...) {
			std::cout << "Invalid present field in CSV: " << token << "\n";
			continue;
		}


		std::getline(ss, token, ',');
		try {
			int frame_number = std::stoi(token);
			if (frame_number >= 0 && frame_number <= 31) {
				if (!seen_frames.count(frame_number)) {
					entry.frame_number = frame_number;
					seen_frames.insert(frame_number);
				} else {
					std::cout << "Duplicate of frame number in CSV: " << token << "\n";
					continue;
				}
			}
			else {
				std::cout << "Invalid frame number in CSV: " << token << "\n";
				continue;
			}
		}
		catch (const std::invalid_argument&) {
			std::cout << "Invalid frame number (not a number) in CSV: " << token << "\n";
			continue;
		}
		catch (const std::out_of_range&) {
			std::cout << "Invalid frame number (too large) in CSV: " << token << "\n";
			continue;
		}

		pages.push_back(entry);
	}

	file.close();
	return pages;
}