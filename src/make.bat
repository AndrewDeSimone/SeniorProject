flex lexer.l
bison -d parser.y
gcc -o compiler lex.yy.c parser.tab.c