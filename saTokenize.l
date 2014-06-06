%{
#include "saToken.h"
#include "saTokenize.tab.h"
int line_num = 1;
int col_num = 0;
void setStringSource(char *line);
%}

%%
" "          {col_num += 1;}
[\t]         ;
[\n]         { ++line_num; col_num = 0; }
\(           { yylval.sval = strdup(yytext); col_num += 1; return (TOKEN_LEFT_PAREN); }
\)           { yylval.sval = strdup(yytext); col_num += 1; return (TOKEN_RIGHT_PAREN); }
[aA][nN][dD] { yylval.sval = strdup(yytext); col_num += 3; return (TOKEN_AND); }
[bB][uU][tT] { yylval.sval = strdup(yytext); col_num += 3; return (TOKEN_AND); }
[yY][eE][tT] { yylval.sval = strdup(yytext); col_num += 3; return (TOKEN_AND); }
[oO][rR]     { yylval.sval = strdup(yytext); col_num += 2; return (TOKEN_OR);  }
[nN][oO][tT] { yylval.sval = strdup(yytext); col_num += 3; return (TOKEN_NOT); }
[iI][nN]     { yylval.sval = strdup(yytext); col_num += 2; return (TOKEN_IN); }
[iI][sS]     { yylval.sval = strdup(yytext); col_num += 2; return (TOKEN_IS); }
[a-zA-Z\_\-\.]+    { yylval.sval = strdup(yytext); col_num += strlen(yytext); return (TOKEN_FIELD); }

%%
int yywrap() {
}
void setStringSource(char *line) {
  yy_scan_string(line);
}
