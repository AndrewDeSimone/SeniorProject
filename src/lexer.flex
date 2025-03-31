%{
      #include <stdio.h>
      #include <stdlib.h>
      
%}

%%

"+" {printf("PLUS");}
"-" {printf("MINUS");}
"*" {printf("STAR");}
"/" {printf("SLASH");}
"{" {printf("LEFTBRACKET");}
"}" {printf("RIGHTBRACKET");}
"(" {printf("LEFTPAREN");}
")" {printf("RIGHTPAREN");}
";" {printf("SEMICOLON");}
"&&" {printf("AND");}
"||" {printf("OR");}
"==" {printf("EQUAL_EQUAL");}
"!=" {printf("BANG_EQUAL");}
">=" {printf("GREATER_EQUAL");}
"<=" {printf("LESSER_EQUAL");}
">" {printf("GREATER");}
"<" {printf("LESSER");}
"var" {printf("VAR");}
"into" {printf("INTO");}
"if" {printf("IF");}
"while" {printf("WHILE");}
"print" {printf("PRINT");}
[0-9]+ {printf("number");}
[a-zA-Z_][a-zA-Z0-9_]* {printf("identifier");}
. {}

%% 

int main(int argc, char **argv) {
      if (argc > 1) {
          FILE *file = fopen(argv[1], "r");
          if (file) {
              yyin = file;
              yylex();
              fclose(file);
          } else {
              printf("Error opening file: %s\n", argv[1]);
          }
      } else {
          printf("No file provided!\n");
      }
  
      return 0;
  }

  int yywrap(void) {
      return 1;  // End of input processing
  }