/*
  span-bar.hh -- declare Span_bar

  source file of the GNU LilyPond music typesetter

  (c)  1997--1999 Han-Wen Nienhuys <hanwen@cs.uu.nl>
*/


#ifndef SPAN_BAR_HH
#define SPAN_BAR_HH

#include "bar.hh"

/**
   This is a barline that is spanned across other bar lines.  This is
   the implementation of the long barlines that occur in orchestral
   score and other multi-staff music.

   TODO: Is this usable for other stuff besides barlines? We only have
   to span a Score_element.  Perhaps this can be used for large time
   sigs?
*/
class Span_bar : public Bar
{
  Interval get_spanned_interval () const;
public:
  Span_bar();
    
  VIRTUAL_COPY_CONS(Score_element);
  void add_bar (Score_element*);
  void set_align (Align_element *);
protected:
  void evaluate_empty ();

  static Interval width_callback(Dimension_cache const*) ;
  static Interval height_callback(Dimension_cache const*) ;  
  
  virtual void do_pre_processing();
  virtual void do_post_processing();
  virtual Molecule * do_brew_molecule_p() const;
};

#endif // SPAN_BAR_HH
