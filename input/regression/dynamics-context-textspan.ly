\version "2.14.0"

\header {
  texidoc = "Text spanners work in the @code{Dynamics} context."
}

<<
  \new Staff \relative c' {
    c1 | c
  }
  \new Dynamics {
    \override TextSpanner #'(bound-details left text) = #"rit."
    s1\startTextSpan
    s1\stopTextSpan
  }
  \new Staff \relative c' {
    c1 | c
  }
>>
