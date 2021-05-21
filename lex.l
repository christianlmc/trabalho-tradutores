%option nounput noinput noyywrap
%{

  #include "syntax.tab.h"
  #include "types.h"
  #include "tokens.h"

  extern int line, column;
  extern tinyint hasError;

%}

ID [A-Za-z_][A-Za-z_0-9]*
INT [0-9]+
FLOAT [0-9]+\.[0-9]+
STRING ["][^"]*["]
CHARACTER ['][^'][']|[']\\a[']|[']\\b[']|[']\\f[']|[']\\n[']|[']\\r[']|[']\\t[']|[']\\v[']|[']\\\\[']|[']\\'[']|[']\\'[']|[']\\\?[']|[']\\\"[']
WRONG_STRING ['][^']{2,}[']

%%

"//".*[^\n]       
\n		            { line++; column=1; }
"int"			        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(INT);  
                  }
"float"		        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(FLOAT);  
                  }
"elem"		        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(ELEM);  
                  }
"set"			        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(SET);  
                  }
"EMPTY"		        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(EMPTY);  
                  }
"main"		        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column); 
                    return(MAIN);  
                  }
"!"				        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return('!');  
                  }
"&&"			        { 
                    column += yyleng;
                    yylval.token = createToken("&&", line, column);
                    return(AND_OP);  
                  }
"||"			        { 
                    column += yyleng; 
                    yylval.token = createToken("||", line, column);
                    return(OR_OP);  
                  }
"="     	        { 
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return('=');  
                  }
">"				        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return('>');  
                  }
"<"				        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return('<');  
                  }
"<="			        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(LTE_OP);  
                  }
">="			        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(GTE_OP);  
                  }
"!="			        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(NEQ_OP);  
                  }
"=="			        { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(EQ_OP);  
                  }
"is_set"	        { column += yyleng; 
                    yylval.token = createToken("IS_SET", line, column);
                    return(IS_SET);
                  }
"add"			        { column += yyleng; 
                    yylval.token = createToken("ADD_SET", line, column);
                    return(ADD_SET);
                  }
"remove"	        { column += yyleng; 
                    yylval.token = createToken("REMOVE_SET", line, column);
                    return(REMOVE_SET);
                  }
"exists"	        { column += yyleng; 
                    yylval.token = createToken("EXISTS_SET", line, column);
                    return(EXISTS_SET);
                  }
"forall"	        { 
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return(FORALL);
                  }
"in"			        { 
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return(IN);
                  }
"if"			        { 
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return(IF);
                  }	
"else"		        { 
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return(ELSE);
                  }	
"for"			        { 
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return(FOR);
                  }
"read"		        { 
                    column += yyleng;
                    yylval.token = createToken("read()", line, column);
                    return(READ);  
                  }	
"write"		        { 
                    column += yyleng;
                    yylval.token = createToken("write()", line, column);
                    return(WRITE);  
                  }	
"writeln"         { 
                    column += yyleng;
                    yylval.token = createToken("writeln()", line, column);
                    return(WRITELN);  
                  }		
"return"          { 
                    column += yyleng;
                    yylval.token = createToken(yytext, line, column);
                    return(RETURN);  
                  }		
{ID}              { 
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return ID;
                  }
{INT}             { 
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return INT_LITERAL;
                   }
"+"               { column += yyleng; 
                    yylval.token = createToken("+", line, column);
                    return('+'); 
                  }
"-"               { column += yyleng; 
                    yylval.token = createToken("-", line, column);
                    return('-'); 
                  }
"*"               { column += yyleng; 
                    yylval.token = createToken("*", line, column);
                    return('*'); 
                  }
"/"               { column += yyleng; 
                    yylval.token = createToken("/", line, column);
                    return('/'); 
                  }
{FLOAT}           {
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return FLOAT_LITERAL;
                  }
{STRING}          {
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return STRING_LITERAL;
                   }
{CHARACTER}       {
                    column += yyleng; 
                    yylval.token = createToken(yytext, line, column);
                    return CHAR_LITERAL;
                  }
{WRONG_STRING}    { 
                    printf("%d:%d STRINGS SHOULD BE DOUBLE QUOTED: %s\n", line, column, yytext);
                    column += yyleng;
                    hasError = 1;
                  }
";"               { column += yyleng; return(';'); }
","               { column += yyleng; return(','); }
"("               { column += yyleng; return('('); }
")"               { column += yyleng; return(')'); }
"{"               { column += yyleng; return('{'); }
"}"               { column += yyleng; return('}'); }
[ \t\r]+          { column += yyleng; }
.	                { 
                    printf("%d:%d UNRECOGNIZED CHARACTER: %s\n", line, column, yytext);
                    column += yyleng;
                    hasError = 1;
                  }
