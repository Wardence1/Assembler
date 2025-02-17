#include <iostream>
#include <fstream>
#include <string>
#include "lexer.hpp"
#include "preprocess.hpp"

using namespace std;

// Each vector of strings represents each word in the given line
vector<vector<Token>> TOKENS;
vector<Label> LABELS;

int main(int argc, char** argv) {

    /* Error Checking */
    if (argc <= 1) {
	cerr << "Use: assembler <executable>\n";
	return 1;
    }

    // Load the file
    ifstream file(argv[1]);
    if (!file) {
	cerr << "Couldn't load the file.\n";
	return 1;
    }

    lex(file);
    preprocess();
    
    // **DEBUG** Print out all tokens    
    /*for (auto commands : TOKENS) {
	for (Token token : commands)
	    cout << token.lexeme << " ";
	cout << "\n";
    }*/

    // **DEBUG** Print out all labels
    for (Label label : LABELS) {
	cout << label.name << "\n";
    }

    return 0;
}
