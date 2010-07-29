%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.29"

\header {
  lsrtags = "rhythms, tweaks-and-overrides"

%% Translation of GIT committish: 190a067275167c6dc9dd0afef683d14d392b7033

  texidocfr = "
Selon la tradition, les crochets indicateurs de nolet sont toujours
imprimés sauf dans le cas où ils seraient de la même longuer qu'une
ligature.  LilyPond permet, au travers de la propriété
@code{'bracket-visibility}, de contôler précisément leur
affichage@tie{}: déterminée à @code{#t}, ils seront toujours
imprimés@tie{}; @code{#f} permet de ne jamais les imprimer, et
@code{#'if-no-beam} les imprimera en l'absence de ligature.

"
  doctitlefr = "Contrôle de l'impression des crochets de nolet"


  texidoc = "
The default behavior of tuplet-bracket visibility is to print a bracket
unless there is a beam of the same length as the tuplet. To control the
visibility of tuplet brackets, set the property
@code{'bracket-visibility} to either @code{#t} (always print a
bracket), @code{#f} (never print a bracket) or @code{#'if-no-beam}
(only print a bracket if there is no beam).

"
  doctitle = "Controlling tuplet bracket visibility"
} % begin verbatim

music = \relative c'' {
  \times 2/3 { c16[ d e } f8]
  \times 2/3 { c8 d e }
  \times 2/3 { c4 d e }
}

\new Voice {
  \relative c' {
    << \music s4^"default" >>
    \override TupletBracket #'bracket-visibility = #'if-no-beam
    << \music s4^"'if-no-beam" >>
    \override TupletBracket #'bracket-visibility = ##t
    << \music s4^"#t" >>
    \override TupletBracket #'bracket-visibility = ##f
    << \music s4^"#f" >>
  }
}


