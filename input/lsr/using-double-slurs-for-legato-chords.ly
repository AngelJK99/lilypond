%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.66"

\header {
  lsrtags = "expressive-marks"

  texidoces = "
Algunos compositores escriben dos ligaduras cuando quieren acordes
legato.  Esto se puede conseguir estableciendo @code{doubleSlurs}.

"
  doctitlees = "Utilizar ligaduras dobles para acordes legato"
  
%% Translation of GIT committish :<6ce7f350682dfa99af97929be1dec6b9f1cbc01a>
texidocde = "
Einige Komponisten schreiben doppelte Bögen, wenn Legato-Akkorde notiert
werden.  Das kann mit der Eigenschaft @code{doubleSlurs} erreicht werden.

"
  doctitlede = "Doppelte Bögen für Legato-Akkorde benutzen"

  texidoc = "
Some composers write two slurs when they want legato chords.  This can
be achieved by setting @code{doubleSlurs}. 

"
  doctitle = "Using double slurs for legato chords"
} % begin verbatim

\relative c' {
  \set doubleSlurs = ##t
  <c e>4( <d f> <c e> <d f>)
}
