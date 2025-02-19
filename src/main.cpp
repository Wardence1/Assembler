#include <iostream>
#include <fstream>
#include <string>
#include "lexer.hpp"
#include "preprocess.hpp"
#include "write.hpp"

using namespace std;

// Each vector of strings represents each word in the given line
vector<vector<Token>> TOKENS;
vector<Label> LABELS;
size_t HEADER_SIZE = 0x54; // Size of the ELF header + program header/s
size_t CODE_START = 0x08048000 + HEADER_SIZE;
size_t TEXT_SIZE = 0x0;

int main(int argc, char** argv) {

    /* Error Checking */
    if (argc <= 2) {
	cerr << "Use: assembler <source> <executable>\n";
	return 1;
    }

    // Load the input file
    ifstream iFile(argv[1]);
    if (!iFile) {
	cerr << "Couldn't load the file\"" << argv[1] << "\"\n";
	return 1;
    }
    // Load the output file
    ofstream oFile(argv[2]);
    if (!oFile) {
	cerr << "Couldn't load the file\"" << argv[2] << "\"\n";
	return 1;
    }

    lex(iFile);
    preprocess();
    write_code(oFile);
    
    // **DEBUG** Print out all tokens    
    for (auto commands : TOKENS) {
	for (Token token : commands)
	    cout << token.lexeme << " ";
	cout << "\n";
    }

    // **DEBUG** Print out all labels
    /*for (Label label : LABELS) {
	cout << label.name << " : 0x" << hex << CODE_START + label.mem_pos << "\n";
    }*/

    iFile.close();
    oFile.close();

    return 0;
}
