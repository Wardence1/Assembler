/*
 * This is a test file and has nothing to do with the actual assembler
*/

#include <iostream>
#include <elf.h>
#include <fstream>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {

    /* Error Checking */
    if (argc <= 1) {
	cerr << "Use: assembler <executable>\n";
	return 1;
    }

    // Load the file
    ofstream file(argv[1], ios::binary);
    if (!file) {
	cerr << "Couldn't load the file.\n";
	return 1;
    }

    /* Setup */
    int header_size = sizeof(Elf32_Ehdr) + (sizeof(Elf32_Phdr) * 2);
    int code_start = 0x08048000;
    
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
    ehdr.e_entry = code_start + header_size; // where the program starts
    ehdr.e_phoff = sizeof(Elf32_Ehdr); // position where program header starts
    ehdr.e_ehsize = sizeof(Elf32_Ehdr); // size of the elf header
    ehdr.e_phentsize = sizeof(Elf32_Phdr); // size of each entry in the program header table
    ehdr.e_phnum = 2; // number of entries in the program header table
    
    /* Text Program Header */
    Elf32_Phdr text_phdr = {0};
    text_phdr.p_type = PT_LOAD; // loads it into memory
    text_phdr.p_offset = 0; // @todo: change this to get rid of the elf header. Then offset everything else.
    text_phdr.p_vaddr = code_start; // virtual address
    text_phdr.p_paddr = code_start; // physical address * unused
    text_phdr.p_filesz = 0x5; // size of the segment in the file
    text_phdr.p_memsz = 0x5; // size of the segment in memory
    text_phdr.p_flags = PF_R | PF_X; // read and execute
    text_phdr.p_align = 0x1000; // makes sure the segment's aligned with each page

    /* Data Program Header */
    Elf32_Phdr data_phdr = {0};
    data_phdr.p_type = PT_LOAD; // loads it into memory
    data_phdr.p_offset = 0x1000;
    data_phdr.p_vaddr = code_start + 0x1000; // virtual address
    data_phdr.p_paddr = code_start + 0x1000; // physical address * unused
    data_phdr.p_filesz = 0xc; // size of the segment in the file
    data_phdr.p_memsz = 0xc; // size of the segment in memory
    data_phdr.p_flags = PF_R | PF_W; // read and write @todo: allows execution within the sector???
    data_phdr.p_align = 0x1000; // makes sure the segment's aligned with each page
    
    /* Machine Code */
    const char code[] =
	"\xe9\x87\x0f\x00\x00"; // jmp to data segment

    /*
      "\xb8\x04\x00\x00\x00" // mov eax, 4 (sys_write)
      "\xbb\x01\x00\x00\x00" // mov ebx, 1 (stdout)
      "\xb9\x00\x90\x04\x08" // mov ecx, [str] (starting address of "Hello World\n")
      "\xba\x0e\x00\x00\x00" // mov edx, 14 (legnth of "Hello World\n")
      "\xcd\x80";             // int 0x80
    */

    
    const char data[] =
	"\xb8\x01\x00\x00\x00" // mov eax, 1 (sys_exit)
	"\xbb\x00\x00\x00\x00" // mov ebx, 0 (exit status)
	"\xcd\x80";          // int 0x80

    //"Hello, World!\n";
    
    /* Write to the file */
    file.write(reinterpret_cast<const char*>(&ehdr), sizeof(ehdr)); // ELF header
    file.write(reinterpret_cast<const char*>(&text_phdr), sizeof(text_phdr)); // Text program header
    file.write(reinterpret_cast<const char*>(&data_phdr), sizeof(data_phdr)); // Data program header
    file.seekp(header_size, ios::beg);
    file.write(code, sizeof(code) - 1);
    file.seekp(0x1000, ios::beg); // Move to the next page
    file.write(data, sizeof(data) - 1);
    file.close();
    
    return 0;
}
