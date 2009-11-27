%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.4"

\header {
  lsrtags = "expressive-marks"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Las expresiones dinámicas que se comienzan, terminan o se producen
en la misma nota se alinean verticalmente.  Para asegurar que las
expresiones dinámicas se alinean cuando no se producen sobre la
misma nota, incremente la propiedad @code{staff-padding} del
objeto @code{DynamicLineSpanner}.

"
  doctitlees = "Alinear verticalmente expresiones dinámicas que abarcan varias notas"

  texidoc = "
Dynamics that occur at, begin on, or end on the same note will be
vertically aligned.  To ensure that dynamics are aligned when they do
not occur on the same note, increase the @code{staff-padding} property
of the @code{DynamicLineSpanner} object. 

"
  doctitle = "Vertically aligning dynamics across multiple notes"
} % begin verbatim

\relative c' {
  \override DynamicLineSpanner #'staff-padding = #4
  c2\p f\mf
  g2\< b4\> c\!
}

