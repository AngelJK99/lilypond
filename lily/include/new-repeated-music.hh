/*   
  new-repeated-music.hh -- declare New_repeated_music
  
  source file of the GNU LilyPond music typesetter
  
  (c) 1999 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef NEW_REPEATED_MUSIC_HH
#define NEW_REPEATED_MUSIC_HH

#include "music.hh"


/**
   Musical repeats.

   A repeat, when played has the form of BODY A BODY B BODY C.

   In this examples, the list {A B C} is put in ALTERNATIVES_P_.  BODY
   is put in REPEAT_BODY_P_.  Either the body or the alternative may
   be omitted.

   There are three modes of representing this  music:

   BODY A
        B
	C

   is called "folded". Mostly used for lyrics.

   
   BODY A B C

   is called "semi" folded.  This is common notation

   BODY A BODY B BODY C

   is called unfolded.  Useful for MIDI.

*/
class New_repeated_music : public Music
{
public:
  bool fold_b_;
  /// if FOLD_B_ is false, semifold this.
  bool semi_fold_b_;

  /// how often do we repeat?
  int repeats_i_;

  Music * repeat_body_p_;
  Music_sequence * alternatives_p_;

  virtual Musical_pitch to_relative_octave (Musical_pitch);

  /// The duration of this piece of music
  virtual Moment length_mom () const;

  Moment alternatives_length_mom () const;

  void print() const;
  /// Transpose, with the interval central C to #p#
  virtual void transpose (Musical_pitch p);

  /// Scale the music in time by #factor#.
  virtual void compress (Moment factor);
  VIRTUAL_COPY_CONS(Music);

  New_repeated_music (Music*, int , Music_sequence*);
  New_repeated_music (New_repeated_music const&);
  ~New_repeated_music ();
protected:
  virtual void do_print() const;
};


#endif /* NEW_REPEATED_MUSIC_HH */
