%filenames = scanner
%implementation-header = "scanner_impl.h"
%namespace Marsyas

%x directive
%x one_line_comment
%x multi_line_comment

%%

# { begin(StartCondition__::directive); return '#'; }

<directive>{

\n {  begin(StartCondition__::INITIAL); return '\n'; }

"include" { return Parser::INCLUDE; }

"as" { return Parser::AS; }

[[:alpha:]_]+[[:alpha:][:digit:]_]* { return Parser::ID; }

[^[:space:]]+ { return Parser::DIRECTIVE_ARG; }

}


"//" { begin(StartCondition__::one_line_comment); }
<one_line_comment>{
\n { begin(StartCondition__::INITIAL); }
}

"/*" { begin(StartCondition__::multi_line_comment); }
<multi_line_comment>{
"*/" { begin(StartCondition__::INITIAL); }
}

"public" { return Parser::PUBLIC; }

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
"on" { return Parser::ON; }
"else" { return Parser::ELSE; }

true|false { return Parser::BOOL; }

(\+|\-)?[0-9]+\.[0-9]* { return Parser::REAL; }

(\+|\-)?[0-9]+ { return Parser::INT; }

\"([^\"]|[[:space:]])*\" { return Parser::STRING; }

[[:alpha:]_]+[[:alpha:][:digit:]_]* { return Parser::ID; }

\/?[[:alpha:][:digit:]_]+(\/[[:alpha:][:digit:]_]+)* { return Parser::PATH; }
