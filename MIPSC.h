// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan


#ifndef MIPSC_H
#define MIPSC_H
#include "generateAssemblyMath.h"  // Include the header file
FILE* MIPS;

extern char* BuildDirectory;

void MIPSInitializeFile() {
    char MIPSAbsolutePath[256];
    snprintf(MIPSAbsolutePath, sizeof(MIPSAbsolutePath), "%s/output/asm/MIPS.asm", BuildDirectory);

    MIPS = fopen(MIPSAbsolutePath, "w");

    if (MIPS == NULL) {
        perror("Failed to initialize file MIPS.asm!");
    }

    fclose(MIPS);
}

void MIPSEmission() {

    char IRCAbsolutePath[256];

     snprintf(IRCAbsolutePath, sizeof(IRCAbsolutePath), "%s/output/irc/IRC.ir", BuildDirectory);

    FILE* IRCFile = fopen(IRCAbsolutePath, "r");
    if (!IRCFile) {
        perror("Failed to open IRC.ir for reading!");
        return;
    }

    char MIPSAbsolutePath[256];
    snprintf(MIPSAbsolutePath, sizeof(MIPSAbsolutePath), "%s/output/asm/MIPS.asm", BuildDirectory);

    MIPS = fopen(MIPSAbsolutePath, "w");
    if (MIPS == NULL) {
        perror("Failed to initialize file MIPS.asm!");
        return;
    }

    char buf[100];
    int isInDataSection = 1; // Assuming the file starts with the data section
    int isInFunction = 0;
    fprintf(MIPS, ".data\n");

    while (fgets(buf, sizeof(buf), IRCFile)) {
        char varName[32];
        char funcName[32];
        char parameters[64];
        if (strncmp(buf, "main:", 5) == 0) {
            isInDataSection = 0;
            fprintf(MIPS, "\n.text\nmain:\n");
            continue;
        }
        else if (isInDataSection) {
            char varName[50], value[50];
            if (sscanf(buf, "%[^:]: word %s", varName, value) == 2) {
                fprintf(MIPS, "%s: .word %s\n", varName, value);
            }
        }
        else if (strncmp(buf, "function", 8) == 0) {
            isInFunction = 1;
            char funcType[50];
            char funcName[50];
            char funcVars[50];
            
            sscanf(buf, "function %s %s (%s){", funcType, funcName, funcVars);
            fprintf(MIPS, "\n%s:\n", funcName);
            
            char *tokens[50]; // Array to store split values
            int tokenCount = 0;

            // Split based on ","
            char *token = strtok(funcVars, ",");
            while (token != NULL) {
                // Trim leading spaces
                while (*token == ' ') {
                    token++;
                }
                tokens[tokenCount++] = token;
                token = strtok(NULL, ",");
            }
            // Process each value in the array
            // for (int i = 0; i < tokenCount; i++) {
            //     char varType[50];
            //     char varName[50];
            //     sscanf("%s %s", varType, varName);
            // }
            // continue;
            
        }
        else if (strncmp(buf, "}", 1) == 0) {
            isInFunction = 0;
        }
        else if (isInFunction) {
            if (strncmp(buf, "return", 6) == 0) {
                char operand[200];
                sscanf(buf, "return %[^\n]", operand);
                int startReg = 0;  // Starting register number for 't'
                
                AssemblyOutput result = generateAssemblyMath(operand, startReg);
                fprintf(MIPS, "%s", result.code); // how you get what the line of code is
                fprintf(MIPS, "la $a0, t%d # load int\n\n", result.endRegister); // how you get what register was left off on
                fprintf(MIPS, "jr $ra");
            
            
            }
        }
        else if (strncmp(buf, "write", 5) == 0) {
            char operand[50];
            sscanf(buf, "write %s", operand);
            fprintf(MIPS, "la $a0, %s # load int\n", operand);
            fprintf(MIPS, "syscall\n");
            fprintf(MIPS, "li $v0, 1       # syscall to print int\n");
            }
        
        else if (sscanf(buf, "%s = %[^'('](%[^')'])", varName, funcName, parameters) == 3) {
                
                char *token = strtok(parameters, ",");
                int paramCount = 3;
                fprintf(MIPS, "# saving our values before we jump.\n");
                while (token != NULL){
                    while (*token == ' ' || *token == '\t'){
                        token ++;
                    }
                    fprintf(MIPS, "la $a%d, %s\n", paramCount, token);
                    paramCount--;
                    token = strtok(NULL, ",");
                    
                }
                
                fprintf(MIPS, "jal %s\n", funcName);
                fprintf(MIPS, "sw $v0, %s\n", varName);
                
            }
        else{
            printf("NO IDEA WHAT TO DO WITH THIS LINE: %s", buf);

            // sscanf(buf, "%s = %s(%s)", varName, funcName, parameters);
            // printf("got here  %s-%s-%s", varName, funcName, parameters);

        }
        }

    fprintf(MIPS, "\nli $v0, 10 # exit\n");
    fprintf(MIPS, "syscall\n");

    fclose(IRCFile);
    fclose(MIPS);
}

#endif // MIPSC_H
