\header{
filename =	 "corni.ly";
% %title =	 "Ouvert\\"ure zu Collins Trauerspiel \\"Coriolan\\" Opus 62";
description =	 "";
composer =	 "Ludwig van Beethoven (1770-1827)";
enteredby =	 "JCN";
copyright =	 "public domain";


}

\version "1.0.19";

\include "corno-1.ly"
\include "corno-2.ly"

$corni_staff = \context Staff = corni <
	\property Staff.midiInstrument = "french horn"
	\property Staff.instrument = "2 Corni (E\\textflat)"
	\property Staff.instr = "Cor. (E\\textflat)"
	% urg: can't; only My_midi_lexer:<non-static> () parses pitch?
	%\property Staff.transposing = "es"
	\property Staff.transposing = 3
	\notes \context Voice=corni < 
		\time 4/4;
		\$corno1
		\$corno2
	>
>

