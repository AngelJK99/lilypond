\version "2.14.2"

\header {

  texidoc = "If lyrics are assigned to a non-existing voice, a warning should
be printed.  However, if the lyrics context does not contain any lyrics, then
no warning should be printed."

}

#(ly:set-option 'warning-as-error #f)
<<
  \new Staff
    \new Voice = "notes" {
      c1
    }
  % This should not give a warning (empty lyrics, existing voice):
  \new Lyrics \lyricsto "notes" \lyricmode { }
  % This should give a warning (non-existing voice):
  \new Lyrics \lyricsto "not-existing-notes" \lyricmode { Test }
  % This should NOT give a warning (non-existing voice, but also no lyrics):
  \new Lyrics \lyricsto "not-existing-notes" \lyricmode { }
>>
