// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

#ifndef MIPSC_H
#define MIPSC_H

FILE* MIPS;

typedef struct {
    char *name;
    char *addr;
    struct VariableIndex* adj;
} VariableIndex;

extern char* BuildDirectory;

struct VariableIndex* Master;

int InsideFunction = 0;

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

    fprintf(MIPS, ".data\n");

    // loop over each line of IRC.ir
    while (fgets(buf, sizeof(buf), IRCFile)) {

        if (strncmp(buf, "main:", 5) == 0) {
            isInDataSection = 0;
            fprintf(MIPS, "\n.text\nmain:\n");
            continue;
        }

        if(!isInDataSection) {

            char arg1[50], arg2[50], arg3[50];
            char operator;


            if (sscanf(buf, "%s = %[^+*/-]%c%s", arg1, arg2, &operator, arg3) == 4) {
                
            }
            
            //if(sscanf(buf, "%s %s"))

        } else {
            char variableName[50], variableValue[50];
            if (sscanf(buf, "%s word %s", variableName, variableValue) == 2) {
                fprintf(MIPS, "%s .word %s\n", variableName, variableValue);
            }

        }

    }

    fprintf(MIPS, "\nli $v0, 10\n");
    fprintf(MIPS, "syscall\n");

    fclose(IRCFile);
    fclose(MIPS);
}

#endif // MIPSC_H
