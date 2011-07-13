\version "2.14.2"

#(ly:set-option 'warning-as-error #f)

\header {
  texidoc = "An additional opening slur during a running slur should be ignored
(and a warning printed), but never influence the slur's extents."
}

\paper { ragged-right = ##t }

\relative c' {
  \key fis \major
  c1(
  \break
  a2 b4 c)
}

\relative c' {
  \key fis \major
  c1(
  \break
  a2( b4 c)
%   ^ extra SlurEvent
}
%% END
