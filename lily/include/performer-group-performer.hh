/*
  performer-group-performer.hh -- declare Performer_group_performer

  (c) 1996,  1997--1999 Han-Wen Nienhuys <hanwen@cs.uu.nl>
                 Jan Nieuwenhuizen <janneke@gnu.org>
 */

#ifndef PERFORMER_GROUP_PERFORMER_HH
#define PERFORMER_GROUP_PERFORMER_HH

#include "lily-proto.hh"
#include "parray.hh"
#include "performer.hh"
#include "translator-group.hh"

/**
  Group a number of performers. Usually delegates everything to its contents.
*/

class Performer_group_performer : public Performer, public virtual Translator_group {
public:
  VIRTUAL_COPY_CONS(Translator);
  
};

#endif // PERFORMER_GROUP_PERFORMER_HH

