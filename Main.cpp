#include "Main.h"

int main()
{
    char* operation = NULL;
    uint32_t instruction;

    instruction = 0b00000000000000011101001010110111;
    operation = decodeInstruction(instruction);
    if (operation != NULL) {
        printf("%s\n", operation);
        assert(strcmp(operation, "lui rd=5, imm=29") == 0);
        free(operation);
    }
    else {
        printf("Error: operation is NULL\n");
    }

    instruction = 0b00000000000000011000001010010111;
    operation = decodeInstruction(instruction);
    if (operation != NULL) {
        printf("%s\n", operation);
        assert(strcmp(operation, "auipc rd=5, imm=24") == 0);
        free(operation);
    }
    else {
        printf("Error: operation is NULL\n");
    }

    instruction = 0b00000000000000011000001011101111;
    operation = decodeInstruction(instruction);
    if (operation != NULL) {
        printf("%s\n", operation);
        assert(strcmp(operation, "jal rd=5, imm=98304") == 0);
        free(operation);
    }
    else {
        printf("Error: operation is NULL\n");
    }

    return 0;
}