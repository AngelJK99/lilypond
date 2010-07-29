%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.29"

\header {
  lsrtags = "keyboards, tweaks-and-overrides"

%% Translation of GIT committish: 0b55335aeca1de539bf1125b717e0c21bb6fa31b
  texidoces = "
Se puede usar @code{Staff.pedalSustainStrings} para fijar el texto de
las indicaciones de pisar pedal y levantar pedal.  Observe que las
únicas cadenas válidas son las que están en la lista de glifos de
pedal: los valores que aparecen en este fragmento de código son una
relación exhaustiva.

"
  doctitlees = "Modificar el texto de las indicaciones de pedal"



  texidoc = "
@code{Staff.pedalSustainStrings} can be used to set the text used for
pedal down and up.  Note that the only valid strings are those found in
the list of pedal glyphs - the values used this snippet constitute an
exhaustive list.

"
  doctitle = "Changing the text for sustain markings"
} % begin verbatim

sustainNotes = { c4\sustainOn d e\sustainOff\sustainOn f\sustainOff }

\relative c' {
  \sustainNotes
  \set Staff.pedalSustainStrings = #'("P" "P-" "-")
  \sustainNotes
  \set Staff.pedalSustainStrings = #'("d" "de" "e")
  \sustainNotes
  \set Staff.pedalSustainStrings = #'("M" "M-" "-")
  \sustainNotes
  \set Staff.pedalSustainStrings = #'("Ped" "*Ped" "*")
  \sustainNotes
}
