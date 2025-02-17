#include "preprocess.hpp"
#include <vector>
#include <iostream>
#include "lexer.hpp"

using namespace std;

extern vector<Label> LABELS;
extern vector<vector<Token>> TOKENS;

// @todo: set the starting value dynamically
size_t code_start = 0x08048054; // The start of where the code is loaded into memory + the ELF and program header
size_t text_size = 0x0; // distance from code_start

// Set labels
void preprocess() {
    for (auto line : TOKENS) {
	string command = line[0].lexeme;

	// Check the first word of each line
	if (command == "mov") {
	    text_size += 5; // 1 byte for the command, 4 for the value being moved into the register
	} else if (command == "int") {
	    text_size += 2; // 1 byte for the command, 1 to decide the interrupt being called
	} else if(command.back() == ':') { // is a label
	    LABELS.push_back(Label{command.substr(0, command.size()-1), text_size});
	} else {
	    cerr << "Error, line " << line[0].line_num << ", col " << line[0].col_num << " | \"" << command << "\" isn't a valid command\n";
	    exit(1);
	}
    }
}
