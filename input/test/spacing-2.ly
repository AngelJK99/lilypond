\version "1.7.18"
% more yummy regression goodness.  -gp
\header {
texidoc = "
1. Upon stretching every note should stretch according to duration.

2. 8th notes should be spaced equidistantly.
"
}

\score { 
    \notes \relative c''
	\context GrandStaff <
	\context Staff = SA <
		\context Voice = VA { \stemUp\slurUp\tieUp
			e4 dis4 e2 }
		\context Voice = VB { \stemDown\slurDown\tieDown
			 cis8-[ a]  fis-[ b] gis2 }
		{\key e \major }
		>
	\context Staff = SB { \clef "bass" \key e \major
		 a,,16-[ e dis e]  b'8-[ b,]  e16-[ e, gis b] e4
	} 
> 

\paper 
{
%	linewidth = 5.0 \cm % ly2dvi barfs on -1
	linewidth = 8.0 \cm
%	linewidth = 12.0 \cm	
}
}



