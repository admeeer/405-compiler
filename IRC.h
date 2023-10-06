
// CST 405 Alexander Peltier, Matthew Powers, Parker Spaan

FILE * IntermediateRepresentationCode;
FILE * IntermediateRepresentationCodeData;


void IRInitializeFile() {

    IntermediateRepresentationCode = fopen("IntermediateRepresentationCode.ir", "w");

    if(IntermediateRepresentationCode == NULL) {
        perror("Failed to initialize file IntermediateRepresentationCode !");
    }

    IntermediateRepresentationCodeData = fopen("IntermediateRepresentationCodeData.ir", "w");

    if(IntermediateRepresentationCodeData == NULL) {
        perror("Failed to initialize file IntermediateRepresentationCodeData !");
    }

    fprintf(IntermediateRepresentationCode, "main\n\n");

    fprintf(IntermediateRepresentationCodeData, "data\n\n");

    fclose(IntermediateRepresentationCode);

    fclose(IntermediateRepresentationCodeData);
    
}

void  IR_initFile() {

    
    IntermediateRepresentationCode = fopen("IntermediateRepresentationCode.ir", "w");
    IntermediateRepresentationCodeData = fopen("IntermediateRepresentationCodeData.ir", "w");
    
    if(IntermediateRepresentationCode == NULL) {
        perror("Failed to initialize IntermediateRepresentationCode file!");
    }

    if(IntermediateRepresentationCodeData == NULL) {
        perror("Failed to initialize IntermediateRepresentationCodeData file!");
    }
    
    fprintf(IntermediateRepresentationCode, "main\n\n");

    fprintf(IntermediateRepresentationCodeData, "data\n\n");

    fclose(IntermediateRepresentationCode);

}

void emitIRAssignment(char * id1, char * id2){
    IntermediateRepresentationCode = fopen("IntermediateRepresentationCode.ir", "a");

    fprintf(IntermediateRepresentationCode, "load t1,%s\n", id1);
    fprintf(IntermediateRepresentationCode, "load t2,%s\n", id2);
    fprintf(IntermediateRepresentationCode, "load t2, t1\n");

    fclose(IntermediateRepresentationCode);
}

void emitIntermediateRepresentationCodeonstantIntAssignment (int id1, char id2[50]){
    IntermediateRepresentationCode = fopen("IntermediateRepresentationCode.ir", "a");

    fprintf(IntermediateRepresentationCode, "load t%d,%s\n", id1, id2);

    fclose(IntermediateRepresentationCode);
}

void emitIRWriteInt(int n){
    IntermediateRepresentationCode = fopen("IntermediateRepresentationCode.ir", "a");

    fprintf(IntermediateRepresentationCode, "load v0, 1\n");
    fprintf(IntermediateRepresentationCode, "load a0, %d\n", n);
    fprintf(IntermediateRepresentationCode, "system call\n");

    fclose(IntermediateRepresentationCode);
}

void emitIRWriteId(char id[50], char type[5]){
    IntermediateRepresentationCode = fopen("IntermediateRepresentationCode.ir", "a");

    if (strcmp(type, "INT") == 0) {
        fprintf(IntermediateRepresentationCode, "load v0, 1\n");
        fprintf(IntermediateRepresentationCode, "load word a0, %s\n", id);
    } 
    else { 
        fprintf(IntermediateRepresentationCode, "load v0, 4\n");
        fprintf(IntermediateRepresentationCode, "load address a0, %s\n", id);
    }
    fprintf(IntermediateRepresentationCode, "system call\n");    

    fclose(IntermediateRepresentationCode);
}

void emitIntermediateRepresentationCodeharDecl (char id[50], char c) {
    IntermediateRepresentationCodeData = fopen("IntermediateRepresentationCodeData.ir", "a");

    fprintf(IntermediateRepresentationCodeData, "%s byte \'%c\'", id, c);

    fclose(IntermediateRepresentationCodeData);
}

void emitEndOfAssemblyCodeIR(){
    IntermediateRepresentationCode = fopen("IntermediateRepresentationCode.ir", "a ");

    fprintf(IntermediateRepresentationCode, "load v0, 10\n");
    fprintf(IntermediateRepresentationCode, "system call\n");

    fclose(IntermediateRepresentationCode);    
}

void emitIntVarIR(char id[50], int val) {

    IntermediateRepresentationCodeData = fopen("IntermediateRepresentationCodeData.ir", "a ");

    fprintf(IntermediateRepresentationCodeData, "%s: word  %d\n", id, val);

    fclose(IntermediateRepresentationCodeData); 

}

void addMainToDataIR() {

    IntermediateRepresentationCodeData = fopen("IntermediateRepresentationCodeData.ir", "a+");
    IntermediateRepresentationCode = fopen("IntermediateRepresentationCode.ir", "a+");
    //funcs = fopen("funcs.ir", "a+");
 
    // If file is not found then return.
    if (!IntermediateRepresentationCodeData || !IntermediateRepresentationCode) {
        printf("Error: IntermediateRepresentationCodeData or IntermediateRepresentationCode never initialized!");
        return;
    }
 
    char buf[100];

    fprintf(IntermediateRepresentationCodeData, "\n");
 
    while (!feof(IntermediateRepresentationCode)) {
        fgets(buf, sizeof(buf), IntermediateRepresentationCode);
        fprintf(IntermediateRepresentationCodeData, "%s", buf);
    }

    fprintf(IntermediateRepresentationCodeData, "\n");

    fclose(IntermediateRepresentationCodeData);

    //remove(IntermediateRepresentationCode);
    
}  