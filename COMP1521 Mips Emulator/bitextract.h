#ifndef BITEXTRACT_H
#define BITEXTRACT_H

// Prints the bits of a 32-bit integer, for debugging
void printBits(uint32_t input);

// Prints a 32-bit integer as hexadecimal, for debugging
void printHex(uint32_t input);

// Determines the opcode encoded by the instruction and returns it as a string,
// like "blez"
char *getCommand(uint32_t instruction);

// Gets a slice from a 32 bit bit-string given a boundary. Eg.
// extractBitSlice(0xFFFFFFFF, 4, 7) returns 1111 (inclusive boundaries)
// Note: this function assumes bits are index from 0-31
uint32_t extractBitSlice(uint32_t instruction, uint32_t from, uint32_t to);

// Given a 32 bit bit-string, identifies the leading 1 from the string and pads
// every bit to the left of it with 1s. 
// eg. Given 0x000000C0, returns 0xFFFFFFC0
// eg. Given 0x0000002A, returns 0xFFFFFFEA, NOT 0xFFFFFF2A
uint32_t padWithOnes(uint32_t instruction);

#endif