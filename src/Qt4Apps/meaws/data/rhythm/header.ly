\version "2.11.33"
\paper{
  indent=0\mm
  line-width=158\mm  % produces 624 pixels
  oddFooterMarkup=##f
  oddHeaderMarkup=##f
  bookTitleMarkup = ##f
  scoreTitleMarkup = ##f
}

colorNote = #(define-music-function (parser location color) (string?)
  #{
    \once \override Stem #'color = #(x11-color $color)
    \once \override NoteHead #'color = #(x11-color $color)
  #}) 
up = \markup{ \hspace #0.5 \arrow-head #Y #LEFT ##f }
down = \markup{ \hspace #0.5 \arrow-head #Y #RIGHT ##f }

\layout {
  \context { \Score
    proportionalNotationDuration = #(ly:make-moment 1 8)
  }
}

