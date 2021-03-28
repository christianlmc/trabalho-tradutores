%define parse.error verbose
%define lr.type canonical-lr

%{

  #include <stdio.h>
  #include <stdlib.h>
  int yylex(void);
  int yyerror(const char *s);

%}
%union
{
	char *val;
};
%token INT FLOAT ELEM SET EMPTY MAIN AND_OP OR_OP LTE_OP
%token GTE_OP NEQ_OP EQ_OP IS_SET ADD_SET REMOVE_SET EXISTS_SET FORALL
%token IN IF ELSE FOR READ WRITE WRITELN RETURN
%token <val> INT_LITERAL
%token <val> FLOAT_LITERAL
%token <val> STRING_LITERAL
%token <val> CHAR_LITERAL
%token <val> ID

%start program
%%

program:
  function_declaration
  | function_declaration program
  | var_declaration program
  | error
  ;


function_declaration:
  type ID[func_name] '(' { printf("%s (", $func_name); } arguments ')' { printf(")"); } statements_block
  | type MAIN '(' ')' { printf("main ()\n"); } statements_block 
  ;

arguments:
  type ID[id] { printf("%s", $id); }
  | arguments ',' { printf(", "); } type ID[id]  { printf("%s", $id); }
  | %empty
  ;


statements_block:
  '{' '}'  { printf(" {} \n"); }
  | '{' { printf("{\n"); } statements '}' { printf("}\n"); }
  ;

statements:
  { printf("  "); } statement 
  | statements { printf("  "); } statement
  ;


statement:
  var_declaration
  | command
  | assignment_statement { printf("\n"); }
  | loops
  | if_statement
  | return_statement
  ;


var_declaration: 
  type vars ';' { printf(";\n"); }
  ;

vars:
  ID[id] { printf("%s", $id); }
  | ID[id] ',' { printf("%s, ", $id); } vars
  ;

command:
  READ '(' ')' ';' { printf("read();\n"); }
  | WRITE '(' { printf("write("); } expression ')' ';' { printf(");\n"); }
  | WRITELN '(' { printf("writeln("); } expression ')' ';' { printf(");\n"); }
  | WRITE '(' STRING_LITERAL[literal] ')' ';'   { printf("write(%s);\n", $literal); }
  | WRITELN '(' STRING_LITERAL[literal] ')' ';' { printf("writeln(%s);\n", $literal); }
  | WRITE '(' CHAR_LITERAL[literal] ')' ';'     { printf("write(%s);\n", $literal); }
  | WRITELN '(' CHAR_LITERAL[literal] ')' ';'   { printf("writeln(%s);\n", $literal); }
  ;

expression:
  addition_expression
  | set_expression
  ;

set_expression:
  set_bool_expression
  | elem_returning_expression
  | set_returning_expression
  | EMPTY { printf("EMPTY"); }
  ;

set_bool_expression:
  IS_SET '(' ID[id] ')' { printf("is_set(%s)", $id); }
  ;

elem_returning_expression:
  EXISTS_SET '(' { printf("exists("); } inner_set_in_expression ')' { printf(")"); }
  ;

set_returning_expression:
  ADD_SET '(' { printf("add("); } inner_set_in_expression ')' { printf(")"); }
  | REMOVE_SET '(' { printf("remove("); } inner_set_in_expression ')' { printf(")"); }
  ;

inner_set_in_expression:
  expression IN { printf(" in "); } set_in_right_arg 
  ;

set_in_right_arg:
  ID[id] { printf("%s", $id); }
  | set_returning_expression
  ;

addition_expression:
  addition_expression addition_op term
  | term
  ;

term:
  term multiply_op factor
  | factor
  ;

factor:
  number
  | ID[id] { printf("%s", $id); }
  | '(' { printf("("); } expression ')' { printf(")"); }
  ;

addition_op:
  '+'   { printf(" + "); }
  | '-' { printf(" - "); }
  ;

multiply_op:
  '*'   { printf(" * "); }
  | '/' { printf(" / "); }
  ;

assignment:
  ID[id] '=' { printf("%s = ", $id); } expression
  | ID[id] '=' '-' { printf("%s = -", $id); } expression

assignment_statement:
  assignment ';' { printf(";"); }
  ;

loops:
  FOR '(' { printf("for ("); } assignment_statement boolean_statement assignment ')' { printf(") "); } statements_block
  | FORALL '(' { printf("forall ("); } inner_set_in_expression ')' { printf(") "); } statements_block
  ;

if_statement:
  IF '(' { printf("if ("); } boolean_expression ')' { printf(") "); } if_block
  ;


if_block:
  statement
  | statements_block ELSE { printf("else "); } statement
  | statements_block ELSE { printf("else "); } statements_block
  ;

return_statement:
  RETURN ';' { printf("return;\n"); }
  | RETURN { printf("return "); } expression ';' { printf(";\n"); }

boolean_statement:
  boolean_expression ';' { printf(";"); }
  ;

boolean_expression:
  /* TODO: Revisar se essa separação faz sentido */
  comparison_expression comparison_op boolean_expression 
  | comparison_expression logical_op boolean_expression
  | comparison_expression 
  ;

comparison_expression:
  expression
  | '!' { printf("!"); } expression
  ;

comparison_op:
  LTE_OP  { printf(" <= "); }
  | GTE_OP  { printf(" >= "); }
  | NEQ_OP  { printf(" != "); }
  | EQ_OP   { printf(" == "); }
  | '>'     { printf(" > "); }
  | '<'     { printf(" < "); }
  ;

logical_op:
  AND_OP    { printf(" && "); }
  | OR_OP   { printf(" || "); }

number:
  INT_LITERAL[literal]     { printf("%s", $literal); }
  | FLOAT_LITERAL[literal] { printf("%s", $literal); }
  ;

type: 
  INT       { printf("int "); }
  | FLOAT   { printf("float "); }
  | ELEM    { printf("elem "); }
  | SET     { printf("set "); }
  ;
    
