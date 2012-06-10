/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2011--2012 Mike Solomon <mike@apollinemike.com>

  LilyPond is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LilyPond is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "engraver.hh"

#include "music.hh"
#include "stream-event.hh"
#include "international.hh"
#include "item.hh"
#include "pointer-group-interface.hh"
#include "spanner.hh"
#include "system.hh"

#include "translator.icc"

class Footnote_engraver : public Engraver
{
  TRANSLATOR_DECLARATIONS (Footnote_engraver);

  DECLARE_TRANSLATOR_LISTENER (footnote);
  DECLARE_ACKNOWLEDGER (grob);
  DECLARE_END_ACKNOWLEDGER (grob);
  vector<Stream_event *> events_;
  vector<Drul_array<Spanner *> > annotated_spanners_;

  void stop_translation_timestep ();
  void finalize ();
  virtual void derived_mark () const;

  void footnotify (Grob *, Stream_event *);
};

IMPLEMENT_TRANSLATOR_LISTENER (Footnote_engraver, footnote);
void
Footnote_engraver::listen_footnote (Stream_event *ev)
{
  events_.push_back (ev);
}

void
Footnote_engraver::stop_translation_timestep ()
{
  events_.clear ();
}

void
Footnote_engraver::finalize ()
{
  annotated_spanners_.clear ();
}

void
Footnote_engraver::derived_mark () const
{
  for (vsize i = 0; i < events_.size (); ++i)
    scm_gc_mark (events_[i]->self_scm ());
}

Footnote_engraver::Footnote_engraver ()
{
}

void
Footnote_engraver::footnotify (Grob *g, Stream_event *event)
{
  Spanner *s = dynamic_cast<Spanner *>(g);

  if (s)
    {
      Spanner *b = make_spanner ("FootnoteSpanner", event->self_scm ());
      b->set_parent (s, Y_AXIS);
      b->set_parent (s, X_AXIS);
      Grob *bound = unsmob_grob (get_property ("currentMusicalColumn"));
      b->set_bound (LEFT, bound);
      annotated_spanners_.push_back (Drul_array<Spanner *> (s, b));
    }
  else
    {
      Grob *b = make_item ("FootnoteItem", event->self_scm ());
      b->set_parent (g, Y_AXIS);
      b->set_parent (g, X_AXIS);
    }
}

void
Footnote_engraver::acknowledge_grob (Grob_info info)
{
  Music *mus = unsmob_music (info.grob ()->get_property ("footnote-music"));

  if (mus)
    {
      if (!mus->is_mus_type ("footnote-event")) {
	mus->origin ()->programming_error (_ ("Must be footnote-event."));
	return;
      }
      Stream_event *ev = mus->to_event (context ());
      footnotify (info.grob (), ev);
      ev->unprotect ();
      return;
    }

  // The following performance hog should eventually be removed:
  // instead of adding a -\footnote ... \default articulation at the
  // end of a note, you can perfectly well use \footnote ... before
  // the note.  This is just for the sake of automatic convert-ly
  // rules.

  Stream_event *cause = info.event_cause ();

  SCM arts = cause ? cause->get_property ("articulations") : SCM_EOL;
  for (SCM s = arts; scm_is_pair (s); s = scm_cdr (s))
    {
      Stream_event *e = unsmob_stream_event (scm_car (s));
      if (e->in_event_class ("footnote-event"))
        footnotify (info.grob (), e);
    }

  // In contrast, the following code is only called when actual
  // footnote events have been listened to.  It should not affect
  // performance.

  for (vsize i = 0; i < events_.size (); i++)
    {
      SCM name = events_[i]->get_property ("symbol");
      if (!scm_is_symbol (name)
	  || info.grob ()->name () == ly_symbol2string (name))
        footnotify (info.grob (), events_[i]);
    }
}

void
Footnote_engraver::acknowledge_end_grob (Grob_info info)
{
  Spanner *s = dynamic_cast<Spanner *>(info.grob ());

  if (s)
    for (vsize i = 0; i < annotated_spanners_.size (); i++)
      {
        if (annotated_spanners_[i][LEFT] == s)
          {
            Grob *bound = unsmob_grob (get_property ("currentMusicalColumn"));
            annotated_spanners_[i][RIGHT]->set_bound (RIGHT, bound);
            break;
          }
      }
}

ADD_ACKNOWLEDGER (Footnote_engraver, grob);
ADD_END_ACKNOWLEDGER (Footnote_engraver, grob);

ADD_TRANSLATOR (Footnote_engraver,
                /* doc */
                "Create footnote texts.",

                /* create */
                "FootnoteItem "
                "FootnoteSpanner ",

                /*read*/
                "currentMusicalColumn ",

                /*write*/
                ""
               );
