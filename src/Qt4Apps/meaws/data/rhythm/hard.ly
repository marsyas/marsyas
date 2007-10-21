\version "2.10.0"
\include "header.ly"
\score{
\new RhythmicStaff {
  \time 3/4
  c8. c16 \times 2/3{ c8 c c } c16 c8. |
  r16 c8 c16 ~ c c8 r c16 c8 |
  \times 2/3 {c4 c8} c8[ c] \times 2/3 {c8. c16 c8 } |
}
\midi{}
}

