%define parse.error verbose
%define lr.type canonical-lr

%precedence THEN
%precedence ELSE

%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "ast.h"
  #include "symbol_table.h"

  int yylex(void);
  void yyerror(const char *s);
  int yylex_destroy();
  int line = 1, column = 1;

  Node* root;
  SymbolTable* table;
  Symbol* activeSymbol;

%}
%union
{
	char *val;

  struct Node* node;
};

%type <node> init
%type <node> program
%type <node> function_definition
%type <node> declaration_arguments
%type <node> declaration_argument
%type <node> function_call
%type <node> arguments_or_empty
%type <node> arguments
%type <node> statements_block
%type <node> statements
%type <node> statement
%type <node> function_call_statement
%type <node> var_declaration
%type <node> vars
%type <node> command
%type <node> write_command
%type <node> expression
%type <node> or_expression
%type <node> and_expression
%type <node> comparison_expression
%type <node> addition_expression
%type <node> multiplicative_expression
%type <node> unary_expression
%type <node> simple_expression
%type <node> set_expression
%type <node> set_bool_expression
%type <node> elem_returning_expression
%type <node> set_returning_expression
%type <node> inner_set_in_expression
%type <node> set_in_right_arg
%type <node> addition_op
%type <node> multiply_op
%type <node> assignment
%type <node> assignment_statement
%type <node> loops
%type <node> if_statement
%type <node> if_block
%type <node> statement_or_statements_block
%type <node> return_statement
%type <node> comparison_op
%type <node> number
%type <node> type
%type <node> identifier

%token INT FLOAT ELEM SET EMPTY MAIN AND_OP OR_OP LTE_OP
%token GTE_OP NEQ_OP EQ_OP IS_SET ADD_SET REMOVE_SET EXISTS_SET FORALL
%token IN IF FOR READ WRITE WRITELN RETURN
%token <val> INT_LITERAL
%token <val> FLOAT_LITERAL
%token <val> STRING_LITERAL
%token <val> CHAR_LITERAL
%token <val> ID

%start init
%%

init:
  program { 
    $$ = createNode("root");
    $$->child = $1;
    root = $$;
  }
  ;

program:
  function_definition
  | function_definition[func] program { $func->next = $2; }
  | var_declaration[var_dec] program { $var_dec->next = $2; }
  | error  { $$ = createNode("program (error)"); }
  ;

function_definition:
  type ID[id] '(' declaration_arguments[args] ')' {
    Symbol *aux = createSymbol($1->value, $id, line, column, 0, $args);
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;
  } statements_block[stmts] { 
    $$ = createNode("function definition"); 
    $$->child = $1;
    $1->next = createNode("args");
    $1->next->child = $args;
    Node* aux = $1->next;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $stmts;
    free($id);
    activeSymbol = activeSymbol->parent;
  }
  | type MAIN '(' declaration_arguments[args] ')' {
    Symbol *aux = createSymbol($1->value, "main", line, column, 0, $args);
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;
  } statements_block[stmts] { 
    $$ = createNode("main"); 
    $$->child = $1;
    $1->next = createNode("args");
    $1->next->child = $args;
    Node* aux = $1->next;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $stmts;
    activeSymbol = activeSymbol->parent;
  }
  | type ID[id] '(' error ')' statements_block[stmts] { $$ = createNode("function definition (error)"); }
  ;

declaration_arguments:
  declaration_argument
  |  declaration_argument[arg] ',' declaration_arguments[args]
  {
    $arg->next = $args;
  }
  | %empty {
    $$ = createNode("arg (empty)"); 
  }
  ;

declaration_argument: 
  type ID[id] { 
    $$ = createNode("arg"); 
    $$->child = $1;
    $1->next = createNode($id);
    free($id);
  }
  ;

function_call:
  identifier '(' arguments_or_empty[args] ')' {
    $$ = createNode("function call");
    $$->child = $identifier;
    $$->child->next = $args;
  } 
  | identifier '(' error ')' { $$ = createNode("function call (error)"); }
  ;

arguments_or_empty:
  arguments
  | %empty { $$ = createNode("empty arg");  }
  ;

arguments:
  expression
  | expression ',' arguments { 
    $expression->next = $3; ;  
  }
  ;

statements_block:
  '{' '}'  { $$ = createNode("empty block"); }
  | '{' statements '}'  { $$ = $2; }
  ;

statements:
  statement
  | statement statements  { $1->next = $2; }
  ;

statement:
  var_declaration 
  | command
  | function_call_statement
  | assignment_statement
  | loops
  | if_statement
  | return_statement
  | error ';' { $$ = createNode("statement (error)"); }
  ;

function_call_statement:
  set_expression ';'
  | set_bool_expression ';'
  | elem_returning_expression ';'
  | function_call ';'
  ;

var_declaration: 
  type vars ';' { 
    $$ = createNode("declaration");
    $$->child = $1;
    $1->next = $2;
    Node *aux = $2;
    while (aux != NULL) {
      pushChildSymbol(activeSymbol, createSymbol($1->value, aux->value, line, column, 0, NULL));
      aux = aux->next;
    }
  }
  | error  { $$ = createNode("declaration (error)"); }
  ;

vars:
  ID[id] { $$ = createNode($id); free($id); }
  | ID[id] ',' vars[vars_t] { $$ = createNode($id); $$->next = $vars_t; free($id); }
  ;

command:
  READ '(' identifier ')' ';' { 
    $$ = createNode("read()"); 
    $$->child = $identifier; 
  }
  | write_command '(' expression ')' ';' { 
    $1->child = $expression; 
  }
  | write_command '(' STRING_LITERAL[literal] ')' ';' { 
    $1->child = createNode($literal);
    free($literal);
  }
  | write_command '(' CHAR_LITERAL[literal] ')' ';' { 
    $1->child = createNode($literal);
    free($literal);
  }
  | write_command '(' error ')' ';' {
    $$ = createNode("write (error)"); 
  }
  | READ '(' error ')' ';' {
    $$ = createNode("read (error)"); 
  }
  ;

write_command:
  WRITE { $$ = createNode("write()"); }
  | WRITELN { $$ = createNode("writeln()"); }
  ;

expression:
  or_expression 
  | set_expression
  | inner_set_in_expression
  ;

or_expression:
  or_expression OR_OP and_expression { 
    $$ = createNode("OR"); 
    $$->child = $1;
    $1->next = $3;
  }
  | and_expression
  ;

and_expression:
  and_expression AND_OP comparison_expression { 
    $$ = createNode("AND"); 
    $$->child = $1;
    $1->next = $3;
  }
  | comparison_expression
  ;

comparison_expression:
  comparison_expression comparison_op addition_expression  {
    $$ = $2;
    $2->child = $1;
    $1->next = $3;
  }
  | addition_expression
  ;

addition_expression:
  addition_expression addition_op multiplicative_expression { 
    $$ = $2;
    $2->child = $1;
    $1->next = $3;
  }
  | multiplicative_expression
  ;

multiplicative_expression:
  multiplicative_expression multiply_op unary_expression { 
    $$ = $2;
    $2->child = $1;
    $1->next = $3;
  }
  | unary_expression
  ;

unary_expression:
  simple_expression
  | addition_op simple_expression { 
    $$ = createNode("sign"); 
    $$->child = $1;
    $1->next = $2;
  }
  | '!' simple_expression { 
    $$ = createNode("!"); 
    $$->child = $2;
  }
  ;

simple_expression:
  identifier
  | '(' expression ')' { $$ = $2; }
  | number
  | set_bool_expression
  | elem_returning_expression
  | function_call
  | EMPTY { $$ = createNode("EMPTY"); }
  ;


set_expression:
  set_returning_expression
  ;

set_bool_expression:
  IS_SET '(' identifier ')' { 
    $$ = createNode("IS_SET");
    $$->child = $identifier;
  }
  ;

elem_returning_expression:
  EXISTS_SET '(' inner_set_in_expression[arg] ')' { 
    $$ = createNode("EXISTS_SET");
    $$->child = $arg;
  }
  ;

set_returning_expression:
  ADD_SET '('inner_set_in_expression[arg] ')' { 
    $$ = createNode("ADD_SET");
    $$->child = $arg;
  }
  | REMOVE_SET '(' inner_set_in_expression[arg] ')' { 
    $$ = createNode("REMOVE_SET");
    $$->child = $arg;
  }
  ;

inner_set_in_expression:
  expression IN set_in_right_arg { 
    $$ = createNode("IN");
    $$->child = $1;
    $1->next = $3;
  }
  ;

set_in_right_arg:
  identifier
  | set_returning_expression
  ;

addition_op:
  '+'   { $$ = createNode("+"); }
  | '-' { $$ = createNode("-"); }
  ;

multiply_op:
  '*'   { $$ = createNode("*"); }
  | '/' { $$ = createNode("/"); }
  ;

assignment:
  identifier '=' expression { 
    $$ = createNode("="); 
    $$->child = $1; 
    $$->child->next = $expression;
    
  }
  | identifier '=' assignment[rhs] { 
    $$ = createNode("="); 
    $$->child = $1; 
    $$->child->next = $rhs;
  }
  ;

identifier:
  ID[id] { 
    $$ = createNode($id);
    checkForPresence(activeSymbol, $id, line, column); 
    free($id);
  }
  ;


assignment_statement:
  assignment ';'
  ;

loops:
  FOR '(' assignment[arg1] ';'  expression[arg2] ';' assignment[arg3] ')' {
    Symbol *aux = createBlock();
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;
  } statement_or_statements_block[block] {
    $$ = createNode("for");
    $$->child = $arg1;
    Node *aux = $arg1;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $arg2;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $arg3;
    while (aux->next != NULL) aux = aux->next;
    $arg3->next = $block;
    activeSymbol = activeSymbol->parent;
  }
  | FORALL '(' inner_set_in_expression[args] ')' {
    Symbol *aux = createBlock();
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;
  } statement_or_statements_block[block] {
    $$ = createNode("forall");
    $$->child = $args;
    Node *aux = $args;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $block;
    activeSymbol = activeSymbol->parent;
  }
  | FORALL '(' error ')' statement_or_statements_block[block] {
    $$ = createNode("forall (error)");
    $$->child = $block;
  }
  | FOR '(' error ')' statement_or_statements_block[block] {
    $$ = createNode("for (error)");
    $$->child = $block;
  }
  ;

if_statement:
  IF '(' expression ')' { 
    Symbol *aux = createBlock();
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;
   } if_block { 
    $$ = createNode("if"); 
    $$->child = $expression;
    Node *aux = $expression;
    while (aux->next != NULL) aux = aux->next;
    $expression->next = $if_block;
    activeSymbol = activeSymbol->parent;
  }
  | IF '(' error ')' if_block { 
    $$ = createNode("if (error)");
    $$->child = $if_block;
  }
  ;

if_block:
  statement_or_statements_block %prec THEN
  | statement_or_statements_block ELSE statement_or_statements_block { 
    $$ = createNode("case true");
    $$->child = $1;
    $$->next = createNode("case false");
    $$->next->child = $3;
  }
  | statement_or_statements_block ELSE '(' error ')' statement_or_statements_block[else_block] {
    $$ = createNode("true/false (error)"); 
    $$->child = $1;
    Node *aux = $1;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $else_block;
  }
  ;

statement_or_statements_block:
  statement
  | statements_block
  ;

return_statement:
  RETURN ';' { 
    $$ = createNode("return"); 
  }
  | RETURN expression[exp] ';' { 
    $$ = createNode("return");
    $$->child = $exp;
  }

comparison_op:
  LTE_OP    { $$ = createNode("<=");  }
  | GTE_OP  { $$ = createNode(">=");  }
  | NEQ_OP  { $$ = createNode("!=");  }
  | EQ_OP   { $$ = createNode("==");  }
  | '>'     { $$ = createNode(">");  }
  | '<'     { $$ = createNode("<");  }
  ;

number:
  INT_LITERAL[literal]     { $$ = createNode($literal); free($literal); }
  | FLOAT_LITERAL[literal] { $$ = createNode($literal); free($literal); }
  ;

type: 
  INT       { $$ = createNode("int"); }
  | FLOAT   { $$ = createNode("float"); }
  | ELEM    { $$ = createNode("elem"); }
  | SET     { $$ = createNode("set"); }
  ;

%%

int main()
{
  table = (SymbolTable*) malloc(sizeof(SymbolTable));
  activeSymbol = createGlobalSymbol();

  table->first = activeSymbol;

	yyparse();
	yylex_destroy();
  
  printTree(root, 0);
  printSymbolTable(table->first, 0);

  freeTree(root);
  freeSymbolTable(table);

	return 0;
}

void yyerror(const char *s) {
  printf("\nLine %d:%d %s\n",  line, column, s);
}