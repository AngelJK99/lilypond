%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.29"

\header {
  lsrtags = "text"

  texidoc = "
By default the @code{\\mark} command centers objects over a bar line.
This behavior can be modified to align at right or left.

"
  doctitle = "Aligning objects created with the \\mark command"
} % begin verbatim

\relative c' {
  c1 \mark "(Center)"
  c1
  \once \override Score.RehearsalMark #'self-alignment-X = #LEFT
  \mark "(Left)"
  c4 c c c
  c4 c c c
  \once \override Score.RehearsalMark #'self-alignment-X = #RIGHT
  \mark "(Right)"
  c1
}


