#include <fstream>
#include <iostream>
#include "memory_table_loader.h"
#include "disassembler.h"
#include "test.h"

static bool compare_files(std::ifstream& f1, std::ifstream& f2) {
	if (!f1 || !f2)
		return false;

	std::string line1, line2;
	size_t line_num = 1;

	while (std::getline(f1, line1) && std::getline(f2, line2)) {
		if (line1 != line2) {
			std::cout << "Difference at line " << line_num << ":\n";
			std::cout << "Result: " << line1 << "\n";
			std::cout << "Expected: " << line2 << "\n";
			return false;
		}
		++line_num;
	}

	return true;
}


void runTestWithFile(const std::vector<memoryPage>& pages) {
	std::string test_file_name = "test.txt";
	std::ifstream test_file(test_file_name);
	std::ofstream result("result.txt");
	std::ifstream expected("expected.txt");
	std::vector<uint8_t> bytes;

	if (!test_file || !result || !expected) {
		std::cout << "Can not open the file!\n";
		return;
	}

	result << decode_commands(read_hex_bytes(test_file), pages);
	result.close();
	std::ifstream result_read("result.txt");

	if (compare_files(result_read, expected))
		std::cout << "Test with file \"" << test_file_name << "\" passed.\n";
	else
		std::cout << "Test with file\"" << test_file_name << "\" did not pass.\n";

	result_read.close();
	expected.close();
	test_file.close();
}


void runTestWithStrings(const std::vector<memoryPage>& pages) {
	struct TestCase {
		std::string input_hex;
		std::string expected_output;
	};

	std::vector<TestCase> tests = {
		{
			"1A12",
			"1A 12:\nMOV R1, R2\n\n",
		},
		{
			"1B1100000042",
			"1B 11 00 00 00 42:\nMOV [0x00000842], R1\n\n"
		},
		{
			"0C0123",
			"0C 01 23:\nSUB R1, R2, R3\n\n"
		},
		{
			"0D1200005432",
			"0D 12 00 00 54 32:\nERROR: Page 10 not found for address [0x5432]\n\n"
		},
		{
			"210123",
			"21 01 23:\nMUL R1, R2, R3\n\n"
		},
		{
			"231200123001",
			"23 12 00 12 30 01:\nMUL R1, R2, [0x00001801]\n\n"
		},
		{
			"23 12 00 00 4F A9",
			"23 12 00 00 4F A9:\nMUL R1, R2, [0x00002FA9]\n\n"
		},
		{
			"23 12 00 00 AF A9",
			"23 12 00 00 AF A9:\nERROR: Page 21 is not present in memory for address [0xAFA9]\n\n"
		},
		{
			"8012",
			"80 12:\nCMP R1, R2\n\n"
		},
		{
			"1C0133",
			"1C 01 33:\nMOV R1, 51\n\n"
		},
		{
			"1C113344",
			"1C 11 33 44:\nMOV R1, 13124\n\n"
		}
	};

	size_t passed_count = tests.size();
	for (const auto& test : tests) {
		std::string result = decode_commands(parse_hex_string(test.input_hex), pages);
		if (result != test.expected_output) {
			std::cout << "Result:\n" << result;
			std::cout << "Expected:\n" << test.expected_output;
			std::cout << "Failed\n\n";
			--passed_count;
		}
	}
	std::cout << passed_count << "/" << tests.size() << " test cases passed.\n";
}
