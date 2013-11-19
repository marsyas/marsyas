%filenames parser
%scanner scanner.h
%namespace Marsyas
%baseclass-preinclude "syntax_tree.hpp"

%stype node
%token INT REAL STRING BOOL ID ARROW

%%

input:
  //empty
| actor { d_result = std::move($1); }
;

actor:
  actor_id actor_type actor_def
  {
    $$.tag = ACTOR_NODE;
    $$.components = {
      std::move($1),
      std::move($2),
      std::move($3)
    };
#if 0
    std::cout
    << "actor:"
    << " '" << $$.components[0].s << "'"
    << " '" << $$.components[1].s << "'"
    << std::endl;
#endif
  }
;

actor_id:
  id
  { $$ = std::move($1); }
;

actor_type:
  // empty

| ':' id
  { $$ = std::move($2); }
;

actor_def:
  // empty

| '{' control_list actor_list '}'
  {
    $$.components = { std::move($2), std::move($3) };
  }
;

actor_list:
  //empty

| actor_list ARROW actor
  {
    $1.components.push_back( std::move($3) );
    $$ = std::move($1);
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
  id '=' control_value {
    $$.tag = CONTROL_NODE;
    $$.components = { std::move($1), std::move($3) };
#if 0
    std::cout << "control:"
    << " '" << $$.components[0].s << "'"
    << " '" << $$.components[1].s << "'"
    << std::endl;
#endif
  }
;

control_value:
  bool | int | real | string
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
  ID { $$ = d_scanner.matched(); }
;
