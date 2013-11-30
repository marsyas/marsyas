%filenames = scanner
%namespace Marsyas

%%

\{ { return '{'; }
\} { return '}'; }
\[ { return '['; }
\] { return ']'; }
\( { return '('; }
\) { return ')'; }
= { return '='; }
, { return ','; }
; { return ';'; }
: { return ':'; }
~ { return '~'; }
\+ { return '+'; }
\- { return '-'; }
\* { return '*'; }
\/ { return '/'; }

"==" { return Parser::EQ; }
"!=" { return Parser::NEQ; }
"<" { return Parser::LESS; }
">" { return Parser::MORE; }

"->" { return Parser::ARROW; }

"when" { return Parser::WHEN; }

"else" { return Parser::ELSE; }

true|false { return Parser::BOOL; }

(\+|\-)?[0-9]+\.[0-9]* { return Parser::REAL; }

(\+|\-)?[0-9]+ { return Parser::INT; }

\"([^\"]|[[:space:]])*\" { return Parser::STRING; }

[[:alpha:]_]+[[:alpha:][:digit:]_]* { return Parser::ID; }

\/?[[:alpha:][:digit:]_]+(\/[[:alpha:][:digit:]_]+)* { return Parser::PATH; }
