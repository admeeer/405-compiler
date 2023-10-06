
// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

FILE * intermediateIRC;
FILE * IRC;

void  IR_initFile() {

    
    intermediateIRC = fopen("intermediateIRC.ir", "w");
    IRC = fopen("IRC.ir", "w");
    
    if(intermediateIRC == NULL) {
        perror("Failed to initialize intermediateIRC file!");
    }

    if(IRC == NULL) {
        perror("Failed to initialize IRC file!");
    }
    
    fprintf(intermediateIRC, "main\n\n");

    fprintf(IRC, "data\n\n");

    fclose(intermediateIRC);

}

void emitIRAssignment(char * id1, char * id2){
    intermediateIRC = fopen("intermediateIRC.ir", "a");

    fprintf(intermediateIRC, "load t1,%s\n", id1);
    fprintf(intermediateIRC, "load t2,%s\n", id2);
    fprintf(intermediateIRC, "load t2, t1\n");

    fclose(intermediateIRC);
}

void emitintermediateIRConstantIntAssignment (int id1, char id2[50]){
    intermediateIRC = fopen("intermediateIRC.ir", "a");

    fprintf(intermediateIRC, "load t%d,%s\n", id1, id2);

    fclose(intermediateIRC);
}

void emitIRWriteInt(int n){
    intermediateIRC = fopen("intermediateIRC.ir", "a");

    fprintf(intermediateIRC, "load v0, 1\n");
    fprintf(intermediateIRC, "load a0, %d\n", n);
    fprintf(intermediateIRC, "system call\n");

    fclose(intermediateIRC);
}

void emitIRWriteId(char id[50], char type[5]){
    intermediateIRC = fopen("intermediateIRC.ir", "a");

    if (strcmp(type, "INT") == 0) {
        fprintf(intermediateIRC, "load v0, 1\n");
        fprintf(intermediateIRC, "load word a0, %s\n", id);
    } 
    else { 
        fprintf(intermediateIRC, "load v0, 4\n");
        fprintf(intermediateIRC, "load address a0, %s\n", id);
    }
    fprintf(intermediateIRC, "system call\n");    

    fclose(intermediateIRC);
}

void emitintermediateIRCharDecl (char id[50], char c) {
    IRC = fopen("IRC.ir", "a");

    fprintf(IRC, "%s byte \'%c\'", id, c);

    fclose(IRC);
}

void emitEndOfAssemblyCodeIR(){
    intermediateIRC = fopen("intermediateIRC.ir", "a ");

    fprintf(intermediateIRC, "load v0, 10\n");
    fprintf(intermediateIRC, "system call\n");

    fclose(intermediateIRC);    
}

void emitIntVarIR(char id[50], int val) {

    IRC = fopen("IRC.ir", "a ");

    fprintf(IRC, "%s: word  %d\n", id, val);

    fclose(IRC); 

}

void addMainToDataIR() {

    IRC = fopen("IRC.ir", "a+");
    intermediateIRC = fopen("intermediateIRC.ir", "a+");
    //funcs = fopen("funcs.ir", "a+");
 
    // If file is not found then return.
    if (!IRC || !intermediateIRC) {
        printf("Error: IRC or intermediateIRC never initialized!");
        return;
    }
 
    char buf[100];

    fprintf(IRC, "\n");
 
    while (!feof(intermediateIRC)) {
        fgets(buf, sizeof(buf), intermediateIRC);
        fprintf(IRC, "%s", buf);
    }

    fprintf(IRC, "\n");

    fclose(IRC);

    //remove(intermediateIRC);
    
}  