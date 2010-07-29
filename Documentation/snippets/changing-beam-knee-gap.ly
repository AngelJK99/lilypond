%% Do not edit this file; it is automatically
%% generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.13.29"

\header {
  lsrtags = "rhythms"

%% Translation of GIT committish: 0b55335aeca1de539bf1125b717e0c21bb6fa31b
  texidoces = "
Se insertan automáticamente barras en ángulo cuando se detecta un
intervalo muy grande entre las notas.  Se puede hacer un ajuste
fino de este comportamiento a través de la propiedad
@code{auto-knee-gap}.  Se traza una barra doblada si el salto es
mayor que el valor de @code{auto-knee-gap} más el ancho del objeto
barra (que depende de la duración de las notas y de la inclinación
de la barra).  De forma predeterminada @code{auto-knee-gap} está
establecido a 5.5 espacios de pentagrama.

"
  doctitlees = "Cambiar el salto de las barras en ángulo"

%% Translation of GIT committish: 0a868be38a775ecb1ef935b079000cebbc64de40
texidocde = "
Balken mit Hälsen in unterschiedliche Richtungen werden automatisch
erstellt, wenn ein großer Sprung zwischen Tonhöhen gefunden wird.  Dieses
Verhalten kann durch die @code{auto-knee-gap}-Eigenschaft beeinflusst
werden.  Ein derartiger Knie-Balken wird erstellt, wenn der Abstand größer
ist als der Wert von @code{auto-knee-gap} plus der Dicke des Balkens
(was von der Notendauer und der Neigung des Balkens abhängt).  Der
Standardwert von @code{auto-knee-gap} ist 5.5 Notensystemabstände.

"
  doctitlede = "Balken für weit auseinander liegende Noten ändern"


%% Translation of GIT committish: 708e0d9c660856b19929736ca18a6f9d68539c60
  texidocfr = "
LilyPond insère automatiquement des ligatures coudées --- certaines
hampes vers le haut, d'autres vers le bas --- lorsqu'il détecte un
intervalle important entre des têtes de notes. Ce comportement peut être
changé par l'intermédiaire de l'objet @code{auto-knee-gap} --- défini
par défaut à @samp{5,5} espace, plus la largeur et la pente de la
ligature en question.

"
  doctitlefr = "Ligature et directions de hampe inversées"

  texidoc = "
Kneed beams are inserted automatically when a large gap is detected
between the note heads.  This behavior can be tuned through the
@code{auto-knee-gap} property.  A kneed beam is drawn if the gap is
larger than the value of @code{auto-knee-gap} plus the width of the
beam object  (which depends on the duration of the notes and the slope
of the beam). By default @code{auto-knee-gap} is set to 5.5 staff
spaces.

"
  doctitle = "Changing beam knee gap"
} % begin verbatim

{
  f8 f''8 f8 f''8
  \override Beam #'auto-knee-gap = #6
  f8 f''8 f8 f''8
}

