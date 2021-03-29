%define parse.error verbose
%define lr.type canonical-lr

%precedence THEN
%precedence ELSE

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
%token IN IF FOR READ WRITE WRITELN RETURN
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
  type ID[func_name] '(' { printf("%s (", $func_name); } declaration_arguments ')' { printf(")"); } statements_block
  | type MAIN '(' ')' { printf("main ()\n"); } statements_block 
  ;

declaration_arguments:
  type ID[id] { printf("%s", $id); }
  | declaration_arguments ',' { printf(", "); } type ID[id]  { printf("%s", $id); }
  | %empty
  ;

function_call:
  ID[func_name] '(' { printf("%s (", $func_name); } arguments_or_empty ')' { printf(")"); }
  ;

arguments_or_empty:
  arguments
  | %empty
  ;

arguments:
  expression
  | expression ',' { printf(", "); } arguments
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
  | function_call_statement
  | assignment_statement { printf("\n"); }
  | loops
  | if_statement
  | return_statement
  ;

function_call_statement:
  set_expression ';' { printf(";\n"); }
  | set_bool_expression ';' { printf(";\n"); }
  | elem_returning_expression ';' { printf(";\n"); }
  | function_call ';' { printf(";\n"); }
  ;

var_declaration: 
  type vars ';' { printf(";\n"); }
  ;

vars:
  ID[id] { printf("%s", $id); }
  | ID[id] ',' { printf("%s, ", $id); } vars
  ;

command:
  READ '(' ID[id] ')' ';' { printf("read(%s);\n", $id); }
  | WRITE '(' { printf("write("); } expression ')' ';' { printf(");\n"); }
  | WRITELN '(' { printf("writeln("); } expression ')' ';' { printf(");\n"); }
  | WRITE '(' STRING_LITERAL[literal] ')' ';'   { printf("write(%s);\n", $literal); }
  | WRITELN '(' STRING_LITERAL[literal] ')' ';' { printf("writeln(%s);\n", $literal); }
  | WRITE '(' CHAR_LITERAL[literal] ')' ';'     { printf("write(%s);\n", $literal); }
  | WRITELN '(' CHAR_LITERAL[literal] ')' ';'   { printf("writeln(%s);\n", $literal); }
  ;

expression:
  or_expression
  | set_expression
  | inner_set_in_expression
  ;

or_expression:
  or_expression OR_OP { printf(" || "); } and_expression
  | and_expression
  ;

and_expression:
  and_expression AND_OP { printf(" && "); } comparison_expression
  | comparison_expression
  ;

comparison_expression:
  comparison_expression comparison_op addition_expression
  | addition_expression
  ;

addition_expression:
  addition_expression addition_op multiplicative_expression
  | multiplicative_expression
  ;

multiplicative_expression:
  multiplicative_expression multiply_op unary_expression
  | unary_expression
  ;

unary_expression:
  simple_expression
  | addition_op simple_expression
  | '!' { printf("!"); } simple_expression
  ;

simple_expression:
  ID[id] { printf("%s", $id); }
  | '(' { printf("("); } expression ')' { printf(")"); }
  | number
  | set_bool_expression
  | elem_returning_expression
  | function_call
  | EMPTY { printf("EMPTY"); }
  ;


set_expression:
  set_returning_expression
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
  ;

assignment_statement:
  assignment ';' { printf(";"); }
  ;

loops:
  FOR '(' { printf("for ("); } assignment ';' { printf("; "); }  expression ';' { printf("; "); } assignment ')' { printf(") "); } statement_or_statements_block
  | FORALL '(' { printf("forall ("); } inner_set_in_expression ')' { printf(") "); } statement_or_statements_block
  ;

if_statement:
  IF '(' { printf("if ("); } expression ')' { printf(") "); } if_block
  ;

if_block:
  statement_or_statements_block %prec THEN
  | statement_or_statements_block ELSE { printf("else "); } statement_or_statements_block
  ;

statement_or_statements_block:
  statement
  | statements_block
  ;

return_statement:
  RETURN ';' { printf("return;\n"); }
  | RETURN { printf("return "); } expression ';' { printf(";\n"); }

comparison_op:
  LTE_OP  { printf(" <= "); }
  | GTE_OP  { printf(" >= "); }
  | NEQ_OP  { printf(" != "); }
  | EQ_OP   { printf(" == "); }
  | '>'     { printf(" > "); }
  | '<'     { printf(" < "); }
  ;

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
    
