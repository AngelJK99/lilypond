%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.66"

\header {
  lsrtags = "percussion"

  texidoc = "
Graphic symbols for percussion instruments are not natively supported;
however it is possible to include such symbols, either as an external
EPS file or as embedded PostScript code inside a markup, as
demonstrated in this example.

"
  doctitle = "Percussion beaters"
} % begin verbatim

stick = \markup { 
  \with-dimensions #'(0 . 5) #'(0 . 5) 
  \postscript #"
    0 6 translate
    0.8 -0.8 scale
    0 0 0 setrgbcolor
    [] 0 setdash
    1 setlinewidth
    0 setlinejoin
    0 setlinecap
    gsave [1 0 0 1 0 0] concat
    gsave [1 0 0 1 -3.5406095 -199.29342] concat
    gsave
    0 0 0 setrgbcolor
    newpath
    7.1434065 200.94354 moveto
    7.2109628 200.90454 7.2785188 200.86554 7.3460747 200.82654 curveto
    8.2056347 202.31535 9.0651946 203.80414 9.9247546 205.29295 curveto
    9.8571989 205.33195 9.7896429 205.37095 9.7220864 205.40996 curveto
    8.8625264 203.92115 8.0029664 202.43233 7.1434065 200.94354 curveto
    closepath
    eofill
    grestore
    gsave
    0 0 0 setrgbcolor
    newpath
    4.9646672 203.10444 moveto
    5.0036707 203.03688 5.0426744 202.96933 5.0816777 202.90176 curveto
    6.5704792 203.76133 8.0592809 204.6209 9.5480824 205.48045 curveto
    9.5090791 205.54801 9.4700754 205.61556 9.4310717 205.68311 curveto
    7.94227 204.82356 6.4534687 203.96399 4.9646672 203.10444 curveto
    closepath
    eofill
    grestore
    gsave
    <<
    /ShadingType 3
    /ColorSpace /DeviceRGB
    /Coords [113.13708 207.87465 0 113.13708 207.87465 16.162441]
    /Extend [true true]
    /Domain [0 1]
    /Function <<
    /FunctionType 3
    /Functions
    [
    <<
    /FunctionType 2
    /Domain [0 1]
    /C0 [1 1 1]
    /C1 [0.72941178 0.72941178 0.72941178]
    /N 1
    >>
    ]
    /Domain [0 1]
    /Bounds [ ]
    /Encode [ 0 1 ]
    >>
    >>
    newpath
    7.6422017 200.76488 moveto
    7.6505696 201.02554 7.3905363 201.24867 7.1341335 201.20075 curveto
    6.8759501 201.16916 6.6949602 200.87978 6.7801462 200.63381 curveto
    6.8480773 200.39155 7.1438307 200.25377 7.3728389 200.35861 curveto
    7.5332399 200.42458 7.6444521 200.59122 7.6422017 200.76488 curveto
    closepath
    clip
    gsave [0.052859054 0.063089841 -0.020912282 0.017521108 5.7334261 189.76443] concat
    shfill
    grestore
    grestore
    0 0 0 setrgbcolor
    [] 0 setdash
    0.027282091 setlinewidth
    0 setlinejoin
    0 setlinecap
    newpath
    7.6422017 200.76488 moveto
    7.6505696 201.02554 7.3905363 201.24867 7.1341335 201.20075 curveto
    6.8759501 201.16916 6.6949602 200.87978 6.7801462 200.63381 curveto
    6.8480773 200.39155 7.1438307 200.25377 7.3728389 200.35861 curveto
    7.5332399 200.42458 7.6444521 200.59122 7.6422017 200.76488 curveto
    closepath
    stroke
    gsave
    <<
    /ShadingType 3
    /ColorSpace /DeviceRGB
    /Coords [113.13708 207.87465 0 113.13708 207.87465 16.162441]
    /Extend [true true]
    /Domain [0 1]
    /Function <<
    /FunctionType 3
    /Functions
    [
    <<
    /FunctionType 2
    /Domain [0 1]
    /C0 [1 1 1]
    /C1 [0.72941178 0.72941178 0.72941178]
    /N 1
    >>
    ]
    /Domain [0 1]
    /Bounds [ ]
    /Encode [ 0 1 ]
    >>
    >>
    newpath
    5.2721217 202.83181 moveto
    5.2804896 203.09247 5.0204563 203.3156 4.7640539 203.26768 curveto
    4.5058701 203.23609 4.3248803 202.94671 4.4100662 202.70074 curveto
    4.4779975 202.45848 4.7737511 202.3207 5.0027593 202.42554 curveto
    5.1631598 202.49149 5.2743721 202.65813 5.2721217 202.83181 curveto
    closepath
    clip
    gsave [0.052859054 0.063089841 -0.020912282 0.017521108 3.363346 191.83136] concat
    shfill
    grestore
    grestore
    0 0 0 setrgbcolor
    [] 0 setdash
    0.027282091 setlinewidth
    0 setlinejoin
    0 setlinecap
    newpath
    5.2721217 202.83181 moveto
    5.2804896 203.09247 5.0204563 203.3156 4.7640539 203.26768 curveto
    4.5058701 203.23609 4.3248803 202.94671 4.4100662 202.70074 curveto
    4.4779975 202.45848 4.7737511 202.3207 5.0027593 202.42554 curveto
    5.1631598 202.49149 5.2743721 202.65813 5.2721217 202.83181 curveto
    closepath
    stroke
    grestore
    grestore
  "
}

\score {
  b1^\stick
}
