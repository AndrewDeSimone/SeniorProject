%{
#include "parser.tab.h"
#include <stdio.h>
%}

number      [0-9]+
asterik     \*
plus        \+
semicolon   \;

%%

{number}    { yylval.num = atoi(yytext); return NUMBER; }
{asterik}   { return ASTERISK; }
{plus}      { return PLUS; }
{semicolon} { return SEMICOLON; }

[ \t\n\r]+  { /* skip whitespace */ }

%%

int yywrap() {
    return 1;
}
