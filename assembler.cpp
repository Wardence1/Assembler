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
    int header_size = sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr);
    
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
    ehdr.e_entry = 0x08048000 + header_size; // where the program starts
    ehdr.e_phoff = sizeof(Elf32_Ehdr); // position where program header starts
    ehdr.e_ehsize = sizeof(Elf32_Ehdr); // size of the elf header
    ehdr.e_phentsize = sizeof(Elf32_Phdr); // size of each entry in the program header table
    ehdr.e_phnum = 1; // number of entries in the program header table
    
    /* Program Header */
    Elf32_Phdr phdr = {0};
    phdr.p_type = PT_LOAD; // loads it into memory
    phdr.p_offset = 0; // @todo: change this to get rid of the elf header. Then offset everything else.
    phdr.p_vaddr = 0x08048000; // virtual address
    phdr.p_paddr = 0x08048000; // physical address * unused
    phdr.p_filesz = 0x1000; // size of the segment in the file
    phdr.p_memsz = 0x1000; // size of the segment in memory
    phdr.p_flags = PF_R | PF_X; // read and execute
    phdr.p_align = 0x1000; // makes sure the segment's aligned with each page
    
    /* Machine Code */
    const char code[] =
	"\xb8\x04\x00\x00\x00" // mov eax, 4 (sys_write)
	"\xbb\x01\x00\x00\x00" // mov ebx, 1 (stdout)
	"\xb9\x76\x80\x04\x08" // mov ecx, [str] (starting address of "Hello World\n")
	"\xba\x0e\x00\x00\x00" // mov edx, 14 (legnth of "Hello World\n")
	"\xcd\x80"             // int 0x80
	
	"\xb8\x01\x00\x00\x00" // mov eax, 1 (sys_exit)
	"\xbb\x00\x00\x00\x00" // mov ebx, 0 (exit status)
	"\xcd\x80"             // int 0x80
	"Hello, World!\n";

    /* Write to the file */
    file.write(reinterpret_cast<const char*>(&ehdr), sizeof(ehdr));
    file.write(reinterpret_cast<const char*>(&phdr), sizeof(phdr));
    file.seekp(header_size, ios::beg);
    file.write(code, sizeof(code) - 1);
    file.close();
    
    return 0;
}
