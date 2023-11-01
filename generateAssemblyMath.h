#ifndef generateAssemblyMath_H
#define generateAssemblyMath_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* generateAssemblyMath(const char* infix) {
    #define MAX_SIZE 100

    typedef struct {
        int data[MAX_SIZE];
        int top;
    } Stack;

    static char result[1000]; // Static to allow returning it
    char postfix[MAX_SIZE];
    int tempCounter = 0;
    int resultIndex = 0;

    // Stack functions
    void init(Stack *s) {
        s->top = -1;
    }

    int isEmpty(Stack *s) {
        return s->top == -1;
    }

    void push(Stack *s, int item) {
        s->data[++s->top] = item;
    }

    int pop(Stack *s) {
        return s->data[s->top--];
    }

    int peek(Stack *s) {
        return s->data[s->top];
    }

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

    const char* opToString(char op) {
        switch (op) {
            case '+': return "add";
            case '-': return "sub";
            case '*': return "mul";
            case '/': return "div";
            default: return "";
        }
    }

    void printValue(int val) {
        if (val >= 'a' && val <= 'z') {
            result[resultIndex++] = val;
        } else {
            sprintf(result + resultIndex, "t%d", val - '0');
            while (result[resultIndex] != '\0') {
                resultIndex++;
            }
        }
    }

    void infixToPostfix(const char *infix, char *postfix) {
        Stack s;
        init(&s);
        int j = 0;
        for (int i = 0; i < strlen(infix); i++) {
            if (isalpha(infix[i]) || isdigit(infix[i])) {
                postfix[j++] = infix[i];
            } else if (isOperator(infix[i])) {
                while (!isEmpty(&s) && precedence(peek(&s)) >= precedence(infix[i])) {
                    postfix[j++] = pop(&s);
                }
                push(&s, infix[i]);
            }
        }
        while (!isEmpty(&s)) {
            postfix[j++] = pop(&s);
        }
        postfix[j] = '\0';
    }

    void process(const char *postfix) {
        Stack s;
        init(&s);
        for (int i = 0; i < strlen(postfix); i++) {
            if (isalpha(postfix[i]) || isdigit(postfix[i])) {
                push(&s, postfix[i]);
            } else if (isOperator(postfix[i])) {
                int val2 = pop(&s);
                int val1 = pop(&s);
                sprintf(result + resultIndex, "%s t%d, ", opToString(postfix[i]), tempCounter);
                while (result[resultIndex] != '\0') {
                    resultIndex++;
                }
                printValue(val1);
                result[resultIndex++] = ',';
                result[resultIndex++] = ' ';
                printValue(val2);
                result[resultIndex++] = '\n';
                push(&s, '0' + tempCounter);
                tempCounter++;
            }
        }
    }

    // Main logic
    infixToPostfix(infix, postfix);
    process(postfix);
    result[resultIndex] = '\0'; // Null-terminate the result string

    return result;
}


#endif // generateAssemblyMath_H