#pragma once

#include "lexer.hpp"

typedef struct {
    std::string name;
    size_t mem_pos;
} Label;

void preprocess();
