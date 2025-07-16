#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include "memory_table_loader.h"
#include "memory_converter.h"

std::vector<uint8_t> parse_hex_string(const std::string& hex_input) {
    std::vector<uint8_t> bytes;
    std::string hexstring;

    for (char c : hex_input)
        if (std::isxdigit(c)) hexstring += c;

    if (hexstring.size() % 2 != 0)
        throw std::runtime_error("Odd number of hex characters in string!");

    for (size_t i = 0; i < hexstring.size(); i += 2) {
        std::string byte_str = hexstring.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoul(byte_str, nullptr, 16));
        bytes.push_back(byte);
    }

    return bytes;
}


std::vector<uint8_t> read_hex_bytes(std::ifstream& file) {
    std::string hexstring;
    char c;

    if (!file)
        throw std::runtime_error("Can not open the file!");

    while (file.get(c))
        if (std::isxdigit(c))
            hexstring += c;

    file.close();
    return parse_hex_string(hexstring);
}


static void print_bytes(std::ostream& out, const std::vector<uint8_t>& bytes, size_t from, size_t count) {
    for (size_t i = from; i < from + count - 1; ++i)
        out << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]) << " ";

    out << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[from + count - 1]) << ":\n";
}


static void mov_or_cmp_reg_reg(std::ostream& out, const std::vector<uint8_t>& bytes, size_t& i, const std::string& str = "MOV") {
    uint8_t b = bytes[i + 1];
    uint8_t reg1 = b >> 4;
    uint8_t reg2 = b & 0x0F;

    print_bytes(out, bytes, i, 2);
    out << str << " R" << static_cast<int>(reg1) << ", R" << static_cast<int>(reg2) << "\n\n";
    i += 2;
}


static void mov_addr_reg(std::ostream& out, const std::vector<uint8_t>& bytes, size_t& i, const std::vector<memoryPage>& table) {
    uint8_t b = bytes[i + 1];
    uint8_t reg = b & 0x0F;
    uint32_t addr =
        (static_cast<uint32_t>(bytes[i + 2]) << 24) |
        (static_cast<uint32_t>(bytes[i + 3]) << 16) |
        (static_cast<uint32_t>(bytes[i + 4]) << 8) |
        (static_cast<uint32_t>(bytes[i + 5]));

    print_bytes(out, bytes, i, 6);
    int physical_address = convert(addr, table, out);

    if (physical_address != -1) {
    out << "MOV [0x" << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << physical_address << "], R"
        << static_cast<int>(reg) << "\n\n";
    }

    i += 6;
}


static void sub_or_mul_reg_reg_reg(std::ostream& out, const std::vector<uint8_t>& bytes, size_t& i, const std::string& str = "SUB") {
    uint8_t b = bytes[i + 1];
    uint8_t reg1 = b & 0x0F;
    b = bytes[i + 2];
    uint8_t reg2 = b >> 4;
    uint8_t reg3 = b & 0x0F;

    print_bytes(out, bytes, i, 3);
    out << str << ' ' << std::hex << std::uppercase << std::setfill('0') << "R"
        << static_cast<int>(reg1) << ", R" << static_cast<int>(reg2) << ", R" << static_cast<int>(reg3) << "\n\n";
    i += 3;
}


static void sub_or_mul_reg_reg_addr(std::ostream& out, const std::vector<uint8_t>& bytes, size_t& i, const std::vector<memoryPage>& table, const std::string& str = "SUB") {
    uint8_t b = bytes[i + 1];
    uint8_t reg1 = b >> 4;
    uint8_t reg2 = b & 0x0F;

    uint32_t addr =
        (static_cast<uint32_t>(bytes[i + 2]) << 24) |
        (static_cast<uint32_t>(bytes[i + 3]) << 16) |
        (static_cast<uint32_t>(bytes[i + 4]) << 8) |
        (static_cast<uint32_t>(bytes[i + 5]));

    print_bytes(out, bytes, i, 6);
    int physical_address = convert(addr, table, out);

    if (physical_address != -1) {
        out << str << " R" << static_cast<int>(reg1) << ", R" << static_cast<int>(reg2) << ", [0x"
            << std::hex << std::uppercase << std::setw(8) << std::setfill('0') << physical_address << "]" << "\n\n";
    }
    i += 6;
}


static void mov_reg_lit8(std::ostream& out, const std::vector<uint8_t>& bytes, size_t& i) {
    uint8_t b = bytes[i + 1];
    uint8_t reg = b & 0x0F;
    b = bytes[i + 2];

    print_bytes(out, bytes, i, 3);
    out << "MOV R" << static_cast<int>(reg) << ", " << std::dec << static_cast<int>(b) << "\n\n";
    i += 3;
}


static void mov_reg_lit16(std::ostream& out, const std::vector<uint8_t>& bytes, size_t& i) {
    uint8_t b = bytes[i + 1];
    uint8_t reg = b & 0x0F;
    uint16_t lit16 = (static_cast<uint16_t>(bytes[i + 2]) << 8) | bytes[i + 3];

    print_bytes(out, bytes, i, 4);
    out << "MOV R" << static_cast<int>(reg) << ", " << std::dec << lit16 << "\n\n";
    i += 4;
}


std::string decode_commands(const std::vector<uint8_t>& bytes, const std::vector<memoryPage>& pages) {
    std::ostringstream out;
    size_t i = 0;

    while (i < bytes.size()) {
        switch (bytes[i]) {
        case 0x1A:
            mov_or_cmp_reg_reg(out, bytes, i);
            break;
        case 0x1B:
            mov_addr_reg(out, bytes, i, pages);
            break;
        case 0x0C:
            sub_or_mul_reg_reg_reg(out, bytes, i);
            break;
        case 0x0D:
            sub_or_mul_reg_reg_addr(out, bytes, i, pages);
            break;
        case 0x21:
            sub_or_mul_reg_reg_reg(out, bytes, i, "MUL");
            break;
        case 0x23:
            sub_or_mul_reg_reg_addr(out, bytes, i, pages, "MUL");
            break;
        case 0x80:
            mov_or_cmp_reg_reg(out, bytes, i, "CMP");
            break;
        case 0x1C:
            if ((bytes[i + 1] >> 4) == 0)
                mov_reg_lit8(out, bytes, i);
            else if((bytes[i + 1] >> 4) == 1)
                mov_reg_lit16(out, bytes, i);
            break;
        default:
            out << "Unknown command with code: 0x" << std::hex << std::uppercase << static_cast<int>(bytes[i]) << "\n";
            ++i;
        }
    }

    return out.str();
}


void runDisassembler(const std::string& str, const std::vector<memoryPage>& table) {
    std::cout << decode_commands(parse_hex_string(str), table);
}


void runDisassembler(std::ifstream& file, const std::vector<memoryPage>& table) {
    if (file) {
        std::cout << decode_commands(read_hex_bytes(file), table);
    } else {
        std::cout << "Can not open the file!\n";
    }
}
