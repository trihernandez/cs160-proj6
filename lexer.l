%option yylineno
%pointer

%{
    #include <stdlib.h>
    #include <errno.h>
    #include <limits.h>
    #include "ast.hpp"
    #include "parser.hpp"
    void yyerror(const char *);
%}

%x C_COMMENT

num      0|[1-9][0-9]*
zero     "0"
id       [a-zA-Z][a-zA-Z0-9]*
arrow    "->"
print    "print"
return   "return"
if       "if"
else     "else"
while    "while"
new      "new"
integer  "integer"
boolean  "boolean"
none     "none"
equals   "equals"
and      "and"
or       "or"
not      "not"
true     "true"
false    "false"
extends  "extends"
do       "do"

%%

{num}     { yylval.base_int = atoi( yytext ); return T_NUMBER; }
{arrow}   { return T_ARROW; }
"+"       { return T_PLUS; }
"-"       { return T_MINUS; }
"*"       { return T_MULTIPLY; }
"/"       { return T_DIVIDE; }
">"       { return T_GREATER; }
">="      { return T_GREATEREQ; }
"="       { return T_EQUALSIGN; }
"("       { return T_LPARENTH; }
")"       { return T_RPARENTH; }
"{"       { return T_LBRACKET; }
"}"       { return T_RBRACKET; }
"."       { return T_PERIOD; }
","       { return T_COMMA; }
";"       { return T_SEMICOLON; }
print     { return T_PRINT; }
return    { return T_RETURN; }
if        { return T_IF; }
else      { return T_ELSE; }
while     { return T_WHILE; }
new       { return T_NEW; }
integer   { return T_INTEGER; }
boolean   { return T_BOOLEAN; }
none      { return T_NONE; }
equals    { return T_EQUALS; }
and       { return T_AND; }
or        { return T_OR; }
not       { return T_NOT; }
true      { return T_TRUE; }
false     { return T_FALSE; }
extends   { return T_EXTENDS; }
do        { return T_DO; }
{id}      { yylval.base_char_ptr = strdup(yytext); return T_IDENTIFIER; }

"/*"                { BEGIN(C_COMMENT); }
<C_COMMENT><<EOF>>  { yyerror("Dangling comment"); }
<C_COMMENT>"*/"     { BEGIN(INITIAL); }
<C_COMMENT>.        { }

[ \t\n] ;
.          { yyerror("invalid character"); }

%%

int yywrap(void) {
  return 1;
}
