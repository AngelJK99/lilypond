\version "2.14.0"

\header {
  texidoc="
Setting staff-space to 0 does not cause a segmentation fault.
"
}

\new Staff \with {
  \override StaffSymbol #'staff-space = #0
  } {
  \relative c' {
    c1 \bar ":|"
  }
}
