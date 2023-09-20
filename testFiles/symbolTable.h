#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct Symbol {
    char* name;
    char* type;
    struct Symbol* next;
} Symbol;

Symbol* symbol_table = NULL;   // Global symbol table

// Function to add a symbol
void add_symbol(char* name, char* type) {
    Symbol* new_symbol = malloc(sizeof(Symbol));
    new_symbol->name = strdup(name);
    new_symbol->type = strdup(type);
    new_symbol->next = symbol_table;
    symbol_table = new_symbol;
}

// Function to lookup a symbol
int lookup_symbol(char* name) {
    Symbol* current = symbol_table;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return 1;  // Found
        }
        current = current->next;
    }
    return 0;  // Not found
}

#endif // SYMBOL_TABLE_H
