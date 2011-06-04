\version "2.14.0"

\header {
  lsrtags = "expressive-marks, tweaks-and-overrides"
  texidoc = "Custom text spanners can be defined and used with hairpin
  and text crescendos.  @code{\<} and @code{\>} produce hairpins by
  default, @code{\cresc} etc. produce text spanners by default."
  doctitle = "Dynamics text spanner postfix"
}

% Some sample text dynamic spanners, to be used as postfix operators
crpoco =
#(make-music 'CrescendoEvent
             'span-direction START
             'span-type 'text
             'span-text "cresc. poco a poco")

\relative c' {
  c4\cresc d4 e4 f4 |
  g4 a4\! b4\crpoco c4 |
  c4 d4 e4 f4 |
  g4 a4\! b4\< c4 |
  g4\dim a4 b4\decresc c4\!
}

