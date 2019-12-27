#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitextract.h"

// =============================================================================
void printBits(uint32_t input) {
    int position = 1;
    for (int i = 8 * sizeof(input) - 1; i >= 0; i--, position++) {
        printf("%d", (input >> i) & 1u);
    }
    printf("\n");
}

void printHex(uint32_t input) {
    printf("0x");
    char *hexMap = "0123456789ABCDEF";
    for (int i = (2 * sizeof(input) - 1); i >= 0; i--) {
        printf("%c", hexMap[(input >> (i * 4)) & 0x0000000F]);
    }
    printf("\n");
}

char *getCommand(uint32_t instruction) {
    char *command = NULL;
    //printBits(instruction, 1);
    int leftSix = instruction >> 26;
    // If the left six bits aren't 000000, then the instruction is uniquely identifiable:
    if (leftSix != 0u) {
        switch (leftSix) {
            case 0x0000001C:
                command = malloc(sizeof(char) * (strlen("mul") + 1)); 
                strcpy(command, "mul");
                return command;
            case 0x00000008:
                command = malloc(sizeof(char) * (strlen("addi") + 1)); 
                strcpy(command, "addi");
                return command;
            case 0x0000000C:
                command = malloc(sizeof(char) * (strlen("andi") + 1)); 
                strcpy(command, "andi");
                return command;
            case 0x0000000D:
                command = malloc(sizeof(char) * (strlen("ori") + 1)); 
                strcpy(command, "ori");
                return command;
            case 0x0000000E:
                command = malloc(sizeof(char) * (strlen("xori") + 1)); 
                strcpy(command, "xori");
                return command;
            case 0x0000000A:
                command = malloc(sizeof(char) * (strlen("slti") + 1)); 
                strcpy(command, "slti");
                return command;
            case 0x0000000F:
                command = malloc(sizeof(char) * (strlen("lui") + 1)); 
                strcpy(command, "lui");
                return command;
            case 0x00000020:
                command = malloc(sizeof(char) * (strlen("lb") + 1)); 
                strcpy(command, "lb");
                return command;
            case 0x00000021:
                command = malloc(sizeof(char) * (strlen("lh") + 1)); 
                strcpy(command, "lh");
                return command;
            case 0x00000023:
                command = malloc(sizeof(char) * (strlen("lw") + 1)); 
                strcpy(command, "lw");
                return command;
            case 0x00000028:
                command = malloc(sizeof(char) * (strlen("sb") + 1)); 
                strcpy(command, "sb");
                return command;
            case 0x00000029:
                command = malloc(sizeof(char) * (strlen("sh") + 1)); 
                strcpy(command, "sh");
                return command;
            case 0x0000002B:
                command = malloc(sizeof(char) * (strlen("sw") + 1)); 
                strcpy(command, "sw");
                return command;
            case 0x00000004:
                command = malloc(sizeof(char) * (strlen("beq") + 1)); 
                strcpy(command, "beq");
                return command;
            case 0x00000005:
                command = malloc(sizeof(char) * (strlen("bne") + 1)); 
                strcpy(command, "bne");
                return command;
            case 0x00000006:
                command = malloc(sizeof(char) * (strlen("blez") + 1)); 
                strcpy(command, "blez");
                return command;
            case 0x00000007:
                command = malloc(sizeof(char) * (strlen("bgtz") + 1)); 
                strcpy(command, "bgtz");
                return command;
            case 0x00000002:
                command = malloc(sizeof(char) * (strlen("j") + 1)); 
                strcpy(command, "j");
                return command;
            case 0x00000003:
                command = malloc(sizeof(char) * (strlen("jal") + 1)); 
                strcpy(command, "jal");
                return command;
        }
    }
    
    // If the left six bits ARE 000000, then scan the last 6 bits
    if (leftSix == 0u) {
        int rightSix = instruction & 0x0000003F; 
        // If the left six bits are: 000000, then 
        switch (rightSix) {
            case 0x00000020:
                command = malloc(sizeof(char) * (strlen("add") + 1)); 
                strcpy(command, "add");
                return command;
            case 0x00000022:
                command = malloc(sizeof(char) * (strlen("sub") + 1)); 
                strcpy(command, "sub");
                return command;
            case 0x00000024:
                command = malloc(sizeof(char) * (strlen("and") + 1)); 
                strcpy(command, "and");
                return command;
            case 0x00000025:
                command = malloc(sizeof(char) * (strlen("or") + 1)); 
                strcpy(command, "or");
                return command;
            case 0x00000026:
                command = malloc(sizeof(char) * (strlen("xor") + 1)); 
                strcpy(command, "xor");
                return command;
            case 0x00000004:
                command = malloc(sizeof(char) * (strlen("sllv") + 1)); 
                strcpy(command, "sllv");
                return command;
            case 0x00000006:
                command = malloc(sizeof(char) * (strlen("srlv") + 1)); 
                strcpy(command, "srlv");
                return command;
            case 0x0000002A:
                command = malloc(sizeof(char) * (strlen("slt") + 1)); 
                strcpy(command, "slt");
                return command;
            case 0x00000000:
                command = malloc(sizeof(char) * (strlen("sll") + 1)); 
                strcpy(command, "sll");
                return command;
            case 0x00000002:
                command = malloc(sizeof(char) * (strlen("srl") + 1)); 
                strcpy(command, "srl");
                return command;
            case 0x00000008:
                command = malloc(sizeof(char) * (strlen("jr") + 1)); 
                strcpy(command, "jr");
                return command;
            case 0x0000000C:
                command = malloc(sizeof(char) * (strlen("syscall") + 1)); 
                strcpy(command, "syscall");
                return command;
        }
    }
    
    // If the left six bits are 000001, then scan bits 16 to 20
    // If the program execution reaches here, the command is either blez or bgez
    int midFive = (instruction >> 16) & 0x0000001F;
    switch (midFive)  {
        case 0x00000000:
            command = malloc(sizeof(char) * (strlen("bltz") + 1)); 
            strcpy(command, "bltz");
            return command;
        case 0x00000001:
            command = malloc(sizeof(char) * (strlen("bgez") + 1)); 
            strcpy(command, "bgez");
            return command;
    }
    return NULL;
}

uint32_t extractBitSlice(uint32_t instruction, uint32_t from, uint32_t to) {
    uint32_t bitSlice = 0x00000000;
    uint32_t one = 0x00000001;
    for (int i = to; i >= (int)from; i--) {
        uint32_t currentBit = (instruction >> i) & one;
        bitSlice = bitSlice | (currentBit << i);
    }
    return (bitSlice >> from);
}

uint32_t padWithOnes(uint32_t instruction) {
    int32_t n = 0;
    for (int i = 0; i < 8 * sizeof(instruction); i++) {
        if (((instruction >> i) & 0x00000001) == 0x00000001) {
            n = i;
        }
    }
    uint32_t pad = 0x00000000;
    uint32_t one = 0x00000001;
    for (int i = 8 * sizeof(instruction) - 1; i > n; i--) {
        pad = pad | (one << i);
    }
    return (instruction | pad);
}
// =============================================================================
