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

    /* Machine Code */
    const uint8_t code[] = {
	0xb8, 0x01, 0x0, 0x0, 0x0,
	0xbb, 0x00, 0x0, 0x0, 0x0,
	0xcd, 0x80
    };
    
    /* Write to the file */
    oFile.write(reinterpret_cast<const char*>(&ehdr), sizeof(ehdr)); // ELF header
    oFile.write(reinterpret_cast<const char*>(&text_phdr), sizeof(text_phdr)); // Text program header
    oFile.seekp(HEADER_SIZE, ios::beg);
    oFile.write(reinterpret_cast<const char*>(code), sizeof(code));
}
