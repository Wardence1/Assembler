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


ssize_t current_byte = 0; // Keeps track of where the program is currently at for offset calculation
// Put a byte into the file
void put_byte(ofstream& oFile, uint8_t byte) {
    oFile.put(byte);
    current_byte++;
}

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
	cerr << "Error | \"" << str << "\" is not completely full of integers.\n";
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
int get_register(Token token) {
    // @todo: add other registers
    /*
     * eax : 1
     * ebx : 2
     * ecx : 3
     * edx : 4
    */

    string str = token.lexeme;
    if (str == "eax") {
	return 1;
    } else if (str == "ebx") {
	return 2;
    } else if (str == "ecx") {
	return 3;
    } else if (str == "edx") {
	return 4;
    }

    cerr << "Error line " << token.line_num << " : \"" << str << "\" is an invalid register\n";
    exit(1);
}

// Writes the 4 bytes from the token into the file. NOTE: this is done in little endian format.
void write_token(ofstream& oFile, Token token) {

    size_t num = str_to_int(token.lexeme);
    
    if (num > 0xFFFFFFFF) {
	cerr << "Error line " << token.line_num << ", col " << token.col_num << " : Value of \"" << num << "\" is over 32 bits large.\n";
	exit(1);
    }

    put_byte(oFile, num & 0xFF); // lowest byte in num
    put_byte(oFile, (num >> 8) & 0xFF);
    put_byte(oFile, (num >> 16) & 0xFF);
    put_byte(oFile, (num >> 24) & 0xFF); // highest byte in num
}

// Write the memory address of a label to the file
void write_label(ofstream& oFile, Label label) {
    size_t mem_addr = label.mem_pos + CODE_START;
    oFile.write(reinterpret_cast<char*>(&mem_addr), 4); // Write memory address
    current_byte += 4;
}

// Write the offset between the current bytes and a label to the file
void write_offset(ofstream& oFile, Label label) {
    int32_t offset = static_cast<int32_t>(label.mem_pos) - (static_cast<int32_t>(current_byte)+4); // Get the offset between the label and the current point
    oFile.write(reinterpret_cast<char*>(&offset), 4); // Write the offset
    current_byte += 4;
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
    text_phdr.p_filesz = TEXT_SIZE + HEADER_SIZE; // size of the segment in the file
    text_phdr.p_memsz = TEXT_SIZE + HEADER_SIZE; // size of the segment in memory
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
	if (command == "mov") { // MOV : moves a value into a register
	    // Make sure the line's the right size
	    if (line.size() < 3) {
		cerr << "Error: line " << line[0].line_num << " doesn't have the correct amount of arguments for \"MOV\"\n";
		exit(1);
	    }

            // Write the register
	    int reg_num;
	    if (reg_num = get_register(line[1])) {
		switch (reg_num) {
		case 1:
		    put_byte(oFile, 0xb8);
		    break;
		case 2:
		    put_byte(oFile, 0xbb);
		    break;
		case 3:
		    put_byte(oFile, 0xb9);
		    break;
		case 4:
		    put_byte(oFile, 0xba);
		    break;
		}
	    }
	    // Write the 4 bytes
	    if (is_str_int(line[2].lexeme)) {
		write_token(oFile, line[2]); // Write a constant
	    } else {
		bool found = false;
		for (Label label : LABELS) {
		    if (label.name == line[2].lexeme) {
			write_label(oFile, label);
			found = true;
		    }
		}
		if (!found) cerr << "Error : line " << line[2].line_num << " | \"" << line[2].lexeme << "\" is not a number or label.\n";
	    }
	    
	} else if (command == "syscall") { // SYSCALL : same as "int 0x80", calls a system call based on register values
	    put_byte(oFile, 0xcd);
	    put_byte(oFile, 0x80);
	} else if (command == "ds") { // DS : defines a string in memory
	    for (char c : line[1].lexeme) {
		put_byte(oFile, c);
	    }
	} else if (command == "jmp") { // JMP : jumps to the specified label
	    // Make sure the line's the right size
	    if (line.size() < 2) {
		cerr << "Error: line " << line[0].line_num << " doesn't have the correct amount of arguments for \"JMP\"\n";
		exit(1);
	    }

	    put_byte(oFile, 0xE9);

	    // Write the offset of the label
	    bool found = false;
	    for (Label label : LABELS) {
		if (label.name == line[1].lexeme) {
		    write_offset(oFile, label);
		    found = true;
		}
	    }
	    if (!found) cerr << "Error : line " << line[0].line_num << " | \"" << line[1].lexeme << "\" is not a label.\n";

	} else if (command.back() == ':') { // is a label
	    
	} else {
	    cerr << "Invalid command \"" << command << "\"\n";
	    exit(1);
	}
    }
}
