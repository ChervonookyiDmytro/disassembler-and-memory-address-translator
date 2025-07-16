#pragma once
#include <iostream>
#include <vector>
#include "memory_table_loader.h"

int convert(uint32_t addr, const std::vector<memoryPage>&, std::ostream& out);