%option nounput noinput noyywrap
%{

  #include "syntax.tab.h"

  extern int line, column;

%}

ID [A-Za-z_][A-Za-z_0-9]*
INT [0-9]+
FLOAT [0-9]+\.[0-9]+
STRING ["].*["]
CHARACTER ['].[']|[']\\n[']|[']\\r[']|[']\\t[']

%%

"//".*\n      { line++; column=1; }
\n		        { line++; column=1; }
"int"			    { column += yyleng; return(INT);  }
"float"		    { column += yyleng; return(FLOAT);  }
"elem"		    { column += yyleng; return(ELEM);  }
"set"			    { column += yyleng; return(SET);  }
"EMPTY"		    { column += yyleng; return(EMPTY);  }
"main"		    { column += yyleng; return(MAIN);  }
"!"				    { column += yyleng; return('!');  }
"&&"			    { column += yyleng; return(AND_OP);  }
"||"			    { column += yyleng; return(OR_OP);  }
"="     	    { column += yyleng; return('=');  }
">"				    { column += yyleng; return('>');  }
"<"				    { column += yyleng; return('<');  }
"<="			    { column += yyleng; return(LTE_OP);  }
">="			    { column += yyleng; return(GTE_OP);  }
"!="			    { column += yyleng; return(NEQ_OP);  }
"=="			    { column += yyleng; return(EQ_OP);  }
"is_set"	    { column += yyleng; return(IS_SET);  }
"add"			    { column += yyleng; return(ADD_SET);  }
"remove"	    { column += yyleng; return(REMOVE_SET);  }
"exists"	    { column += yyleng; return(EXISTS_SET);  }
"forall"	    { column += yyleng; return(FORALL);  }
"in"			    { column += yyleng; return(IN);  }
"if"			    { column += yyleng; return(IF);  }	
"else"		    { column += yyleng; return(ELSE);  }	
"for"			    { column += yyleng; return(FOR);  }
"read"		    { column += yyleng; return(READ);  }	
"write"		    { column += yyleng; return(WRITE);  }	
"writeln"     { column += yyleng; return(WRITELN);  }		
"return"      { column += yyleng; return(RETURN);  }		
{ID}          { 
                column += yyleng; 
                yylval.val = malloc(strlen(yytext));
                strncpy(yylval.val, yytext, strlen(yytext));
                return ID;
              }
{INT}         { 
                column += yyleng; 
                yylval.val = malloc(strlen(yytext));
                strncpy(yylval.val, yytext, strlen(yytext));
                return INT_LITERAL;
               }
"+"           { column += yyleng; return('+'); }
"-"           { column += yyleng; return('-'); }
"*"           { column += yyleng; return('*'); }
"/"           { column += yyleng; return('/'); }
{FLOAT}       {
                column += yyleng; 
                yylval.val = malloc(strlen(yytext));
                strncpy(yylval.val, yytext, strlen(yytext));
                return FLOAT_LITERAL;
              }
{STRING}      {
                column += yyleng; 
                yylval.val = malloc(strlen(yytext));
                strncpy(yylval.val, yytext, strlen(yytext));
                return STRING_LITERAL;
               }
{CHARACTER}   {
                column += yyleng; 
                yylval.val = malloc(strlen(yytext));
                strncpy(yylval.val, yytext, strlen(yytext));
                return CHAR_LITERAL;
              }
'[.]{2,}'     { printf("%d:%d STRINGS SHOULD BE DOUBLE QUOTED: %s\n", line, column, yytext);column += yyleng; }
";"           { column += yyleng; return(';'); }
","           { column += yyleng; return(','); }
"("           { column += yyleng; return('('); }
")"           { column += yyleng; return(')'); }
"{"           { column += yyleng; return('{'); }
"}"           { column += yyleng; return('}'); }
[ \t\r]+      { column += yyleng; }
.	            { printf("%d:%d UNRECOGNIZED CHARACTER: %s\n", line, column, yytext);column += yyleng; }
