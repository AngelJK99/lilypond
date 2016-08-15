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

#ifndef EMBOSSING_HH
#define EMBOSSING_HH

#include "std-vector.hh"
#include "music-output.hh"

/* Braille output.  */
class Embossing : public Music_output
{
public:
  Embossing (bool ports = false);
  ~Embossing ();
  DECLARE_CLASSNAME (Embossing);

  SCM get_header () const;
  void set_header (SCM header);

  virtual void derived_mark () const;

  void add_element (Audio_element *p);
  virtual void process ();
  void remap_grace_durations ();
  void output (Midi_stream &midi_stream, const string &performance_name) const;
  void output_header_track (Midi_stream &midi_stream) const;

  void print () const;
  void write_output (string filename, const string &performance_name) const;

  vector<Audio_staff *> audio_staffs_;
  vector<Audio_element *> audio_elements_;
  Output_def *midi_;
  bool ports_;
  SCM header_;
};

#endif /* EMBOSSING_HH */
