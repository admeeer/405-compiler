
FILE * MIPS;

void MIPSInitializeFile() {

    MIPS = fopen("MIPS.asm", "w");

    if(MIPS == NULL) {
        perror("Failed to initialize file MIPS.asm!");
    }

    fclose(MIPS);
    
}

void MIPSEmission() {
    FILE* IRCFile = fopen("IRC.ir", "r");
    if (!IRCFile) {
        perror("Failed to open IRC.ir for reading!");
        return;
    }

    MIPS = fopen("MIPS.asm", "w");
    if (MIPS == NULL) {
        perror("Failed to initialize file MIPS.asm!");
        return;
    }

    char buf[100];
    int isInDataSection = 1; // Assuming the file starts with the data section

    fprintf(MIPS, ".data\n");

    while (fgets(buf, sizeof(buf), IRCFile)) {
        if (strncmp(buf, "main:", 5) == 0) {
            isInDataSection = 0;
            fprintf(MIPS, "\n.text\nmain:\n");
            continue;
        }

        if (isInDataSection) {
            char varName[50], value[50];
            if (sscanf(buf, "%[^:]: word %s", varName, value) == 2) {
                fprintf(MIPS, "%s: .word %s\n", varName, value);
            }
        } else {
            char instruction[10], operand[50];
            if (sscanf(buf, "%s %s", instruction, operand) == 2) {
                if (strcmp(instruction, "write") == 0) {
                    fprintf(MIPS, "li $v0, 1       # syscall to print int\n");
                    fprintf(MIPS, "la $a0, %s # load int\n", operand);
                    fprintf(MIPS, "syscall\n");
                }
            }
        }
    }

    fprintf(MIPS, "\nli $v0, 10 # exit\n");
    fprintf(MIPS, "syscall\n");

    fclose(IRCFile);
    fclose(MIPS);
}