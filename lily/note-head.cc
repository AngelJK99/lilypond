/*
  notehead.cc -- implement Note_head

  source file of the GNU LilyPond music typesetter

  (c)  1997--2003 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/
#include <math.h>
#include <ctype.h>

#include "misc.hh"
#include "dots.hh"
#include "note-head.hh"
#include "warn.hh"
#include "font-interface.hh"
#include "molecule.hh"
#include "event.hh"
#include "rhythmic-head.hh"
#include "staff-symbol-referencer.hh"
#include "lookup.hh"
#include "paper-def.hh"

/*
  Note_head contains the code for printing note heads.

  Ledger lines:

  It also contains the ledger lines, for historical reasons.  Ledger
  lines are somewhat of a PITA. In some cases, they take up no space, in
  some cases they don't:

  DO take space:

  - when ledgered notes are juxtaposed: there should be some white
   space between the ledger lines.

  - when accidentals are near: the accidentals should not be on the
  ledger lines

  [both tips by Heinz Stolba from Universal Edition].

  DO NOT take space into account:

  - for basically everything else, e.g. swapping ledgered notes on
   clustered chords, spacing between ledgered and unledgered notes.
  
  TODO: fix this. It is not feasible to have a special grob for
  ledgers, since you basically don't know if there will be ledgers,
  unless you know at interpretation phase already 1. the Y-position,
  2. the number of staff lines. It's not yet specced when both pieces
  of information are there, so for now, it is probably better to build
  special support for ledgers into the accidental and separation-item
  code.

  (Besides a separate ledger seems overkill. For what else would
  it be useful?)

*/

/*
  TODO: ledger lines are also a property of the staff. Maybe move them
  to there?
 */
Molecule
Note_head::brew_ledger_lines (Grob *me,
                              int pos,
                              int interspaces,
                              Interval x_extent,
			      bool take_space)
{
  Real inter_f = Staff_symbol_referencer::staff_space (me)/2;
  int line_count = (abs (pos) < interspaces)
    ? 0
    : (abs (pos) - interspaces) / 2;
  Molecule molecule = Molecule();


  if (line_count)
    {
      Real ledgerlinethickness =
	(me->get_paper ()->get_realvar (ly_symbol2scm ("ledgerlinethickness")));
      Real blotdiameter = ledgerlinethickness;
      //	(me->get_paper ()->get_realvar (ly_symbol2scm ("blotdiameter")));
      Interval y_extent =
	Interval (-0.5*(ledgerlinethickness),
		  +0.5*(ledgerlinethickness));
      Box ledger_line (x_extent, y_extent);

      Molecule proto_ledger_line =
	Lookup::round_filled_box (ledger_line, blotdiameter);

      if (!take_space)
        proto_ledger_line.set_empty (true);

      
      Direction dir = (Direction)sign (pos);
      Real offs = (Staff_symbol_referencer::on_staffline (me, pos))
        ? 0.0
        : -dir * inter_f;
      for (int i = 0; i < line_count; i++)
        {
          Molecule ledger_line (proto_ledger_line);
          ledger_line.translate_axis (-dir * inter_f * i * 2 + offs, Y_AXIS);
          molecule.add_molecule (ledger_line);
        }
    }

  return molecule;
}

Molecule
internal_brew_molecule (Grob *me, bool with_ledgers)
{
  SCM style  = me->get_grob_property ("style");
  if (!gh_symbol_p (style))
    {
      return Molecule ();
    }

  SCM log = gh_int2scm (Note_head::get_balltype (me));
  SCM proc = me->get_grob_property ("glyph-name-procedure");
  SCM scm_font_char = scm_call_2 (proc, log, style);
  String font_char = "noteheads-" + ly_scm2string (scm_font_char);

  Font_metric * fm = Font_interface::get_default_font (me);
  Molecule out = fm->find_by_name (font_char);
  if (out.empty_b())
    {
      me->warning (_f ("note head `%s' not found", font_char.to_str0 ()));
    }

  int interspaces = Staff_symbol_referencer::line_count (me)-1;
  int pos = (int)rint (Staff_symbol_referencer::get_position (me));
  if (with_ledgers && interspaces >= 0
      && abs (pos) - interspaces > 1)
    {
      Interval hd = out.extent (X_AXIS);
      Real left_ledger_protusion = hd.length ()/4;
      Real right_ledger_protusion = left_ledger_protusion;

      if (Grob * g = unsmob_grob(me->get_grob_property ("accidental-grob")))
	{
	  /*
	    make a little room for accidentals.
	  
	    TODO: this will look silly if a chord has ledger lines,
	    and only the bottom note has an accidental.
	  */

	  Grob *common = g->common_refpoint (me, X_AXIS);
	  Real d =
	    me->extent (common, X_AXIS)[LEFT]
	    -g->extent (common, X_AXIS)[RIGHT];
	  
	  left_ledger_protusion = left_ledger_protusion <? (d/2);
	}

      Interval l_extents = Interval (hd[LEFT] - left_ledger_protusion,
				     hd[RIGHT] + right_ledger_protusion);
      out.add_molecule (Note_head::brew_ledger_lines (me, pos, interspaces,
						      l_extents, false));
    }
  return out;
}


MAKE_SCHEME_CALLBACK (Note_head,brew_molecule,1);
SCM
Note_head::brew_molecule (SCM smob)  
{
  Grob *me = unsmob_grob (smob);

  /*
    ledgers don't take space. See top of file.
   */
  return internal_brew_molecule (me, true).smobbed_copy ();
}

/*
  Compute the width the head without ledgers.

  -- there used to be some code from the time that ledgers
  did take space. Nowadays, we can simply take the standard extent.
 */
Interval
Note_head::head_extent (Grob *me, Axis a)
{
  SCM brewer = me->get_grob_property ("molecule-callback");
  if (brewer == Note_head::brew_molecule_proc)
    {
      Molecule mol = internal_brew_molecule (me, false);
  
      if (!mol.empty_b())
	return mol.extent (a);
    }
  else
    {
      Molecule * mol = me->get_molecule ();
      if (mol)
	return  mol->extent (a) ;
    }
  
  return Interval (0,0);
}

/*
  This is necessary to prevent a cyclic dependency: the appearance of
  the ledgers depends on positioning, so the Grob::get_molecule() can
  not be used for determining the note head extent.
  
 */ 
MAKE_SCHEME_CALLBACK (Note_head,extent,2);
SCM
Note_head::extent (SCM smob, SCM axis)  
{
  Grob *me = unsmob_grob (smob);

  return ly_interval2scm (head_extent (me, (Axis) gh_scm2int (axis)));
}

MAKE_SCHEME_CALLBACK (Note_head,brew_ez_molecule,1);
SCM
Note_head::brew_ez_molecule (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  int l = Note_head::get_balltype (me);

  int b = (l >= 2);

  SCM cause = me->get_grob_property ("cause");
  SCM spitch = unsmob_music (cause)->get_mus_property ("pitch");
  Pitch* pit =  unsmob_pitch (spitch);

  char s[2] = "a";
  s[0] = (pit->get_notename () + 2)%7 + 'a';
  s[0] = toupper (s[0]);
  
  SCM charstr = scm_makfrom0str (s);
  
  SCM at = scm_list_n (ly_symbol2scm ("ez-ball"),
		       charstr,
		       gh_int2scm (b),
		       gh_int2scm (1-b),
		       SCM_UNDEFINED);
  Box bx (Interval (0, 1.0), Interval (-0.5, 0.5));
  Molecule m (bx, at);

  int pos = (int)rint (Staff_symbol_referencer::get_position (me));
  int interspaces = Staff_symbol_referencer::line_count (me)-1;
  if (abs (pos) - interspaces > 1)
    {
      Interval hd = m.extent (X_AXIS);
      Real hw = hd.length ()/4;
      Interval extent = Interval (hd[LEFT] - hw, hd[RIGHT] + hw);
      m.add_molecule (brew_ledger_lines (me, pos, interspaces, extent, false));
    }

  return m.smobbed_copy ();
}


Real
Note_head::stem_attachment_coordinate (Grob *me, Axis a)
{
  SCM brewer = me->get_grob_property ("molecule-callback");
  Font_metric * fm  = Font_interface::get_default_font (me);
  
  if (brewer == Note_head::brew_molecule_proc)
    {
      SCM style  = me->get_grob_property ("style");
      if (!gh_symbol_p (style))
	{
	  return 0.0;
	}
      
      SCM log = gh_int2scm (Note_head::get_balltype (me));
      SCM proc = me->get_grob_property ("glyph-name-procedure");
      SCM scm_font_char = scm_call_2 (proc, log, style);
      String font_char = "noteheads-" + ly_scm2string (scm_font_char);

      int k = fm->name_to_index (font_char);
      Box b = fm->get_indexed_char (k);
      Offset wxwy = fm->get_indexed_wxwy (k);
      Interval v = b[a];
      if (!v.empty_b ())
	return 2 * (wxwy[a] - v.center()) / v.length ();
    }

  /*
    Fallback
   */
  SCM v = me->get_grob_property ("stem-attachment-function");
  if (!gh_procedure_p (v))
    return 0.0;
  
  SCM result = scm_call_2 (v, me->self_scm(), gh_int2scm (axis));
  if (!gh_pair_p (result))
    return 0.0;

  result = (a == X_AXIS) ? ly_car (result) : ly_cdr (result);
  
  return gh_number_p (result) ?  gh_scm2double (result) : 0.0;
}
int
Note_head::get_balltype (Grob*me) 
{
  SCM s = me->get_grob_property ("duration-log");
  return gh_number_p (s) ? gh_scm2int (s) <? 2 : 0;
}

ADD_INTERFACE (Note_head,"note-head-interface",
  "Note head",
  "glyph-name-procedure accidental-grob style stem-attachment-function");

