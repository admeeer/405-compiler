// math_to_assembly.h

#ifndef MATH_TO_ASSEMBLY_H
#define MATH_TO_ASSEMBLY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    char* code;
    int endRegister;
} AssemblyOutput;


int isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

int precedence(char op) {
    switch (op) {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        default:
            return 0;
    }
}

void infixToPostfix(const char* infix, char* postfix) {
    int i, j = 0, k = 0;
    char stack[strlen(infix)];
    int top = -1;

    for (i = 0; i < strlen(infix); i++) {
        if (isdigit(infix[i]) || isalpha(infix[i])) {
            postfix[j++] = infix[i];
        } else if (infix[i] == '(') {
            stack[++top] = infix[i];
        } else if (infix[i] == ')') {
            while (top != -1 && stack[top] != '(') {
                postfix[j++] = stack[top--];
            }
            if (top != -1 && stack[top] == '(') {
                top--;
            }
        } else {
            while (top != -1 && precedence(infix[i]) <= precedence(stack[top])) {
                postfix[j++] = stack[top--];
            }
            stack[++top] = infix[i];
        }
    }
    while (top != -1) {
        postfix[j++] = stack[top--];
    }
    postfix[j] = '\0';
}

AssemblyOutput generateAssemblyMath(const char* infix, int startRegister) {
    char postfix[strlen(infix)];
    infixToPostfix(infix, postfix);

    char* assemblyCode = (char*)malloc(1000 * sizeof(char));
    assemblyCode[0] = '\0';  // Initialize the string to empty
    int regCounter = startRegister;
    int top = -1;
    char stack[strlen(postfix)][10];

    for (int i = 0; i < strlen(postfix); i++) {
        if (isdigit(postfix[i]) || isalpha(postfix[i])) {
            char temp[10];
            sprintf(temp, "%c", postfix[i]);
            strcpy(stack[++top], temp);
        } else {
            char op1[10], op2[10];
            strcpy(op2, stack[top--]);
            strcpy(op1, stack[top--]);
            char temp[10];
            sprintf(temp, "t%d", regCounter++);
            strcat(assemblyCode, postfix[i] == '+' ? "add " : postfix[i] == '-' ? "sub " : postfix[i] == '*' ? "mul " : "div ");
            strcat(assemblyCode, temp);
            strcat(assemblyCode, ", ");
            strcat(assemblyCode, op1);
            strcat(assemblyCode, ", ");
            strcat(assemblyCode, op2);
            strcat(assemblyCode, "\n");
            strcpy(stack[++top], temp);
        }
    }

    AssemblyOutput output;
    output.code = assemblyCode;
    output.endRegister = regCounter - 1;
    return output;
}

#endif // MATH_TO_ASSEMBLY_H