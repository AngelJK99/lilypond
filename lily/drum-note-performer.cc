/*
  note-performer.cc -- implement Drum_note_performer

  source file of the GNU LilyPond music typesetter

  (c) 1996--2004 Jan Nieuwenhuizen <janneke@gnu.org>
 */

#include "performer.hh"
#include "event.hh"
#include "audio-item.hh"
#include "audio-column.hh"
#include "global-translator.hh"
#include "warn.hh"

class Drum_note_performer : public Performer {
public:
  TRANSLATOR_DECLARATIONS(Drum_note_performer);
  
protected:
  virtual bool try_music (Music *ev) ;
  virtual void stop_translation_timestep ();
  virtual void create_audio_elements ();

  Global_translator* get_global_translator ();
private:
  Link_array<Music> note_evs_;
  Link_array<Audio_note> notes_;
  Link_array<Audio_note> delayeds_;
};

void 
Drum_note_performer::create_audio_elements ()
{
  SCM tab =0;
  if (!tab) tab = get_property ("drumPitchTable");
  
  while (note_evs_.size ())
    {
      Music* n = note_evs_.pop ();
      SCM sym = n->get_mus_property ("drum-type");
      SCM defn = SCM_EOL;

      if (gh_symbol_p (sym)
	  &&  (scm_hash_table_p (tab) == SCM_BOOL_T))
	defn = scm_hashq_ref (tab, sym, SCM_EOL);
      
      if (Pitch * pit = unsmob_pitch (defn))
	{
	  Audio_note* p = new Audio_note (*pit,  n->get_length (), 0);
	  Audio_element_info info (p, n);
	  announce_element (info);
	  notes_.push (p);
	}
    }
  
  note_evs_.clear ();
}

Global_translator*
Drum_note_performer::get_global_translator ()
{
  Translator *t = this;
  Global_translator *global =0;
  do
    {
      t = t->daddy_trans_ ;
      global = dynamic_cast<Global_translator*> (t);
    }
  while (!global);

  return global;
}


void
Drum_note_performer::stop_translation_timestep ()
{
  // why don't grace notes show up here?
  // --> grace notes effectively do not get delayed
  Global_translator* global = get_global_translator ();
  for (int i=0; i < notes_.size (); i++)
    {
      Audio_note* n = notes_[i];
      Moment m= n->delayed_until_mom_;
      if (m.to_bool ())
	{
	  global->add_moment_to_process (m);
	  delayeds_.push (n);
	  notes_[i] = 0;
	  notes_.del (i);
	  i--;
	}
    }

  Moment now = now_mom ();
  for (int i=0; i < notes_.size (); i++)
    {
      play_element (notes_[i]);
    }
  notes_.clear ();
  note_evs_.clear ();
  for (int i=0; i < delayeds_.size (); i++)
    {
      Audio_note* n = delayeds_[i];
      if (n->delayed_until_mom_ <= now)
	{
	  play_element (n);
	  delayeds_[i] = 0;
	  delayeds_.del (i);
	  i--;
	}
    }
}
 
bool
Drum_note_performer::try_music (Music* ev)
{
  if (ev->is_mus_type ("note-event"))
    {
      note_evs_.push (ev);
      return true;
    }
  else if (ev->is_mus_type ("busy-playing-event"))
    return note_evs_.size ();
  
  return false;
}

ENTER_DESCRIPTION(Drum_note_performer,
		  "Play drum notes.","",
		  "note-event busy-playing-event","","","");

Drum_note_performer::Drum_note_performer()
{
}
