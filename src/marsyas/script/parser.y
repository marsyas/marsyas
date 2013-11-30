%filenames parser
%scanner scanner.h
%namespace Marsyas
%baseclass-preinclude "syntax_tree.hpp"

%stype node
%token INT REAL STRING BOOL ID PATH ARROW WHEN ELSE
%left '+' '-'
%left '*' '/'
%left '&' '|'

%%

input:
  //empty
| actor_instance { d_result = $1; }
;

actor_instance:
actor_instance_decl actor_def
{
  $$ = node();
  $$.tag = ACTOR_NODE;
  $$.components = { $1.components[0], $1.components[1], $2 };
}
;

actor_prototype:
actor_prototype_decl actor_def
{
  $$ = node();
  $$.tag = PROTOTYPE_NODE;
  $$.components = { $1.components[0], $1.components[1], $2 };
}
;

actor_instance_decl:
id
{ $$ = node(); $$.components = {node(), $1}; }
|
id ':' id
{ $$ = node(); $$.components = {$1, $3}; }
;


actor_prototype_decl:
id ':' id
{ $$ = node(); $$.components = {$1, $3}; }
;

actor_def:
  // empty

| '{' actor_def_list '}' { $$ = $2; }
;

actor_def_list:
  // empty

| actor_def_list actor_def_element
{
  $1.components.push_back($2);
  $$ = $1;
}
;

actor_def_element:

  ARROW actor_instance { $$ = $2; }

| '~' actor_prototype { $$ = $2; }

| control

| state
;

control:
  control_name '=' control_value {
    $$ = node();
    $$.tag = CONTROL_NODE;
    $$.components = { std::move($1), std::move($3) };
#ifdef MARSYAS_DEBUG_SCRIPT
    std::cout << "control:"
    << " '" << $$.components[0].s << "'"
    << " '" << $$.components[1].s << "'"
    << std::endl;
#endif
  }
;

control_name:
path
|
'+' id
{ $$ = std::string("+") + $2.s; $$.tag = ID_NODE; }
;

control_value:
bool | int | real | matrix | string | path

| '(' operation ')'
{
  $$ = $2;
}
;

state:
when_state else_state
{
  $$ = node();
  $$.tag = STATE_NODE;
  $$.components.push_back($1);
  $$.components.push_back($2);
}
;

when_state:
WHEN '(' operation ')' '{' state_def '}'
{
  $$ = node();
  $$.components.push_back($3);
  $$.components.insert($$.components.end(), $6.components.begin(), $6.components.end());
}
;

else_state:
// empty

| ELSE '{' state_def '}'
{
  $$ = node();
  $$.components = std::move($3.components);
}
;

state_def:
//empty
| state_def control
{
  $1.components.push_back($2);
  $$ = $1;
}
;

matrix:
'[' matrix_contents ']'
{
  $$ = $2;
  $$.tag = MATRIX_NODE;
}
;

matrix_contents:
  //empty

| matrix_row
{
  $$ = node();
  $$.components.push_back($1);
}

| matrix_contents ';' matrix_row
{
  $$ = $1;
  $$.components.push_back($3);
}
;

matrix_row:
  matrix_value
{
  $$ = node();
  $$.components.push_back($1);
}

| matrix_row ',' matrix_value
{
  $$ = $1;
  $$.components.push_back($3);
}
;

matrix_value: real | int
;

operation:
operation_value

| operation '+' operation { $$.set_operation($1, '+', $3); }
| operation '-' operation { $$.set_operation($1, '-', $3); }
| operation '*' operation { $$.set_operation($1, '*', $3); }
| operation '/' operation { $$.set_operation($1, '/', $3); }
| operation '&' operation { $$.set_operation($1, '&', $3); }
| operation '|' operation { $$.set_operation($1, '|', $3); }
| '(' operation ')' { $$ = $2; }
;

operation_value: bool | int | real | matrix | string | path;

bool:
  BOOL { $$ = d_scanner.bool_value(); }
;

int:
  INT { $$ = d_scanner.int_value(); }
;

real:
  REAL { $$ = d_scanner.real_value(); }
;

string:
  STRING { $$ = d_scanner.string_value(); }
;

id:
  ID { $$ = d_scanner.matched(); $$.tag = ID_NODE; }
;

path:
  id
| PATH { $$ = d_scanner.matched(); $$.tag = ID_NODE; }
;
