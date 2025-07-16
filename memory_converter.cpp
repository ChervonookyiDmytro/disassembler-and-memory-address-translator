#include <iostream>
#include "memory_table_loader.h"

int convert(uint32_t addr, const std::vector<memoryPage>& table, std::ostream& out) {
	
	uint32_t page_number = addr >> 11;
	uint32_t offset = addr & 0x7FF;
	for (auto page : table) {
		if (page_number == page.page_number) {
			if (page.present) {
				return (page.frame_number << 11) + offset;
			} else {
				out << "ERROR: Page " << std::dec << page_number << " is not present in memory for address [0x"
					<< std::hex << addr << "]\n\n";
				return -1;
			}
		}
	}
	out << "ERROR: Page " << std::dec << page_number << " not found for address [0x"
		<< std::hex << addr << "]\n\n";
	return -1;
}

