%{
#include <stdio.h>
#include <stdlib.h>

extern int yylex(void);
extern int yyerror(const char *s);

FILE *yyin;  // Declare yyin as a global file pointer
FILE *output;
%}

%union {
    int num;    // For storing numbers
}

%token <num> NUMBER
%token ASTERISK
%token PLUS
%token SEMICOLON

%start program

%%

program:
    vars stmts SEMICOLON
    ;

vars:
    NUMBER  { fprintf(output, "\ta = %d;\n", $1); }  // Printing the variable value
    ;

stmts:
    multstmts addstmts
    ;

multstmts:
    /* Empty */
    | multstmts ASTERISK NUMBER  { fprintf(output, "\ta *= %d;\n", $3); }
    ;

addstmts:
    /* Empty */
    | addstmts PLUS NUMBER  { fprintf(output, "\ta += %d;\n", $3); }
    ;

%%

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Open the file provided as a command-line argument
    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Error opening file");
        return 1;
    }
    output = fopen("a.c", "w");
    fprintf(output, "#include<stdio.h>\n int main(){ \n\tint a;\n");

    yyparse();  // Start parsing
    fclose(yyin);  // Close the file after parsing
    fprintf(output, "\treturn 0; \n}");
    fclose(output);
    return 0;
}

int yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
    return 0;
}
