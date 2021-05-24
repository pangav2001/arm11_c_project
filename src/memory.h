#include <stdint.h>

#ifndef MEMORY_H
#define MEMORY_H

void initialize_memory(void);

int store(uint16_t address, int8_t data);

int32_t load(uint16_t address, int num_bytes);

void free_memory(void);

void print_memory_state(void);

#endif