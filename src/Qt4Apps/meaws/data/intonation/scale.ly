colorNote = #(define-music-function (parser location color) (string?)
  #{
    \once \override Stem #'color = #(x11-color $color)
    \once \override NoteHead #'color = #(x11-color $color)
  #}) 
up = \markup{ \hspace #0.5 \arrow-head #Y #LEFT ##f }
down = \markup{ \hspace #0.5 \arrow-head #Y #RIGHT ##f }
\version "2.10.0"
\score{
{
c'4
d'8
e'
f'4
g'
a'4.
b'8
c''2
}
\layout{}
}
