%{
#include "saToken.h"
#include "TestParser.tab.h"
int line_num = 1;
void setStringSource(char *line);
%}

%%
[\t]         ;
[\n]         { ++line_num; }
[ \t\n]      ;
\(           { yylval.sval = strdup(yytext); return (TOKEN_LEFT_PAREN); }
\)           { yylval.sval = strdup(yytext); return (TOKEN_RIGHT_PAREN); }
[aA][nN][dD] { yylval.sval = strdup(yytext); return (TOKEN_AND); }
[bB][uU][tT] { yylval.sval = strdup(yytext); return (TOKEN_AND); }
[yY][eE][tT] { yylval.sval = strdup(yytext); return (TOKEN_AND); }
[oO][rR]     { yylval.sval = strdup(yytext); return (TOKEN_OR); 
             }
[nN][oO][tT] { yylval.sval = strdup(yytext); return (TOKEN_NOT); }
[iI][nN]     { yylval.sval = strdup(yytext); return (TOKEN_IN); }
[iI][sS]     { yylval.sval = strdup(yytext); return (TOKEN_IS); }
[a-zA-Z]+    { yylval.sval = strdup(yytext); return (TOKEN_FIELD); }

%%
yywrap() {
}
void setStringSource(char *line) {
  yy_scan_string(line);
}
