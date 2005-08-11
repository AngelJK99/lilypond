/*
  pitch-squash-engraver.cc -- implement Pitch_squash_engraver

  source file of the GNU LilyPond music typesetter

  (c) 1997--2005 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#include "staff-symbol-referencer.hh"
#include "note-head.hh"
#include "rhythmic-head.hh"
#include "engraver.hh"

class Pitch_squash_engraver : public Engraver
{
public:
  TRANSLATOR_DECLARATIONS (Pitch_squash_engraver);
  DECLARE_ACKNOWLEDGER (note_head);
};

void
Pitch_squash_engraver::acknowledge_note_head (Grob_info i)
{
  SCM newpos = get_property ("squashedPosition");
  if (scm_is_number (newpos))
    {
      i.grob ()->set_property ("staff-position", newpos);
    }
}

Pitch_squash_engraver::Pitch_squash_engraver ()
{
}

#include "translator.icc"
ADD_ACKNOWLEDGER (Pitch_squash_engraver,note_head);
ADD_TRANSLATOR (Pitch_squash_engraver,
		/* doc */ 
		"Set the vertical position of noteheads to "
		"@code{squashedPosition}, if that "
		"property is set. "
		"This can be used to make  a single line staff "
		"demonstrating the rhythm of a melody.",

		/* create */ "",
		/* accept */ "",
		/* read */ "squashedPosition",
		/* write */ "");
