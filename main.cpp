#include <fstream>
#include "disassembler.h"
#include "memory_table_loader.h"
#include "test.h"

int main() {
    runTestWithStrings(read_CSV("table_for_test.csv"));
    runTestWithFile(read_CSV("table_for_test.csv"));

    std::ifstream file("test.txt");
    runDisassembler("231200004FA9", read_CSV("table.csv"));
    runDisassembler(file, read_CSV("table.csv"));

    file.close();
}
