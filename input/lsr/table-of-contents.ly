%% Do not edit this file; it is auto-generated from input/new
%% This file is in the public domain.
\version "2.11.66"
\header {
  lsrtags = "paper-and-layout"
  texidoc = "A table of contents is included using
@w{@code{\\markuplines \\table-of-contents}}.  The TOC items are
added with the @code{\\tocItem} command."
  doctitle = "Table of contents"
} % begin verbatim


#(set-default-paper-size "a6")

\book {
  \markuplines \table-of-contents
  \pageBreak
  \tocItem \markup { The first score }
  \score {
    {
      c'1 \pageBreak
      \mark "A" \tocItem \markup { Mark A }
      d'1
    }
  }
  \pageBreak
  \tocItem \markup { The second score }
  \score {
    { e'1 }
    \header { piece = "Second score" }
  }
}
