/*   
  music-constructor.hh -- declare Music_constructor
  
  source file of the GNU LilyPond music typesetter
  
  (c) 2001--2004 Han-Wen Nienhuys <hanwen@cs.uu.nl>
  
 */

#ifndef MUSIC_CONSTRUCTOR_HH
#define MUSIC_CONSTRUCTOR_HH

#include "lily-guile.hh"
#include "string.hh"
#include "global-ctor.hh"

#define ADD_MUSIC(type) \
Music * _ ## type ## _ctor (SCM init)\
{\
  return new type (init);\
}\
static void  _ ## type ## _adder () {\
      add_music_ctor (#type, & _ ## type ## _ctor);\
}\
ADD_SCM_INIT_FUNC( _ ## type ## _adder_prefix,  _ ## type ## _adder);

void add_music_ctor (String, Music* (*) (SCM));
Music*make_music (String, SCM);





#endif /* MUSIC_CONSTRUCTOR_HH */

