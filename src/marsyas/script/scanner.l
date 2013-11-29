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

[0-9]+\.[0-9]* { return REAL; }

[0-9]+ { return INT; }

[a-zA-Z_]+[a-zA-Z0-9_]* { return ID; }

@\/?[[:alpha:][:digit:]_]+(\/[[:alpha:][:digit:]_]+)* { return PATH; }

\"([^\"]|[[:space:]])*\" { return STRING; }
