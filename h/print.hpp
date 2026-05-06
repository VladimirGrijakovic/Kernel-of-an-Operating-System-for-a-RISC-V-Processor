//
// Created by os on 10/4/25.
//

#ifndef PROJECT_BASE_V1_1_PRINT_HPP
#define PROJECT_BASE_V1_1_PRINT_HPP

#include "syscall_c.hpp"

typedef unsigned long uint64;

extern "C" uint64 copy_and_swap(uint64 &lock, uint64 expected, uint64 desired);

void printStringA(char const *string);

char* getStringA(char *buf, int max);

int stringToIntA(const char *s);

void printIntA(int xx, int base=10, int sgn=0);

#endif //PROJECT_BASE_V1_1_PRINT_HPP