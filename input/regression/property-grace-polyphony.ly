
\header {

    texidoc = "Property overrides and reverts from @code{\\grace} do
    not interfere with the overrides and reverts from polyphony."

}
\layout { ragged-right = ##t }

\version "2.7.32"
   \relative c'' {
      <<
	  { \grace e8 d2 }
	  \\ { a2 } >>
  }

