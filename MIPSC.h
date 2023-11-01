// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan


#ifndef MIPSC_H
#define MIPSC_H

#include "generateAssemblyMath.h"  // Include the header file
FILE* MIPS;
FILE* FUNC;


typedef struct {
    char varName[50];
    char value[50];
} Dictionary;

// Function to get the value from the dictionary based on the varName
char* getValue(Dictionary dict[], int dictCount, char* key) {
    for (int i = 0; i < dictCount; i++) {
        if (strcmp(dict[i].varName, key) == 0) {
            return dict[i].value;
        }
    }
    return NULL; // Return NULL if key is not found
}

char* replaceWithDictValue(char* code, Dictionary dict[], int dictCount) {
    static char newCode[1000]; // Static buffer to hold the result
    char temp[50]; // Temporary buffer to hold each token
    char* delimiters = "*+,/-()"; // Add more special characters if needed
    char* ptr = code;

    strcpy(newCode, ""); // Clear the buffer

    while (*ptr) {
        if (strchr(delimiters, *ptr)) {
            // If the character is a delimiter, append it to the new code
            strncat(newCode, ptr, 1);
            ptr++;
        } else {
            // Extract the next token
            int len = strcspn(ptr, delimiters);
            strncpy(temp, ptr, len);
            temp[len] = '\0';
            char* replacement = getValue(dict, dictCount, temp);
            if (replacement) {
                strcat(newCode, replacement);
            } else {
                strcat(newCode, temp);
            }
            ptr += len;
        }
    }

    return newCode;
}

extern char* BuildDirectory;

void MIPSInitializeFile() {
    char MIPSAbsolutePath[256], FUNCpath[256];
    snprintf(MIPSAbsolutePath, sizeof(MIPSAbsolutePath), "%s/output/asm/MIPS.asm", BuildDirectory);
    snprintf(FUNCpath, sizeof(FUNCpath), "%s/output/asm/FUNC.txt", BuildDirectory);

    MIPS = fopen(MIPSAbsolutePath, "w");
    FUNC = fopen(FUNCpath, "w");

    if (MIPS == NULL) {
        perror("Failed to initialize file MIPS.asm!");
    }
    if(FUNC == NULL){
        perror("Failed to initialize Func file FUNC.txt!");
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

    char MIPSAbsolutePath[256], FUNCpath[256];
    snprintf(MIPSAbsolutePath, sizeof(MIPSAbsolutePath), "%s/output/asm/MIPS.asm", BuildDirectory);
    snprintf(FUNCpath, sizeof(FUNCpath), "%s/output/asm/FUNC.txt", BuildDirectory);

    MIPS = fopen(MIPSAbsolutePath, "w");
    if (MIPS == NULL) {
        perror("Failed to initialize file MIPS.asm!");
        return;
    }
    FUNC = fopen(FUNCpath, "w");
    if (FUNC == NULL){
        perror("Failed to open FUNC.txt Bitches");
    }

    char buf[100];
    int isInDataSection = 1; // Assuming the file starts with the data section
    int isInFunction = 0;
    fprintf(MIPS, ".data\n");

    Dictionary varDict[10];
    int varDictCount = 0;
    int startReg = 0;  // Starting register number for 't'
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
            
            sscanf(buf, "function %s %[^'('](%[^')']){", funcType, funcName, funcVars);
            fprintf(FUNC, "\n%s:\n", funcName);
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
            for (int i = 0; i < tokenCount; i++) {
                char varType[50];
                char varName[50];
                sscanf(tokens[i], "%s %s", varType, varName);
                // Add to dictionary if not existant
                char* searchedVal = getValue(varDict, varDictCount, varName);
                if (!searchedVal) {
                    strcpy(varDict[varDictCount].varName, varName);
                    sprintf(varDict[varDictCount].value, "a%d", i);
                    varDictCount++;
                }

            }   
        }
        else if (strncmp(buf, "}", 1) == 0) {
            isInFunction = 0;
            startReg = 0;
        }
        else if (isInFunction) {
            char varName[20];
            char operands[100];
            if (strncmp(buf, "return", 6) == 0) {
                
                sscanf(buf, "return %[^\n]", operands);

                char* newCode = replaceWithDictValue(operands, varDict, varDictCount);
                AssemblyOutput result = generateAssemblyMath(newCode, startReg);
                fprintf(FUNC, "%s", result.code); // how you get what the line of code is
                fprintf(FUNC, "la $v0, t%d # load int\n\n", result.endRegister); // how you get what register was left off on
                fprintf(FUNC, "jr $ra");
                
            }
            else if (sscanf(buf, "%s = %s", varName, operands) == 2) {
                char* searchedVal = getValue(varDict, varDictCount, varName);
                if (!searchedVal) {
                    strcpy(varDict[varDictCount].varName, varName);
                    sprintf(varDict[varDictCount].value, "$t%d", startReg);
                    startReg = startReg + 1;
                    varDictCount++;
                }
                char* newCode = replaceWithDictValue(operands, varDict, varDictCount);
                AssemblyOutput result = generateAssemblyMath(newCode, startReg);
                fprintf(MIPS, "%s", result.code); // how you get what the line of code is
                fprintf(MIPS, "move %s, t%d # load int\n", getValue(varDict, varDictCount, varName), result.endRegister); // how you get what register was left off on
                startReg = result.endRegister + 1;
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
