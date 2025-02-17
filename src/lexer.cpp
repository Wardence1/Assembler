#include "lexer.hpp"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

extern vector<vector<Token>> TOKENS;

// Checks to see if a string is empty
bool is_blank(string str) {
    if (str == "") return false;
    for (char c : str) {
	if (c != ' ' || c != '\t' || c != '\n') return false;
    }
    return true;
}

void lex(ifstream& file) { 

    /* Go through each line of the file */
    // Setup
    size_t line_num = 0, col_num = 0;
    vector<Token> commands;
    bool in_word = false;
    bool line_over = false; // When set to true, input will go to the next line on the next iteration of the loop
    string command = "";
    string line;
    
    while (getline(file, line)) {
	line_over = false;
	line_num++;
	col_num = 0;
	in_word = false;
	line += ' '; // Save the last word of the line

	// Go through the line
	for (char c : line) {
	    c = tolower(c); // Make case insensitive
	    if (line_over) break;
	    col_num++;

	    // If the stream encounters an empty space, save the word
	    if (c == ' ' || c == '\t' || c == ';') {
		if (in_word) commands.push_back(Token{command, line_num, col_num});
		in_word = false;
		command = "";
		if (c == ';') line_over = true; // If the stream encounters a semicolon go to the next line
		continue;
	    } else {
		in_word = true;
		command += c;
	    }
	}

	// Check to see if commands is empty
	for (Token token : commands) {
	    if (!is_blank(token.lexeme)) {
	        TOKENS.push_back(commands);
		break;
	    }
	}
	
	commands.clear();
    }
}
