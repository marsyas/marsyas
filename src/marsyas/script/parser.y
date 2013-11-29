%filenames parser
%scanner scanner.h
%namespace Marsyas
%baseclass-preinclude "syntax_tree.hpp"

%stype node
%token INT REAL STRING BOOL ID PATH ARROW

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

| '{' control_list actor_list '}'
  {
    $$ = node();
    $$.components = { std::move($2), std::move($3) };
  }
;

actor_list:
  //empty

| actor_list ARROW actor_instance
{
  $1.components.push_back($3);
  $$ = $1;
}

| actor_list '~' actor_prototype
{
  $1.components.push_back($3);
  $$ = $1;
}
;

control_list:
  //empty

| control_list control
  {
    $1.components.push_back( std::move($2) );
    $$ = std::move($1);
  }
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
  bool | int | real | string | path | matrix
;

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
