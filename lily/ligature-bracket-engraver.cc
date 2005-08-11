/*
  ligature-bracket-engraver.cc -- implement Ligature_bracket_engraver

  source file of the GNU LilyPond music typesetter

  (c) 2002--2005 Juergen Reuter <reuter@ipd.uka.de>
*/

#include "ligature-engraver.hh"
#include "note-column.hh"
#include "tuplet-bracket.hh"
#include "spanner.hh"

/*
 * This engraver marks ligatures of any kind by just printing a
 * horizontal square bracket on top of each ligature.  See class
 * Ligature_engraver for more information on the interaction between
 * this class and its superclass.
 */
class Ligature_bracket_engraver : public Ligature_engraver
{
protected:
  virtual Spanner *create_ligature_spanner ();
  virtual void typeset_ligature (Spanner *ligature,
				 Array<Grob_info> primitives);
  DECLARE_ACKNOWLEDGER (rest);
  DECLARE_ACKNOWLEDGER (note_column);
public:
  TRANSLATOR_DECLARATIONS (Ligature_bracket_engraver);
};

Ligature_bracket_engraver::Ligature_bracket_engraver ()
{
}

Spanner *
Ligature_bracket_engraver::create_ligature_spanner ()
{
  return make_spanner ("LigatureBracket", SCM_EOL);
}

void
Ligature_bracket_engraver::typeset_ligature (Spanner *, Array<Grob_info>)
{
  // no real ligature to typeset; the LigatureBracket just does it
}

void
Ligature_bracket_engraver::acknowledge_note_column (Grob_info info)
{
  if (current_ligature ())
    {
      Tuplet_bracket::add_column (current_ligature (),
				  dynamic_cast<Item *> (info.grob ()));

      // avoid "junking empty ligature" message by acknowledging dummy
      // note head
      Ligature_engraver::acknowledge_note_head (Grob_info ());
    }
}

void
Ligature_bracket_engraver::acknowledge_rest (Grob_info info)
{
  if (current_ligature ())
    Ligature_engraver::acknowledge_rest (info);
}

#include "translator.icc"

ADD_ACKNOWLEDGER (Ligature_bracket_engraver, rest);
ADD_ACKNOWLEDGER (Ligature_bracket_engraver, note_column);
ADD_TRANSLATOR (Ligature_bracket_engraver,
		/* doc */ "Handles Ligature_events by engraving Ligature brackets.",
		/* create */ "TupletBracket",
		/* accept */ "ligature-event",
		/* read */ "",
		/* write */ "");
