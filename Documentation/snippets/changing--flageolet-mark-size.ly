%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.4"

\header {
  lsrtags = "expressive-marks, unfretted-strings"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Para hacer más pequeño el círculo de @code{\\flageolet} (armónico)
utilice la siguiente función de Scheme.

"
  doctitlees = "Cambiar el tamaño de la marca de \\flageolet"


%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  texidocde = "
Um den @code{\\flageolet}-Kreis kleiner zu machen, kann diese
Scheme-Funktion eingesetzt werden.

"
  doctitlede = "\\flageolet-Zeichen verkleinern"

%% Translation of GIT committish: 0099693675b95c7ec6664d7277f22411a14e9c1b

  texidocfr = "
Il est possible de rapetisser le cercle d'un @code{\\flageolet} grâce
à une fonction Scheme.

"
  doctitlefr = "Modifier la taille d'un \\flageolet"


  texidoc = "
To make the @code{\\flageolet} circle smaller use the following Scheme
function. 

"
  doctitle = "Changing \\flageolet mark size"
} % begin verbatim

smallFlageolet =
#(let ((m (make-music 'ArticulationEvent
                      'articulation-type "flageolet")))
   (ly:music-set-property! m 'tweaks
     (acons 'font-size -3
       (ly:music-property m 'tweaks)))
  m)

\layout { ragged-right = ##f }

\relative c'' {
  d4^\flageolet_\markup { default size } d_\flageolet
  c4^\smallFlageolet_\markup { smaller } c_\smallFlageolet
}

