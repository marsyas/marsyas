%filenames = scanner
%namespace Marsyas

%%

\{ { return '{'; }
\} { return '}'; }
= { return '='; }
; { return ';'; }
: { return ':'; }

"->" { return CHILD; }

true|false { return BOOL; }

[0-9]+\.[0-9]* { return REAL; }

[0-9]+ { return INT; }

[a-zA-Z_]+ { return ID; }

\"([^\"]|[[:space:]])*\" { return STRING; }
