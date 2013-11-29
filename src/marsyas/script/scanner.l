%filenames = scanner
%namespace Marsyas

%%

\{ { return '{'; }
\} { return '}'; }
\[ { return '['; }
\] { return ']'; }
= { return '='; }
, { return ','; }
; { return ';'; }
: { return ':'; }
\+ { return '+'; }
~ { return '~'; }

"->" { return ARROW; }

true|false { return BOOL; }

(\+|\-)?[0-9]+\.[0-9]* { return REAL; }

(\+|\-)?[0-9]+ { return INT; }

\"([^\"]|[[:space:]])*\" { return STRING; }

[[:alpha:]_]+[[:alpha:][:digit:]_]* { return ID; }

\/?[[:alpha:][:digit:]_]+(\/[[:alpha:][:digit:]_]+)* { return PATH; }
