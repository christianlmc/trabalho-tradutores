%define parse.error verbose
%define lr.type canonical-lr

%precedence THEN

%{
  #include <stdio.h>
  #include <stdlib.h>
  #include "ast.h"
  #include "symbol_table.h"
  #include "semantic.h"
  #include "tokens.h"
  #include "gen_tac.h"
  #include "types.h"

  extern tinyint hasError;
  int yylex(void);
  void yyerror(const char *s);
  int yylex_destroy();
  int line = 1, column = 1;

  tinyint hasMain = 0;
  Node* root;
  SymbolTable* table;
  Symbol* activeSymbol;
  int availableTacVar = 0;
  char* tacTable;
  char* tacCode;
%}
%union
{
	char *val;

  struct Token* token;
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
%type <node> statements_or_empty
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

%token <token> MAIN
%token <token> INT FLOAT ELEM SET EMPTY
%token <token> IS_SET ADD_SET REMOVE_SET EXISTS_SET FORALL
%token <token> IN IF FOR RETURN
%token <token> READ WRITE WRITELN
%token <token> GTE_OP NEQ_OP EQ_OP LTE_OP AND_OP OR_OP '>' '<' '!' '='
%token <token> INT_LITERAL FLOAT_LITERAL STRING_LITERAL CHAR_LITERAL ID 
%token <token> '-' '+' '*' '/'
%precedence <token> ELSE

%start init
%%

init:
  program { 
    $$ = createNodeFromString("root");
    $$->child = $1;
    root = $$;
  }
  ;

program:
  function_definition
  | function_definition[func] program { $func->next = $2; }
  | var_declaration[var_dec] program { $var_dec->next = $2; }
  | error  { $$ = createNodeFromString("program (error)"); }
  ;

function_definition:
  type ID[id] '(' declaration_arguments[args] ')' {
    Node *nodeAux = createNode($id);
    Symbol *aux = createSymbol($1->value, nodeAux, 0, $args);
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;

    char *command = formatStr("%s:", $id->value);
    tacCode = addCommand(tacCode, command);
    free(command);

    freeTree(nodeAux);
  } statements_block[stmts] {
    $$ = createNodeFromString("function definition"); 
    $$->child = $1;
    $1->next = createNodeFromString("args");
    $1->next->child = $args;
    Node* aux = $1->next;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $stmts;
    activeSymbol = activeSymbol->parent;
    availableTacVar = 0;

    tacCode = addCommand(tacCode, "return");

    freeToken($id);
  }
  | type MAIN[main] '(' declaration_arguments[args] ')' {
    Node *nodeAux = createNode($main);
    Symbol *aux = createSymbol($1->value, nodeAux, 0, $args);
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;
    hasMain = 1;

    tacCode = addCommand(tacCode, "main:");
    
    freeTree(nodeAux);
  } statements_block[stmts] {
    $$ = createNode($main); 
    $$->child = $1;
    $1->next = createNodeFromString("args");
    $1->next->child = $args;
    Node* aux = $1->next;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $stmts;
    activeSymbol = activeSymbol->parent;
    freeToken($main);
  }
  | type ID[id] '(' error ')' statements_block[stmts] { 
    $$ = createNode($id);
    freeToken($id);
  }
  ;

declaration_arguments:
  declaration_argument
  |  declaration_argument[arg] ',' declaration_arguments[args]
  {
    $arg->next = $args;
  }
  | %empty {
    $$ = createNodeFromString("arg (empty)"); 
  }
  ;

declaration_argument: 
  type ID[id] {
    $$ = createNodeFromString("arg"); 
    $$->child = $1;
    $1->next = createNode($id);
    freeToken($id);
  }
  ;

function_call:
  identifier[id] '(' arguments_or_empty[args] ')' {
    $$ = createNodeFromString("function call");
    $$->child = $id;
    $$->child->next = generateArgumentsCoercion(activeSymbol, $id, $args);
    $$->type = $id->type;
    $$->tacSymbol = availableTacVar;
    availableTacVar++;

    Symbol *aux = findSymbolByName($id->value, activeSymbol);
    
    if (aux) {
      char *call = formatStr("call %s, %d", aux->id, aux->argsCount);
      char *pop = formatStr("pop $%d", $$->tacSymbol);

      tacCode = addCommand(tacCode, call);
      tacCode = addCommand(tacCode, pop);

      free(call);
      free(pop);
    }
  } 
  | identifier '(' error ')' { $$ = createNodeFromString("function call (error)"); $$->child = $identifier; }
  ;

arguments_or_empty:
  arguments
  | %empty { $$ = NULL;  }
  ;

arguments:
  expression[exp] {
    char *expAddr = getAddress($exp);
    char *command = formatStr("param %s", expAddr);
    tacCode = addCommand(tacCode, command);
    free(expAddr);
    free(command);
  }
  | expression[exp] {
    char *expAddr = getAddress($exp);
    char *command = formatStr("param %s", expAddr);
    tacCode = addCommand(tacCode, command);
    free(expAddr);
    free(command);
  } ',' arguments[args] { 
    $exp->next = $args;
  }
  ;

statements_block:
  '{' '}'  { $$ = NULL; }
  | '{' statements '}'  { $$ = $2; }
  ;

statements:
  statement
  | statement statements  { $1->next = $2; }
  | { 
    Symbol *aux = createBlock();
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux; 
  } statements_block[block] { 
    activeSymbol = activeSymbol->parent; 
  } statements_or_empty[stmts] {
    $$ = $block;
    $block->next = $stmts;
  }
  ;

statements_or_empty:
  statements
  | %empty { $$ = createNodeFromString("empty block"); }
  ;

statement:
  var_declaration 
  | command
  | function_call_statement
  | assignment_statement
  | loops
  | if_statement
  | return_statement
  | error ';' { $$ = createNodeFromString("statement (error)"); }
  ;

function_call_statement:
  set_expression ';'
  | set_bool_expression ';'
  | elem_returning_expression ';'
  | function_call ';'
  ;

var_declaration: 
  type vars ';' { 
    $$ = createNodeFromString("declaration");
    $$->child = $1;
    $1->next = $2;
    Node *aux = $2;
    while (aux != NULL) {
      Symbol *var = createSymbol($1->value, aux, 0, NULL);
      var->tacSymbol = availableTacVar;
      availableTacVar++;
      pushChildSymbol(activeSymbol, var);
      aux = aux->next;
    }
  }
  | error  { $$ = createNodeFromString("declaration (error)"); }
  ;

vars:
  ID[id] { $$ = createNode($id); freeToken($id); }
  | ID[id] ',' vars[vars_t] { $$ = createNode($id); $$->next = $vars_t; freeToken($id); }
  ;

command:
  READ '(' identifier ')' ';' { 
    $$ = createNode($1); 
    $$->child = $identifier; 
    freeToken($1);
  }
  | write_command '(' expression[exp] ')' ';' { 
    $1->child = $exp;

    char *command = createInstruction($1, $exp, NULL);
    tacCode = addCommand(tacCode, command);

    free(command);
  }
  | write_command '(' STRING_LITERAL[literal] ')' ';' { 
    $1->child = createNode($literal);
    $1->child->tacSymbol = availableTacVar;
    availableTacVar++;

    char *command = createOutputInstruction($1, $1->child);
    tacCode = addCommand(tacCode, command);
    tacTable = createTableString(tacTable, $literal->value);

    free(command);
    freeToken($literal);
  }
  | write_command '(' CHAR_LITERAL[literal] ')' ';' { 
    $1->child = createNode($literal);
    $1->child->tacSymbol = availableTacVar;
    availableTacVar++;

    char *command = createOutputInstruction($1, $1->child);
    char *string = convertCharToString($literal->value);
    tacCode = addCommand(tacCode, command);
    tacTable = createTableString(tacTable, string);

    free(command);
    freeToken($literal);
  }
  | write_command '(' error ')' ';' {
    $1->child = createNodeFromString("write (error)"); 
  }
  | READ '(' error ')' ';' {
    $$ = createNodeFromString("read (error)");
    freeToken($1);
  }
  ;

write_command:
  WRITE { $$ = createNode($1); freeToken($1); }
  | WRITELN { $$ = createNode($1); freeToken($1); }
  ;

expression:
  or_expression 
  | set_expression
  | inner_set_in_expression
  ;

or_expression:
  or_expression OR_OP and_expression { 
    $$ = createNode($2);
    $$->child = generateLogicCoercion($1, $3);
    $$->type = INT_TYPE;
    $$->tacSymbol = availableTacVar;
    availableTacVar++;

    char *command = createInstruction($$, $1, $3);
    tacCode = addCommand(tacCode, command);
    free(command);

    freeToken($2);
  }
  | and_expression
  ;

and_expression:
  and_expression AND_OP comparison_expression { 
    $$ = createNode($2); 
    $$->child = generateLogicCoercion($1, $3);
    $$->type = INT_TYPE;
    $$->tacSymbol = availableTacVar;
    availableTacVar++;

    char *command = createInstruction($$, $1, $3);
    tacCode = addCommand(tacCode, command);
    free(command);

    freeToken($2);
  }
  | comparison_expression
  ;

comparison_expression:
  comparison_expression comparison_op addition_expression  {
    $$ = $2;
    $2->child = generateLogicCoercion($1, $3);
    $2->type = INT_TYPE;
    $$->tacSymbol = availableTacVar;
    availableTacVar++;

    char *command = createInstruction($2, $1, $3);
    tacCode = addCommand(tacCode, command);
    free(command);

  }
  | addition_expression
  ;

addition_expression:
  addition_expression addition_op multiplicative_expression { 
    $$ = $2;
    $2->child = generateAritmeticCoercion($1, $3);
    $2->type = getExpressionType($2->child, $2->child->next);
    $2->tacSymbol = availableTacVar;
    availableTacVar++;

    char *command = createInstruction($2, $1, $3);
    tacCode = addCommand(tacCode, command);
    free(command);
  }
  | multiplicative_expression
  ;

multiplicative_expression:
  multiplicative_expression multiply_op unary_expression { 
    $$ = $2;
    $2->child = generateAritmeticCoercion($1, $3);
    $2->type = getExpressionType($2->child, $2->child->next);
    $2->tacSymbol = availableTacVar;
    availableTacVar++;

    tacCode = addCommand(tacCode, createInstruction($2, $1, $3));  
  }
  | unary_expression
  ;

unary_expression:
  simple_expression
  | addition_op simple_expression { 
    $$ = createNodeFromString("sign");
    $$->child = $1;
    $$->type = $2->type;
    $1->next = $2;

    $$->tacSymbol = availableTacVar;
    availableTacVar++;

    tacCode = addCommand(tacCode, createInstruction($$, $1, $2));
  }
  | '!' simple_expression { 
    $$ = createNodeWithType($1, INT_TYPE);
    $$->child = $2;
    $$->tacSymbol = availableTacVar;
    availableTacVar++;

    tacCode = addCommand(tacCode, createInstruction($$, $2, NULL));

    freeToken($1);
  }
  ;

simple_expression:
  identifier
  | '(' expression ')' { $$ = $2; }
  | number
  | set_bool_expression
  | elem_returning_expression
  | function_call
  | EMPTY { $$ = createNodeWithType($1, SET_TYPE); freeToken($1); }
  ;


set_expression:
  set_returning_expression
  ;

set_bool_expression:
  IS_SET '(' identifier ')' { 
    $$ = createNodeWithType($1, INT_TYPE);
    $$->child = $identifier;
    freeToken($1);
  }
  ;

elem_returning_expression:
  EXISTS_SET '(' inner_set_in_expression[arg] ')' { 
    $$ = createNodeWithType($1, ELEM_TYPE);
    $$->child = $arg;
    freeToken($1);
  }
  ;

set_returning_expression:
  ADD_SET '('inner_set_in_expression[arg] ')' { 
    $$ = createNodeWithType($1, SET_TYPE);
    $$->child = $arg;
    freeToken($1);
  }
  | REMOVE_SET '(' inner_set_in_expression[arg] ')' { 
    $$ = createNodeWithType($1, SET_TYPE);
    $$->child = $arg;
    freeToken($1);
  }
  ;

inner_set_in_expression:
  expression IN set_in_right_arg { 
    $$ = createNodeWithType($2, INT_TYPE);
    $$->child = $1;
    $1->next = $3;
    if ($3->type != SET_TYPE) {
      printf(BOLDRED "Error on %d:%d: " RESET, $2->line, $2->column);
      printf("Right argument of IN needs to be of type " BOLDWHITE "'set'\n" RESET);
    }
    freeToken($2);
  }
  ;

set_in_right_arg:
  identifier
  | set_returning_expression
  ;

addition_op:
  '+'   { $$ = createNode($1); freeToken($1); }
  | '-' { $$ = createNode($1); freeToken($1); }
  ;

multiply_op:
  '*'   { $$ = createNode($1); freeToken($1); }
  | '/' { $$ = createNode($1); freeToken($1); }
  ;

assignment:
  identifier '=' expression {
    $$ = createNode($2);
    $$->child = $1;
    $$->child->next = convertToType($expression, $1->type);
    Symbol* aux = findSymbolByName($1->value, activeSymbol);
    if (aux) {
      char *command = createInstruction($$, $1, $3);
      tacCode = addCommand(tacCode, command);
      free(command);
    }

    freeToken($2);
  }
  | identifier '=' assignment[rhs] { 
    $$ = createNode($2); 
    $$->child = $1; 
    $$->child->next = $rhs;
    freeToken($2);
  }
  ;

identifier:
  ID[id] { 
    $$ = createNode($id);
    checkForPresence(activeSymbol, $id);
    Symbol *aux = findSymbolByName($id->value, activeSymbol);
    if (aux) {
      $$->tacSymbol = aux->tacSymbol;
      strncpy($$->tacType, aux->tacType, 2);
    }
    $$->type = getIdentifierType($identifier, activeSymbol);
    freeToken($id);
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
    $$ = createNode($1);
    $$->child = $arg1;
    Node *aux = $arg1;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $arg2;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $arg3;
    while (aux->next != NULL) aux = aux->next;
    $arg3->next = $block;
    activeSymbol = activeSymbol->parent;
    freeToken($1);
  }
  | FORALL '(' inner_set_in_expression[args] ')' {
    Symbol *aux = createBlock();
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;
  } statement_or_statements_block[block] {
    $$ = createNode($1);
    $$->child = $args;
    Node *aux = $args;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $block;
    activeSymbol = activeSymbol->parent;
    freeToken($1);
  }
  | FORALL '(' error ')' statement_or_statements_block[block] {
    $$ = createNodeFromString("forall (error)");
    $$->child = $block;
    freeToken($1);
  }
  | FOR '(' error ')' statement_or_statements_block[block] {
    $$ = createNodeFromString("for (error)");
    $$->child = $block;
    freeToken($1);
  }
  ;

if_statement:
  IF '(' expression ')' { 
    Symbol *aux = createBlock();
    pushChildSymbol(activeSymbol, aux);
    activeSymbol = aux;
   } if_block { 
    $$ = createNode($1); 
    $$->child = $expression;
    Node *aux = $expression;
    while (aux->next != NULL) aux = aux->next;
    $expression->next = $if_block;
    activeSymbol = activeSymbol->parent;
    freeToken($1);
  }
  | IF '(' error ')' if_block { 
    $$ = createNodeFromString("if (error)");
    $$->child = $if_block;
    freeToken($1);
  }
  ;

if_block:
  statement_or_statements_block %prec THEN
  | statement_or_statements_block ELSE[else] statement_or_statements_block { 
    $$ = createNodeFromString("case true");
    $$->child = $1;
    $$->next = createNodeFromString("case false");
    $$->next->child = $3;
    freeToken($else);
  }
  | statement_or_statements_block ELSE[else] '(' error ')' statement_or_statements_block[else_block] {
    $$ = createNodeFromString("true/false (error)"); 
    $$->child = $1;
    Node *aux = $1;
    while (aux->next != NULL) aux = aux->next;
    aux->next = $else_block;
    freeToken($else);
  }
  ;

statement_or_statements_block:
  statement
  | statements_block
  ;

return_statement:
  RETURN ';' { 
    $$ = createNode($1);
    tacCode = addCommand(tacCode, "return");
    freeToken($1);
  }
  | RETURN expression[exp] ';' {
    Symbol *aux = getCurrentFunction(activeSymbol);  
    if (aux) {
      $$ = createNodeWithType($1, aux->type);
      $$->child = convertToType($exp, aux->type);
      char *expAddr = getAddress($exp);
      char *command = formatStr("return %s", expAddr);
      tacCode = addCommand(tacCode, command);
      free(expAddr);
      free(command);
    } else {
      $$ = createNodeFromString("return (error)");
    }
    freeToken($1);
  }

comparison_op:
  LTE_OP    { $$ = createNode($1); freeToken($1); }
  | GTE_OP  { $$ = createNode($1); freeToken($1); }
  | NEQ_OP  { $$ = createNode($1); freeToken($1); }
  | EQ_OP   { $$ = createNode($1); freeToken($1); }
  | '>'     { $$ = createNode($1); freeToken($1); }
  | '<'     { $$ = createNode($1); freeToken($1); }
  ;

number:
  INT_LITERAL[literal]     { 
    $$ = createNodeWithType($literal, INT_TYPE);
    $$->tacSymbol = availableTacVar;
    availableTacVar++;

    char *command = formatStr("mov $%d, %s", $$->tacSymbol, $literal->value);
    tacCode = addCommand(tacCode, command);

    free(command);
    freeToken($literal);
  }
  | FLOAT_LITERAL[literal] { 
    $$ = createNodeWithType($literal, FLOAT_TYPE);
    $$->tacSymbol = availableTacVar;
    availableTacVar++;
    
    char *command = formatStr("mov $%d, %s", $$->tacSymbol, $literal->value);
    tacCode = addCommand(tacCode, command);

    free(command);
    freeToken($literal);
  }
  ;

type: 
  INT       { $$ = createNodeWithType($1, INT_TYPE); freeToken($1); }
  | FLOAT   { $$ = createNodeWithType($1, FLOAT_TYPE); freeToken($1); }
  | ELEM    { $$ = createNodeWithType($1, ELEM_TYPE); freeToken($1); }
  | SET     { $$ = createNodeWithType($1, SET_TYPE); freeToken($1); }
  ;

%%

int main()
{
  hasError = 0;
  table = (SymbolTable*) malloc(sizeof(SymbolTable));
  activeSymbol = createGlobalSymbol();

  table->first = activeSymbol;
  tacTable = strdup(".table");
  tacCode = strdup(".code");
  tacCode = injectSpecialFunctions(tacCode);

	yyparse();
  if (!hasMain) {
    printf(BOLDRED "Error: " RESET "Expected function 'main' to be defined\n");
    hasError = 1;
  }
	yylex_destroy();
  
  printTree(root, 0);
  printSymbolTable(table->first, 0);

  if (hasError) {
    printf(BOLDRED "Program has error, compilation aborted...\n" RESET);
  } else {
    printf(BOLDWHITE "Generating TAC file...\n" RESET);

    FILE *file = fopen("output.tac", "w");

    printf("%s\n", tacTable);
    printf("\n");
    printf("%s\n", tacCode);
    fwrite(tacTable, 1, strlen(tacTable), file);
    fwrite("\n\n", 1, strlen("\n\n"), file);
    fwrite(tacCode, 1, strlen(tacCode), file);
    fwrite("\n", 1, strlen("\n"), file);

    fclose(file);
  }

  free(tacTable);
  free(tacCode);
  freeTree(root);
  freeSymbolTable(table);

	return 0;
}

void yyerror(const char *s) {
  printf("\nLine %d:%d %s\n",  line, column, s);
  hasError = 1;
}