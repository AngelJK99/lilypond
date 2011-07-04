\header {

  texidoc = "Rests avoid notes.  Each rest is moved in the direction
of the stems in its voice.  Rests may overlap other rests in voices
with the same stem direction, in which case a warning is given, but
is supressed if the rest has a pitch."

}

\version "2.15.0"

\new Staff <<
  \relative c''' { g8 g g r r2 } \\
  \relative c' { a4\rest c r2 } \\
  \relative c'' { c4 c r2 } \\
  \relative c'' { r2 g }
>>


