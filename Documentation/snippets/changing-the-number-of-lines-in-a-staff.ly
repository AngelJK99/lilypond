%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.29"

\header {
  lsrtags = "staff-notation"

  texidoc = "
The number of lines in a staff may changed by overriding the
@code{StaffSymbol} property @code{line-count}.




"
  doctitle = "Changing the number of lines in a staff"
} % begin verbatim

upper = \relative c'' {
  c4 d e f
}

lower = \relative c {
  \clef bass
  c4 b a g
}

\score {
  \context PianoStaff <<
    \new Staff {
      \upper
    }
    \new Staff {
      \override Staff.StaffSymbol #'line-count = #4
      \lower
    }
  >>
}

