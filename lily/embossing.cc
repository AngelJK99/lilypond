/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 1997--2015 Jan Nieuwenhuizen <janneke@gnu.org>

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

#include "embossing.hh"

#include <ctime>
using namespace std;

#include "audio-column.hh"
#include "audio-item.hh"
#include "audio-staff.hh"
#include "file-name.hh"
#include "international.hh"
#include "lily-version.hh"
#include "main.hh"
#include "midi-chunk.hh"
#include "midi-stream.hh"
#include "score.hh"
#include "string-convert.hh"
#include "warn.hh"

Embossing::Embossing (bool ports)
  : midi_ (0),
    ports_ (ports),
    header_ (SCM_EOL)
{
}

Embossing::~Embossing ()
{
  junk_pointers (audio_elements_);
}

void
Embossing::derived_mark () const
{
  scm_gc_mark (header_);
}

SCM
Embossing::get_header () const
{
  return header_;
}

void
Embossing::set_header (SCM module)
{
  assert (ly_is_module (module));
  header_ = module;
}

void
Embossing::output (Midi_stream &midi_stream,
                     const string &embossing_name) const
{
  int tracks_ = audio_staffs_.size ();

  midi_stream.write (Midi_header (1, tracks_, 384));
  debug_output (_ ("Track...") + " ", false);

  //Find the first Audio_item in the performance, so all staves start
  //at the same tick.
  Moment start_mom = 0;
  for (vsize i = 0; i < audio_elements_.size (); i++)
    if (Audio_item *item = dynamic_cast<Audio_item *>(audio_elements_[i]))
      start_mom = min (start_mom, item->audio_column_->when ());

  for (vsize i = 0; i < audio_staffs_.size (); i++)
    {
      Audio_staff *s = audio_staffs_[i];
      if (Audio_control_track_staff *c =
          dynamic_cast<Audio_control_track_staff *>(s))
        {
          // The control track, created by Control_track_performer, should
          // contain a placeholder for the name of the MIDI sequence as its
          // initial audio element.  Fill in the name of the sequence to
          // this element before outputting MIDI.
          assert (!c->audio_items_.empty ());
          Audio_text *text =
            dynamic_cast<Audio_text *>(c->audio_items_.front ());
          assert (text != 0);
          assert (text->type_ == Audio_text::TRACK_NAME);
          assert (text->text_string_ == "control track");
          text->text_string_ = embossing_name;
        }
      debug_output ("[" + ::to_string (i), true);
      s->output (midi_stream, i, ports_, moment_to_ticks (start_mom));
      debug_output ("]", false);
    }
}

void
Embossing::add_element (Audio_element *p)
{
  audio_elements_.push_back (p);
}

void
Embossing::write_output (string out, const string &embossing_name) const
{
  if (out == "-")
    out = "lelie.midi";

  /* Maybe a bit crude, but we had this before */
  File_name file_name (out);
  out = file_name.to_string ();

  Midi_stream midi_stream (out);
  message (_f ("MIDI output to `%s'...", out));

  output (midi_stream, embossing_name);
  progress_indication ("\n");
}

void
Embossing::process ()
{
}
