#include "write.hpp"
#include <iostream>
#include "lexer.hpp"
#include "preprocess.hpp"
#include <elf.h>
#include <cstring>
#include <cstdint>

using namespace std;

extern vector<vector<Token>> TOKENS;
extern vector<Label> LABELS;
extern size_t CODE_START;
extern size_t TEXT_SIZE;
extern size_t HEADER_SIZE;

// Returns true if the ascii character represents a number
bool is_char_num(char c) {
    return c >= '0' && c <= '9';
}

// Returns true if the str is made up of only numbers
bool is_str_int(string str) {
    for (char c : str) {
	if (!is_char_num(c)) return false;
    }
    return true;
}

// Converts a string to an int. NOTE: the string being entered must be completely full of numbers.
size_t str_to_int(string str) {
    // Error catching
    if (!is_str_int(str)) {
	cerr << "In \"str_to_int\" : \"" << str << "\" is not completely full of integers.\n";
	exit(1);
    }

    // Convert the string
    size_t num = 0;
    for (char c : str) {
	num *= 10;
	num += c - '0';
    }

    return num;
}

// Returns an integer based on the register : exits if the register isn't valid
int get_register(string str) {
    // @todo: add the rest
    /*
     * eax : 1
     * ebx : 2
    */
    
    if (str == "eax") {
	return 1;
    } else if (str == "ebx") {
	return 2;
    }

    exit(1);
}

// Writes the 4 bytes from the num into the file. NOTE: this is done in little endian format.
void write_4_bytes(ofstream& oFile, Token token) {

    size_t num = str_to_int(token.lexeme);
    
    if (num > 0xFFFFFFFF) {
	cerr << "Error line " << token.line_num << ", col " << token.col_num << " : Value of \"" << num << "\" is over 32 bits large.\n";
	exit(1);
    }

    oFile.put(num & 0xFF); // lowest byte in num
    oFile.put((num >> 8) & 0xFF);
    oFile.put((num >> 16) & 0xFF);
    oFile.put((num >> 24) & 0xFF); // highest byte in num
}

void write_code(ofstream& oFile) {

    /* ELF Header */
    Elf32_Ehdr ehdr = {0};
    memcpy(ehdr.e_ident, ELFMAG, SELFMAG); // Copy the ELF magic numbers into the header
    ehdr.e_ident[EI_CLASS] = ELFCLASS32;
    ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
    ehdr.e_ident[EI_VERSION] = EV_CURRENT;
    ehdr.e_ident[EI_OSABI] = ELFOSABI_SYSV;
    ehdr.e_ident[EI_ABIVERSION] = 0;
    ehdr.e_type = ET_EXEC;
    ehdr.e_machine = EM_386;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_entry = CODE_START; // where the program starts
    ehdr.e_phoff = sizeof(Elf32_Ehdr); // position where program header starts
    ehdr.e_ehsize = sizeof(Elf32_Ehdr); // size of the elf header
    ehdr.e_phentsize = sizeof(Elf32_Phdr); // size of each entry in the program header table
    ehdr.e_phnum = 1; // number of entries in the program header table
    
    /* Text Program Header */
    Elf32_Phdr text_phdr = {0};
    text_phdr.p_type = PT_LOAD; // loads it into memory
    text_phdr.p_offset = 0; // @todo: change this to get rid of the elf header. Then offset everything else.
    text_phdr.p_vaddr = CODE_START - HEADER_SIZE; // virtual address
    text_phdr.p_paddr = CODE_START - HEADER_SIZE; // physical address * unused
    text_phdr.p_filesz = 0x5; // size of the segment in the file
    text_phdr.p_memsz = 0x5; // size of the segment in memory
    text_phdr.p_flags = PF_R | PF_X; // read and execute
    text_phdr.p_align = 0x1000; // makes sure the segment's aligned with each page

    /* Write the header to the file */
    oFile.write(reinterpret_cast<const char*>(&ehdr), sizeof(ehdr)); // ELF header
    oFile.write(reinterpret_cast<const char*>(&text_phdr), sizeof(text_phdr)); // Text program header
    oFile.seekp(HEADER_SIZE, ios::beg);
    
    /* Go Through the Tokens */
    for (auto line : TOKENS) {
	string command = line[0].lexeme;

	// Go through each command
	if (command == "mov") {
	    // Write the register
	    int reg_num;
	    if (reg_num = get_register(line[1].lexeme)) {
		switch (reg_num) {
		case 1:
		    oFile.put(0xb8);
		    break;
		case 2:
		    oFile.put(0xbb);
		    break;		}
	    }
	    // Write the 4 bytes
	    if (is_str_int(line[2].lexeme)) {
		write_4_bytes(oFile, line[2]);
	    } else {
		cerr << "Error : line " << line[2].line_num << ", col " << line[2].col_num << " | \"" << line[2].lexeme << "\" is not a number.\n";
	    }
	    
	} else if (command == "int") {
	    
	} else if (command.back() == ':') { // could be a valid label
	    
	} else {
	    cerr << "Invalid command \"" << command << "\"\n";
	    exit(1);
	}
    }
    
#if 0
    /* Machine Code */
    const uint8_t code[] = {
	0xb8, 0x01, 0x0, 0x0, 0x0,
	0xbb, 0x00, 0x0, 0x0, 0x0,
	0xcd, 0x80
    };
    
    oFile.write(reinterpret_cast<const char*>(code), sizeof(code));
#endif
}
