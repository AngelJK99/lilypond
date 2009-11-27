%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.4"

\header {
  lsrtags = "repeats, text"

  texidoc = "
Though volte are best specified using @code{\\repeat volta}, the
context property @code{repeatCommands} must be used in cases where the
volta text needs more advanced formatting with @code{\\markup}.

Since @code{repeatCommands} takes a list, the simplest method of
including markup is to use an identifier for the text and embed it in
the command list using the Scheme syntax @code{#(list (list 'volta
textIdentifier))}. Start- and end-repeat commands can be added as
separate list elements: 

"
  doctitle = "Volta text markup using repeatCommands"
} % begin verbatim

voltaAdLib = \markup { 1. 2. 3... \text \italic { ad lib. } }

\relative c'' {
  c1
  \set Score.repeatCommands = #(list (list 'volta voltaAdLib) 'start-repeat)
  c4 b d e
  \set Score.repeatCommands = #'((volta #f) (volta "4.") end-repeat)
  f1
  \set Score.repeatCommands = #'((volta #f))
}
