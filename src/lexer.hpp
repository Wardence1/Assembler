#pragma once

#include <fstream>
#include <vector>

typedef struct {
    std::string lexeme;
    size_t line_num;
    size_t col_num;
} Token;


// Updates a the tokens global
void lex(std::ifstream& file);
