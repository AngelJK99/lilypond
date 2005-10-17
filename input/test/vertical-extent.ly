\version "2.7.13"
% TODO: huh?  what's this file about?  -gp

\header { texidoc = "
Vertical extents may increased by setting @code{minimumVerticalExtent}, 
@code{extraVerticalExtent}, and @code{verticalExtent}. In this example,
@code{verticalExtent} is increased.
" }

\score {
    <<
    \new Staff {
      \set Staff.verticalExtent = #'(-15.0 . 0.0)
      \clef alto
      c1
    }
    \new Staff {
      \set Staff.verticalExtent = #'(-0.0 . 15.0)
      \clef alto
      g1
    }
  >>
  \layout{
      raggedright = ##t
  }
}

