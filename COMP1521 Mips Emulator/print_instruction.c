#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "emu.h"
#include "ram.h"
#include "registers.h"
#include "bitextract.h"

// ========================== My Helper Functions ==============================
// Given a command string, like "mul", determine what info to extract, then
// print the formatted string
static void extractAndPrint(uint32_t instruction, char *command);

// =============================================================================
void print_instruction(uint32_t instruction) {
    char *command = getCommand(instruction);
    extractAndPrint(instruction, command);
    free(command);
}

static void extractAndPrint(uint32_t instruction, char *command) {
    // For commands of bit pattern: 000000|sssss|ttttt|ddddd|00000|OPCODE
    // Format: command $d, $s, $t
    if (strcmp(command, "add") == 0 ||
        strcmp(command, "sub") == 0 ||
        strcmp(command, "mul") == 0 || 
        strcmp(command, "and") == 0 ||
        strcmp(command, "or") == 0  ||
        strcmp(command, "xor") == 0 ||
        strcmp(command, "slt") == 0)  {
        uint32_t dReg = extractBitSlice(instruction, 11, 15);
        uint32_t sReg = extractBitSlice(instruction, 21, 25);
        uint32_t tReg = extractBitSlice(instruction, 16, 20);
        printf("%s $%d, $%d, $%d", command, dReg, sReg, tReg);
    } 
    // Format: command $d, $t, $s
    if (strcmp(command, "sllv") == 0 ||
        strcmp(command, "srlv") == 0) {
        uint32_t dReg = extractBitSlice(instruction, 11, 15);
        uint32_t tReg = extractBitSlice(instruction, 16, 20);
        uint32_t sReg = extractBitSlice(instruction, 21, 25);
        printf("%s $%d, $%d, $%d", command, dReg, tReg, sReg);
    }
    // For commands of bit pattern: OPCODE|sssss|ttttt|IIIIIIIIIIIIIIII
    // Format: command $t, $s, I
    if (strcmp(command, "addi") == 0 ||
        strcmp(command, "andi") == 0 ||
        strcmp(command, "ori") == 0 ||
        strcmp(command, "xori") == 0 ||
        strcmp(command, "slti") == 0) {
        uint32_t tReg = extractBitSlice(instruction, 16, 20);
        uint32_t sReg = extractBitSlice(instruction, 21, 25);
        int16_t imm = extractBitSlice(instruction, 0, 15);
        printf("%s $%d, $%d, %d", command, tReg, sReg, imm);
    }
    // Format: command $s, $t, I
    if (strcmp(command, "beq") == 0 ||
        strcmp(command, "bne") == 0) {
        uint32_t sReg = extractBitSlice(instruction, 21, 25);
        uint32_t tReg = extractBitSlice(instruction, 16, 20);
        int16_t imm = extractBitSlice(instruction, 0, 15); 
        printf("%s $%d, $%d, %d", command, sReg, tReg, imm); 
    }
    // For commands of bit pattern: 000000|0000X|ttttt|ddddd|IIIII|OPCODE
    // Format: command $d, $t, I
    if (strcmp(command, "sll") == 0 ||
        strcmp(command, "srl") == 0) {
        uint32_t dReg = extractBitSlice(instruction, 11, 15);
        uint32_t tReg = extractBitSlice(instruction, 16, 20);
        int16_t imm = extractBitSlice(instruction, 6, 10);
        printf("%s $%d, $%d, %d", command, dReg, tReg, imm);
    }
    // For commands of bit pattern: OPCODE|00000|ttttt|IIIIIIIIIIIIIIII 
    // Format: command $t, I
    if (strcmp(command, "lui") == 0) {
        uint32_t tReg = extractBitSlice(instruction, 16, 20);
        int16_t imm = extractBitSlice(instruction, 0, 15);
        printf("%s $%d, %d", command, tReg, imm);
    }
    // For commands of bit pattern: OPCODE|bbbbb|ttttt|OOOOOOOOOOOOOOOO
    // Format: command $t, O($b)
    if (strcmp(command, "lb") == 0 ||
        strcmp(command, "lh") == 0 ||
        strcmp(command, "lw") == 0 ||
        strcmp(command, "sb") == 0 ||
        strcmp(command, "sh") == 0 ||
        strcmp(command, "sw") == 0) {
        uint32_t tReg = extractBitSlice(instruction, 16, 20);
        uint32_t offset = extractBitSlice(instruction, 0, 15);
        uint32_t base = extractBitSlice(instruction, 21, 25);
        printf("%s $%d, %d($%d)", command, tReg, offset, base);
    }
    // For commands of bit pattern: OPCODE|sssss|0000X|IIIIIIIIIIIIIIII
    // Format: command $s, I
    if (strcmp(command, "blez") == 0 ||
        strcmp(command, "bgtz") == 0 ||
        strcmp(command, "bltz") == 0 ||
        strcmp(command, "bgez") == 0) {
        uint32_t sReg = extractBitSlice(instruction, 21, 25);
        int16_t imm = extractBitSlice(instruction, 0, 15);
        printf("%s $%d, %d", command, sReg, imm);
    }
    // For commands of bit pattern: OPCODE|XXXXXXXXXXXXXXXXXXXXXXXXXX
    // Format: command X
    if (strcmp(command, "j") == 0 ||
        strcmp(command, "jal") == 0 ) {
        uint32_t target = extractBitSlice(instruction, 0, 25);
        printf("%s 0x%x", command, target);
    }
    // For commands of bit pattern: 000000|sssss|000000000000000|OPCODE
    // Format: command $s
    if (strcmp(command, "jr") == 0) { 
        uint32_t sReg = extractBitSlice(instruction, 21, 25);
        printf("%s $%d", command, sReg);
    } 
    // For commands of bit pattern: 000000|00000000000000000|OPCODE
    // Format: syscall
    if (strcmp(command, "syscall") == 0) {
        printf("%s", command);
    }
}
// =============================================================================
