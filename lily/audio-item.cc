/*
  audio-item.cc -- implement Audio items.

  source file of the GNU LilyPond music typesetter

  (c)  1997--1998 Jan Nieuwenhuizen <jan@digicash.com>
*/
#include "debug.hh"
#include "audio-item.hh"
#include "midi-item.hh"
#include "request.hh"
#include "audio-column.hh"

Audio_instrument::Audio_instrument (String instrument_str)
  : Audio_item (0)
{
  str_ = instrument_str;
}

Midi_item*
Audio_instrument::midi_item_p()
{
  return str_.length_i() ? new Midi_instrument(0, str_) : 0;
}
IMPLEMENT_IS_TYPE_B1(Audio_instrument, Audio_item);

Audio_item::Audio_item (Request* req_l)
{
  audio_column_l_ = 0;
  req_l_ = req_l;
}

Audio_key::Audio_key (Request* req_l)
  : Audio_item (req_l)
{
}

Midi_item*
Audio_key::midi_item_p()
{
  return new Midi_key (this);
}
IMPLEMENT_IS_TYPE_B1 (Audio_key, Audio_item);

Audio_note::Audio_note (Request* req_l)
  : Audio_item (req_l)
{
}

Midi_item*
Audio_note::midi_item_p()
{
  return new Midi_note (this);
}

IMPLEMENT_IS_TYPE_B1(Audio_note, Audio_item);

Audio_tempo::Audio_tempo (int per_minute_4_i)
  : Audio_item (0)
{
  per_minute_4_i_ = per_minute_4_i;
}

Midi_item*
Audio_tempo::midi_item_p()
{
  return new Midi_tempo (this);
}

IMPLEMENT_IS_TYPE_B1(Audio_tempo, Audio_item);

Audio_meter::Audio_meter (Request* req_l)
  : Audio_item (req_l)
{
}

Midi_item*
Audio_meter::midi_item_p()
{
  return new Midi_meter (this);
}
IMPLEMENT_IS_TYPE_B1(Audio_meter, Audio_item);

Audio_text::Audio_text (Audio_text::Type type, String text_str)
  : Audio_item (0)
{
  text_str_ = text_str;
  type_ = type;
}

Midi_item*
Audio_text::midi_item_p()
{
  return text_str_.length_i() ? new Midi_text(this) : 0;
}

IMPLEMENT_IS_TYPE_B1(Audio_text, Audio_item);

IMPLEMENT_IS_TYPE_B1(Audio_item, Audio_element);

void
Audio_item::do_print () const
{
#ifndef NPRINT
  if (audio_column_l_)
    {
      DOUT << "at: "<< audio_column_l_->at_mom ();
    }
  if (req_l_)
    {
      DOUT << "from: ";
      req_l_->print ();
    }
#endif
}

