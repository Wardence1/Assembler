#include "preprocess.hpp"
#include <vector>
#include <iostream>
#include "lexer.hpp"
#include <string>

using namespace std;

extern vector<Label> LABELS;
extern vector<vector<Token>> TOKENS;
extern size_t TEXT_SIZE;
extern bool is_str_int(string str);

// Set labels, count data
void preprocess() {
    for (auto line : TOKENS) {
	string command = line[0].lexeme;

	// Check the first word of each line
	if (command == "mov") {
	    TEXT_SIZE += 5; // 1 byte for the command, 4 for the value being moved into the register
	} else if (command == "syscall") {
	    TEXT_SIZE += 2; // 1 byte for the command, 1 for 0x80
	} else if (command == "ds") {
	    if (line.size() < 2) { // return an error if there isn't data to be allocated after "ds"
		cerr << "Error, line " << line[0].line_num << " | insert a string after \"ds\"\n";
		exit(1);
	    }
	    TEXT_SIZE += line[2].lexeme.size(); // size in bytes is based off the size of the string
	} else if (command == "jmp") {
            // @todo: accound for big jumps
	    TEXT_SIZE += 2; // 1 byte for the command, 1 to offset the current memory address
	} else if (command.back() == ':') { // is a label
	    LABELS.push_back(Label{command.substr(0, command.size()-1), TEXT_SIZE});
	} else {
	    cerr << "Error, line " << line[0].line_num << ", col " << line[0].col_num << " | \"" << command << "\" isn't a valid command\n";
	    exit(1);
	}
    }
}
