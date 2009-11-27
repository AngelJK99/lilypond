%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.4"

\header {
  lsrtags = "text"

%% Translation of GIT committish: b2d4318d6c53df8469dfa4da09b27c15a374d0ca
  texidoces = "
Si están especificadas, las marcas de texto se pueden alinear con
objetos de notación distintos a las líneas divisorias.  Entre estos
objetos se encuentran @code{ambitus}, @code{breathing-sign},
@code{clef}, @code{custos}, @code{staff-bar}, @code{left-edge},
@code{key-cancellation}, @code{key-signature} y @code{time-signature}.

En estos casos, las marcas de texto se centran horizontalmente sobre
el objeto, aunque esto se puede cambiar, como se muestra en la segunda
línea de este ejemplo (en una partitura con varios pentagramas, se
debe hacer este ajuste para todos los pentagramas).

"
  doctitlees = "Alinear marcas con varios objetos de notación"


%% Translation of GIT committish: d96023d8792c8af202c7cb8508010c0d3648899d
  texidocde = "
Wenn angegeben, können Textzeichen auch an anderen Objekten als Taktstrichen
ausgerichtet werden.  Zu diesen Objekten gehören @code{ambitus},
@code{breathing-sign}, @code{clef}, @code{custos}, @code{staff-bar},
@code{left-edge}, @code{key-cancellation}, @code{key-signature} und
@code{time-signature}.

In diesem Fall werden die Zeichen horizontal über dem Objekt zentriert.
Diese Ausrichtung kann auch geändert werden, wie die zweite Zeile
des Beispiels zeigt.  In einer Partitur mit vielen Systemen sollte
diese Einstellung für alle Systeme gemacht werden.

"

  doctitlede = "Zeichen an verschiedenen Notationsobjekten ausrichten"

%% Translation of GIT committish: c2e8b1d6d671dbfc138f890cbc7e9882b7be2761
  texidocfr = "
Les indications textuelles peuvent s'aligner par rapport à d'autres
objets que des barres de mesure, tels que @code{ambitus},
@code{breathing-sign}, @code{clef}, @code{custos}, @code{staff-bar},
@code{left-edge}, @code{key-cancellation}, @code{key-signature}, ou
@code{time-signature}.



Par défaut, les indications textuelles sont alignées avec le milieu
des objets de notation. Bien entendu, vous pouvez modifier les
propriétés des objets en question pour obtenir un autre
résultat comme l'illustre la deuxième ligne de l'exemple.  Dans le cas
de portées multiples, ces réglages doivent être faits pour chacune
d'entre elles.



"
  doctitlefr = "Alignement des indications par rapport à divers objets
de notation"


  texidoc = "
If specified, text marks may be aligned with notation objects other
than bar lines.  These objects include @code{ambitus},
@code{breathing-sign}, @code{clef}, @code{custos}, @code{staff-bar},
@code{left-edge}, @code{key-cancellation}, @code{key-signature}, and
@code{time-signature}.



In such cases, text marks will be horizontally centered above the
object. However this can be changed, as demonstrated on the second line
of this example (in a score with multiple staves, this setting should
be done for all the staves).



"
  doctitle = "Aligning marks with various notation objects"
} % begin verbatim

\relative c' {
  e1
  
  % the RehearsalMark will be centered above the Clef
  \override Score.RehearsalMark #'break-align-symbols = #'(clef)
  \key a \major
  \clef treble
  \mark "↓"
  e1
  
  % the RehearsalMark will be centered above the TimeSignature
  \override Score.RehearsalMark #'break-align-symbols = #'(time-signature)
  \key a \major
  \clef treble
  \time 3/4
  \mark \markup { \char ##x2193 }
  e2.
  
  % the RehearsalMark will be centered above the KeySignature
  \override Score.RehearsalMark #'break-align-symbols = #'(key-signature)
  \key a \major
  \clef treble
  \time 4/4
  \mark \markup { \char ##x2193 }
  e1

  \break
  e1
  
  % the RehearsalMark will be aligned with the left edge of the KeySignature
  \once \override Score.KeySignature #'break-align-anchor-alignment = #LEFT
  \mark \markup { \char ##x2193 }
  \key a \major
  e1
  
  % the RehearsalMark will be aligned with the right edge of the KeySignature
  \once \override Score.KeySignature #'break-align-anchor-alignment = #RIGHT
  \key a \major
  \mark \markup { \char ##x2193 }
  e1
  
  % the RehearsalMark will be aligned with the left edge of the KeySignature
  % and then shifted right by one unit.
  \once \override Score.KeySignature #'break-align-anchor = #1
  \key a \major
  \mark \markup { \char ##x2193 }
  e1
}
