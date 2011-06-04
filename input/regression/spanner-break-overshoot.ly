\version "2.14.0"

\header {
  texidoc = "The @code{break-overshoot} property sets the amount that
a spanner (in this case: the beam and tuplet bracket) in case of a line
break extends beyond the rightmost column and extends to the left beyond the
prefatory matter."
}

\paper { ragged-right = ##t }

\relative c'' {
  \override Beam #'break-overshoot = #'(1.0 . 2.0)
  \override TupletBracket #'break-overshoot = #'(1.0 . 2.0)
  \override TupletBracket #'bracket-visibility = ##t
  \override Beam #'breakable = ##t
  c2.. \times 2/3 { c8.[ \break c8.] }
}
