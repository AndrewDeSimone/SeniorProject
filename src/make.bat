flex lexer.l
bison -d parser.y
gcc -o parser lex.yy.c parser.tab.c