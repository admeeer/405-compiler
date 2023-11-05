%option noyywrap

%{

#include <stdio.h>
#include <string.h>

#define YY_DECL int yylex()

#include "parser.tab.h"

int words = 0;
int chars = 0;
int lines = 0;

%}

/* track line number */
%option yylineno

/* this pattern matches a string that starts and ends with a double quote and may contain */
/* any characters in between, except for unescaped double-quotes or newlines. It also supports */
/* escape sequences that begin with a backslash followed by any character or a newline */
STRING		"\"([^\"\n]|(\\.))*\"" 

LETTER      [a-zA-Z]
WORD        [a-zA-Z_][a-zA-Z0-9_]*
DIGIT       [0-9]
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
DELIM       [ \t\n]
INTEGER     {DIGIT}+
FLOAT_T      {DIGIT}+(\.{DIGIT}+)?(E[+\-]?(DIGIT)+)?     
CHARACTER  ["'"]({LETTER}|{DIGIT})["'"]


%% 

"/*"    				{
							int c;
							while((c = input()) != 0) {
								if(c == '*') {
									if((c = input()) == '/')
										break;
									else
										unput(c);
								}
							}
						}

"int"	{words++; chars += strlen(yytext);
			printf("%s : INT\n", yytext);
			yylval.string = strdup(yytext); 
			return INT;
		}

"char"	{words++; chars += strlen(yytext);
			printf("%s : CHAR\n", yytext);
			yylval.string = strdup(yytext); 
			return CHAR;
		}

"float"	{words++; chars += strlen(yytext);
			printf("%s : FLOAT\n", yytext);
			yylval.string = strdup(yytext); 
			return FLOAT;
		}

"write"	{words++; chars += strlen(yytext);
			printf("%s : WRITE\n", yytext);
			yylval.string = strdup(yytext); 
			return WRITE;
		}

"writeln" {words++; chars += strlen(yytext);
		 printf("%s : KEYWORD\n", yytext);
         yylval.string = strdup(yytext);
         return WRITELN;
		}

"return" {words++; chars += strlen(yytext);
		 printf("%s : RETURN\n", yytext);
         yylval.string = strdup(yytext);
         return RETURN;
		}

"function"  {words++; chars += strlen(yytext);
		 printf("%s : FUNCTION\n", yytext);
         yylval.string = strdup(yytext);
         return FUNCTION;
		}

"ssstruct" {words++; chars += strlen(yytext);
		 printf("%s : ssstruct\n", yytext);
		 yylval.string = strdup(yytext);
		 return ssstruct;
		}

"if"  {words++; chars += strlen(yytext);
		 printf("%s : IF\n", yytext);
         yylval.string = strdup(yytext);
         return IF;
		}

"while" {words++; chars += strlen(yytext);
		 printf("%s : WHILE\n", yytext);
         yylval.string = strdup(yytext);
         return WHILE;
		}

"else"	{words++; chars += strlen(yytext);
		 printf("%s : ELSE\n", yytext);
         yylval.string =strdup(yytext);
         return ELSE;
		}


{IDENTIFIER}	{words++; chars += strlen(yytext);
			printf("%s : IDENTIFIER\n",yytext);
			yylval.string = strdup(yytext); 
			return IDENTIFIER;
			}
			
{INTEGER}	{words++; chars += strlen(yytext);
				printf("%s : INTEGER\n",yytext);
				yylval.number = atoi(strdup(yytext)); 
				return INTEGER;
			}

{FLOAT_T}     {words++; chars+= strlen(yytext);
                printf("%s : FLOAT_T\n", yytext);
                yylval.floatValue = atof(yytext);
                return FLOAT_T;
            }

{STRING}    {words++; chars+= strlen(yytext);
                printf("%s : STRING\n", yytext);
                yylval.character = strdup(yytext);
                return STRING;
            }

{CHARACTER} {words++; chars+= strlen(yytext);
                printf("%s : CHARACTER\n", yytext);
                yylval.character = strdup(yytext);
                return CHARACTER;
            }

"."		    {chars++;
			printf("%s : DOT \n", yytext);
			yylval.string = strdup(yytext);
			return DOT;
		}


","			{chars++;
			printf("%s : COMMA\n", yytext);
			yylval.string = strdup(yytext);
			return COMMA;
		}

";"			{chars++;
			printf("%s : SEMICOLON\n", yytext);
			yylval.string = strdup(yytext); 
			return SEMICOLON;
		}
		
"="			{chars++;
			printf("%s : EQUALS\n", yytext);
			yylval.string = strdup(yytext); 
			return Equals;
		}

"=="	{chars++;
			printf("%s : BINOP\n", yytext);
			yylval.string = strdup(yytext); 
			return BINOP;
		}

"!="	{chars++;
			printf("%s : BINOP\n", yytext);
			yylval.string = strdup(yytext); 
			return BINOP;
		}

"<"	    {chars++;
			printf("%s : BINOP\n", yytext);
			yylval.string = strdup(yytext); 
			return BINOP;
		}	

">"	{chars++;
			printf("%s : BINOP\n", yytext);
			yylval.string = strdup(yytext); 
			return BINOP;
		}	

">="	{chars++;
			printf("%s : BINOP\n", yytext);
			yylval.string = strdup(yytext); 
			return BINOP;
		}	

"<="	{chars++;
			printf("%s : BINOP\n", yytext);
			yylval.string = strdup(yytext); 
			return BINOP;
		}		
	
"+"	    {chars++;
			printf("%s : ADD\n", yytext);
			yylval.string = strdup(yytext); 
			return ADD;
		}

"-"	{chars++;
			printf("%s : SUBTRACT\n", yytext);
			yylval.string = strdup(yytext); 
			return SUBTRACT;
		}

"*"	{chars++;
			printf("%s : MULTIPLY\n", yytext);
			yylval.string = strdup(yytext); 
			return MULTIPLY;
		}

"/"	{chars++;
			printf("%s : DIVIDE\n", yytext);
			yylval.string = strdup(yytext); 
			return DIVIDE;
		}

"["	{chars++;
			printf("%s : LBRACKET\n", yytext);
			yylval.string = strdup(yytext); 
			return LBRACKET;
		}
	
"]"	{chars++;
			printf("%s : RBRACKET\n", yytext);
			yylval.string = strdup(yytext); 
			return RBRACKET;
		}

"("	{chars++;
			printf("%s : LPAREN\n", yytext);
			yylval.string = strdup(yytext); 
			return LPAREN;
		}

")"	{chars++;
			printf("%s : RPAREN\n", yytext);
			yylval.string = strdup(yytext); 
			return RPAREN;
		}

"{"	{chars++;
			printf("%s : LBRACKET\n", yytext);
			yylval.string = strdup(yytext); 
			return LBRACKET;
		}

"}"	{chars++;
			printf("%s : RBRACKET\n", yytext);
			yylval.string = strdup(yytext); 
			return RBRACKET;
		}

\n		{lines++; chars=0;}
[ \t]	{chars++;}
.		{chars++;
         printf("%s : Unrecognized symbol at line %d char %d\n", yytext,lines,chars);
		}
		


%%


