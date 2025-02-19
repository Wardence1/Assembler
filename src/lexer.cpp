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
    bool in_string = false;
    bool line_over = false; // When set to true, input will go to the next line on the next iteration of the loop
    string command = "";
    string line;
    
    while (getline(file, line)) {
	line_over = false;
	line_num++;
	col_num = 0;
	in_word = false;
	in_string = false;
	line += ' '; // Save the last word of the line

	// Go through the line
	for (int i = 0; i < line.size(); i++) {	
	    char c = tolower(line[i]); // @maybe_todo: case sensitive words need to be printed for errors.
	    if (line_over) break;
	    col_num++;

	    // Check if the stream enters a string
	    if (c == '\"') {
		in_string = !in_string;
		continue;
	    }
	    // Parse the string
	    if (in_string) {
		if (c == '\\') {
		    if (i+1 != line.size() && line[i+1] != '\"') {
			i++;
			// @todo: completely fill this out
			switch(line[i]) {
			case 'n':
			    c = '\n';
			    break;
			case 't':
			    c = '\t';
			    break;
			case 'r':
			    c = '\r';
			    break;
			case '\"':
			    c = '\"';
			    break;
			default:
			    cerr << "Error line " << line_num << " | invalid escape sequence \"\\" << line[i] << "\"\n";
			    exit(1);
			}
		    } else {
			cerr << "Error line " << line_num << " | escape sequence with no ending\n";
			exit(1);
		    }		    
		}
	    }

	    // Exit if the end of line is reached before the end of string
	    if (col_num >= line.size() && in_string) {
		cerr << "Error, line << " << line_num << " | End of line reached before the end of the string.\n";
		exit(1);
	    }

	    // If the stream encounters an empty space, save the word
	    if (!in_string && c == ' ' || c == '\t' || c == ';') {
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
