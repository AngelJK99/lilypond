\header {

  texidoc = "The markup commands @code{\wordwrap} and @code{\justify}
  produce simple paragraph text."

}

\markup {
  this is normal  text
  \override #'(linewidth . 60)
  \wordwrap {
    This is a test of the wordwrapping function.
    1 This is a test of the wordwrapping function.
    2 This is a test of the wordwrapping function.
    3 This is a test of the wordwrapping function.
    4 1a111 11111  \bold 22222 \italic  2222 
  }
  continuing
}

\markup {
  this is normal  text
  \override #'(linewidth . 40)
  \justify {
    This is a test of the wordwrapping function, but with justification.
    1 This is a test of the wordwrapping function, but with justification.
    2 This is a test of \fraction a b the wordwrapping function, but with justification.
    3 This is a test of the wordwrapping function, but with justification. bla bla
  }
  continuing
}


\markup {

  \override #'(linewidth . 40)

{  \wordwrap-string #" Om mani padme hum Om mani padme hum Om mani
padme hum Om mani padme hum Om mani padme hum Om mani padme hum Om
mani padme hum Om mani padme hum.

Gate Gate paragate Gate Gate paragate Gate Gate paragate Gate Gate
paragate Gate Gate paragate Gate Gate paragate."

  \justify-string #" Om mani padme hum Om mani padme hum Om mani
padme hum Om mani padme hum Om mani padme hum Om mani padme hum Om
mani padme hum Om mani padme hum.

Gate Gate paragate Gate Gate paragate Gate Gate paragate Gate Gate
paragate Gate Gate paragate Gate Gate paragate." }


  
}
