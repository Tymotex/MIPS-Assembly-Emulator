#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "emu.h"
#include "ram.h"
#include "registers.h"
#include "bitextract.h"

// ======================== My Helper Functions ================================
// These functions determine if a given command belongs to the correct 
// class of commands. Eg. isMath("add") returns 1, isBranch("mul") returns 0
static int isMath(char *command);

static int isLoadOrStore(char *command);

static int isBranch(char *command);

static int isJump(char *command);

static int isSyscall(char *command);

// These functions determine the operands and carry out the given command
static void mathOps(uint32_t instruction, char *command, uint32_t *program_counter);

static void loadOrStoreOps(uint32_t instruction, char *command, uint32_t *program_counter);

static void branchOps(uint32_t instruction, char *command, uint32_t *program_counter); 

static void jumpOps(uint32_t instruction, char *command, uint32_t *program_counter);

static void syscall(uint32_t instruction, char *command, uint32_t *program_counter);
// =============================================================================
int execute_instruction(uint32_t instruction, uint32_t *program_counter) {
    char *command = getCommand(instruction);
    if (isMath(command)) {
        mathOps(instruction, command, program_counter);
        (*program_counter) += 4;
    } else  if (isLoadOrStore(command)) {
        loadOrStoreOps(instruction, command, program_counter);
        (*program_counter) += 4;
    } else if (isSyscall(command)) {
        syscall(instruction, command, program_counter);
        (*program_counter) += 4;
    } else if (isBranch(command)) {
        branchOps(instruction, command, program_counter);
    } else if (isJump(command)) {
        (*program_counter) += 4;
    }
    free(command);
    return 0;
}
// =============================================================================
static int isMath(char *command) {
    return (
        strcmp(command, "add") == 0 ||
        strcmp(command, "sub") == 0 ||
        strcmp(command, "mul") == 0 ||
        strcmp(command, "and") == 0 ||
        strcmp(command, "or") == 0 ||
        strcmp(command, "xor") == 0 ||
        strcmp(command, "slt") == 0 ||
        strcmp(command, "sllv") == 0 ||
        strcmp(command, "srlv") == 0 ||
        strcmp(command, "addi") == 0 ||
        strcmp(command, "andi") == 0 ||
        strcmp(command, "addi") == 0 ||
        strcmp(command, "ori") == 0 ||
        strcmp(command, "xori") == 0 ||
        strcmp(command, "slti") == 0 ||
        strcmp(command, "sll") == 0 ||
        strcmp(command, "srl") == 0
    );
}

static int isLoadOrStore(char *command) {
    return (
        strcmp(command, "lui") == 0 ||
        strcmp(command, "lb") == 0 ||
        strcmp(command, "lh") == 0 ||
        strcmp(command, "lw") == 0 ||
        strcmp(command, "sb") == 0 ||
        strcmp(command, "sh") == 0 ||
        strcmp(command, "sw") == 0
    );
}

static int isBranch(char *command) {
    return (
        strcmp(command, "beq") == 0 ||
        strcmp(command, "bne") == 0 ||
        strcmp(command, "blez") == 0 ||
        strcmp(command, "bgtz") == 0 ||
        strcmp(command, "bltz") == 0 ||
        strcmp(command, "bgez") == 0
    );
}

static int isJump(char *command) {
    return (
        strcmp(command, "j") == 0 ||
        strcmp(command, "jal") == 0 ||
        strcmp(command, "jr") == 0
    );
}

static int isSyscall(char *command) {
    return (strcmp(command, "syscall") == 0);
}

static void mathOps(uint32_t instruction, char *command, uint32_t *program_counter) {
    uint32_t dReg = extractBitSlice(instruction, 11, 15);
    uint32_t sReg = extractBitSlice(instruction, 21, 25);
    uint32_t tReg = extractBitSlice(instruction, 16, 20);
    int16_t imm = extractBitSlice(instruction, 0, 15);
    int16_t immShiftAmount = extractBitSlice(instruction, 6, 10);
    int32_t sContents = get_register(sReg);
    int32_t tContents = get_register(tReg);
    
    if (strcmp(command, "add") == 0) {
        set_register(dReg, sContents + tContents);
    } else if (strcmp(command, "sub") == 0) {
        set_register(dReg, sContents - tContents);
    } else if (strcmp(command, "mul") == 0) {
        set_register(dReg, sContents * tContents);
    } else if (strcmp(command, "and") == 0) {
        set_register(dReg, sContents & tContents);
    } else if (strcmp(command, "or") == 0) {
        set_register(dReg, sContents | tContents);
    } else if (strcmp(command, "xor") == 0) {
        set_register(dReg, sContents ^ tContents);
    } else if (strcmp(command, "slt") == 0) {
        set_register(dReg, (sContents < tContents));
    } else if (strcmp(command, "sllv") == 0) {
        set_register(dReg, tContents << sContents);
    } else if (strcmp(command, "srlv") == 0) {
        set_register(dReg, tContents >> sContents);
    } else if (strcmp(command, "addi") == 0) {
        set_register(tReg, sContents + imm);
    } else if (strcmp(command, "andi") == 0) {
        set_register(tReg, sContents & imm);
    } else if (strcmp(command, "ori") == 0) {
        set_register(tReg, sContents | imm);
    } else if (strcmp(command, "xori") == 0) {
        set_register(tReg, sContents ^ imm);
    } else if (strcmp(command, "slti") == 0) {
        set_register(tReg, (sContents < imm));
    } else if (strcmp(command, "sll") == 0) { 
        set_register(dReg, ((uint32_t)tContents << (uint32_t)immShiftAmount));
    } else if (strcmp(command, "srl") == 0) {  
        set_register(dReg, (tContents >> immShiftAmount));
    }
}

static void loadOrStoreOps(uint32_t instruction, char *command, uint32_t *program_counter) {
    uint32_t tReg = extractBitSlice(instruction, 16, 20);
    uint32_t baseReg = extractBitSlice(instruction, 21, 25);
    int16_t imm = extractBitSlice(instruction, 0, 15);
    int16_t offset = extractBitSlice(instruction, 0, 15);
    int32_t tContents = get_register(tReg);
    uint32_t baseContents = get_register(baseReg);

    if (strcmp(command, "lui") == 0) {
        set_register(tReg, imm << 16);
    } else if (strcmp(command, "lb") == 0) {
        uint32_t result = get_byte(baseContents + offset);
        if (result & 0x00000080) {
            // Byte has a leading one, so pad the bits with 1s
            result = padWithOnes(result);
        }
        set_register(tReg, result);
    } else if (strcmp(command, "lh") == 0) {
        uint32_t firstByte =  (uint32_t)get_byte(baseContents + offset);
        uint32_t secondByte = (uint32_t)get_byte(baseContents + offset + 1);
        uint32_t result = firstByte | (secondByte << 8);
        if (result & 0x00008000) {
            // Half word has a leading one, so pad the bits with 1s
            result = padWithOnes(result);
        }
        set_register(tReg, result);
    } else if (strcmp(command, "lw") == 0) {
        uint32_t firstByte =  (uint32_t)get_byte(baseContents + offset);
        uint32_t secondByte = (uint32_t)get_byte(baseContents + offset + 1);
        uint32_t thirdByte =  (uint32_t)get_byte(baseContents + offset + 2);
        uint32_t fourthByte = (uint32_t)get_byte(baseContents + offset + 3);
        uint32_t result = firstByte | (secondByte << 8) | (thirdByte << 16) | (fourthByte << 24);
        set_register(tReg, result);
    } else if (strcmp(command, "sb") == 0) {
        set_byte(offset + baseContents, tContents);
    } else if (strcmp(command, "sh") == 0) {
        set_byte(offset + baseContents, tContents);
        set_byte(offset + baseContents + 1, tContents >> 8);
    } else if (strcmp(command, "sw") == 0) {
        set_byte(offset + baseContents, tContents);
        set_byte(offset + baseContents + 1, tContents >> 8);
        set_byte(offset + baseContents + 2, tContents >> 16);
        set_byte(offset + baseContents + 3, tContents >> 24);
    }
}

static void branchOps(uint32_t instruction, char *command, uint32_t *program_counter) {
    uint32_t sReg = extractBitSlice(instruction, 21, 25);
    uint32_t tReg = extractBitSlice(instruction, 16, 20);
    int16_t imm = extractBitSlice(instruction, 0, 15);
    int32_t sContents = get_register(sReg);
    int32_t tContents = get_register(tReg);
    
    if (strcmp(command, "beq") == 0) {
        if (sContents == tContents) {
            (*program_counter) += imm * 4;
        } else {
            (*program_counter) += 4;
        }
    } else if (strcmp(command, "bne") == 0) {
        if (sContents != tContents) {
            (*program_counter) += imm * 4;
        } else {
            (*program_counter) += 4;
        }
    } else if (strcmp(command, "blez") == 0) {
        if (sContents <= 0) {
            (*program_counter) += imm * 4;
        } else {
            (*program_counter) += 4;
        }
    } else if (strcmp(command, "bgtz") == 0) {
        if (sContents > 0) {
            (*program_counter) += imm * 4;
        } else {
            (*program_counter) += 4;
        }
    } else if (strcmp(command, "bltz") == 0) {
        if (sContents < 0) {
            (*program_counter) += imm * 4;
        } else {
            (*program_counter) += 4;
        }
    } else if (strcmp(command, "bgez") == 0) {
        if (sContents >= 0) {
            (*program_counter) += imm * 4;
        } else {
            (*program_counter) += 4;
        }
    } 
}

static void jumpOps(uint32_t instruction, char *command, uint32_t *program_counter) {
    uint32_t jumpAddress = extractBitSlice(instruction, 21, 25);
    uint32_t sReg = extractBitSlice(instruction, 21, 25); 

    if (strcmp(command, "j") == 0) {
        (*program_counter) = ((*program_counter) & 0xF0000000) | (jumpAddress * 4);
    } else if (strcmp(command, "jal") == 0) {
        set_register(ra, (*program_counter) + 4);
        (*program_counter) = (*program_counter) + (jumpAddress * 4);
    } else if (strcmp(command, "jr") == 0) {
        (*program_counter) = get_register(sReg);
    }
}

static void syscall(uint32_t instruction, char *command, uint32_t *program_counter) {
    uint32_t service = get_register(v0);
    uint32_t arg1 = get_register(4);
    uint32_t arg2 = get_register(5);
    
    if (service == 1) {
        printf("%d", arg1);
    } else if (service == 4) {
        for (int i = 0; get_byte(arg1 + i) != '\0'; i++) {
            printf("%c", get_byte(arg1 + i));
        }
    } else if (service == 5) {
        int32_t input;
        scanf("%d", &input);
        set_register(v0, input);
    } else if (service == 8) {
        for (int i = 0; i < arg2; i++) {
            uint8_t input = (uint8_t)getchar();
            set_byte(arg1 + i, input);
        }
    } else if (service == 10) {
        exit(EXIT_SUCCESS);
    } else if (service == 11) {
        printf("%c", arg1);
    } else if (service == 12) {
        int input = getchar();
        set_register(v0, input);
    } 
}
// =============================================================================
