/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2004--2012 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#include "parse-scm.hh"

#include <cstdio>
using namespace std;

#include "lily-parser.hh"
#include "lily-lexer.hh"
#include "international.hh"
#include "main.hh"
#include "paper-book.hh"
#include "source-file.hh"

/* Pass string to scm parser, read one expression.
   Return result value and #chars read.

   Thanks to Gary Houston <ghouston@freewire.co.uk>  */
SCM
internal_ly_parse_scm (Parse_start *ps)
{
  Source_file *sf = ps->start_location_.get_source_file ();
  SCM port = sf->get_port ();

  long off = ps->start_location_.start () - sf->c_str ();

  scm_seek (port, scm_from_long (off), scm_from_long (SEEK_SET));
  SCM from = scm_ftell (port);

  scm_set_port_line_x (port, scm_from_int (ps->start_location_.line_number () - 1));
  scm_set_port_column_x (port, scm_from_int (ps->start_location_.column_number () - 1));

  bool multiple = ly_is_equal (scm_peek_char (port), SCM_MAKE_CHAR ('@'));

  if (multiple)
    (void) scm_read_char (port);

  SCM form = scm_read (port);
  SCM to = scm_ftell (port);

  ps->nchars = scm_to_int (to) - scm_to_int (from);

  if (!SCM_EOF_OBJECT_P (form))
    {
      if (ps->parser_->lexer_->top_input ())
        {
          // Find any precompiled form.
          SCM c = scm_assv_ref (ps->parser_->closures_, from);
          if (scm_is_true (c))
            // Replace form with a call to previously compiled closure
            form = scm_list_1 (c);
        }
      if (multiple)
	form = scm_list_3 (ly_symbol2scm ("apply"),
			   ly_symbol2scm ("values"),
			   form);
      return scm_cons (form, make_input (ps->start_location_));
    }

  /* Don't close the port here; if we re-enter this function via a
     continuation, then the next time we enter it, we'll get an error.
     It's a string port anyway, so there's no advantage to closing it
     early. */
  // scm_close_port (port);

  return SCM_UNDEFINED;
}

SCM
internal_ly_eval_scm (Parse_start *ps)
{
  if (ps->safe_)
    {
      static SCM module = SCM_BOOL_F;
      if (module == SCM_BOOL_F)
        {
          SCM function = ly_lily_module_constant ("make-safe-lilypond-module");
          module = scm_gc_protect_object (scm_call_0 (function));
        }

      // We define the parser so trusted Scheme functions can
      // access the real namespace underlying the parser.
      if (ps->parser_)
        scm_module_define (module, ly_symbol2scm ("parser"),
                           ps->parser_->self_scm ());
      return scm_eval (ps->form_, module);
    }
  return scm_primitive_eval (ps->form_);
}

SCM
catch_protected_parse_body (void *p)
{
  Parse_start *ps = (Parse_start *) p;

  return (*ps->func_) (ps);
}

SCM
parse_handler (void *data, SCM tag, SCM args)
{
  Parse_start *ps = (Parse_start *) data;

  ps->start_location_.error (_ ("GUILE signaled an error for the expression beginning here"));

  if (scm_ilength (args) > 2)
    scm_display_error_message (scm_cadr (args), scm_caddr (args), scm_current_error_port ());

  if (tag == ly_symbol2scm ("read-error"))
    ps->nchars = 1;

  return SCM_UNDEFINED;
}

SCM
protected_ly_parse_scm (Parse_start *ps)
{
  /*
    Catch #t : catch all Scheme level errors.
   */
  return scm_internal_catch (SCM_BOOL_T,
                             &catch_protected_parse_body,
                             (void *) ps,
                             &parse_handler, (void *) ps);
}

bool parse_protect_global = true;
bool parsed_objects_should_be_dead = false;

/* Try parsing.  Upon failure return SCM_UNDEFINED. */

SCM
ly_parse_scm (char const *s, int *n, Input i, bool safe, Lily_parser *parser)
{
  Parse_start ps;
  ps.str = s;
  ps.start_location_ = i;
  ps.safe_ = safe;
  ps.form_ = SCM_UNDEFINED;
  ps.parser_ = parser;
  ps.func_ = internal_ly_parse_scm;

  SCM ans = parse_protect_global ? protected_ly_parse_scm (&ps)
            : internal_ly_parse_scm (&ps);
  *n = ps.nchars;

  return ans;
}

SCM
ly_eval_scm (SCM form, Input i, bool safe, Lily_parser *parser)
{
  Parse_start ps;
  ps.str = 0;
  ps.start_location_ = i;
  ps.safe_ = safe;
  ps.form_ = form;
  ps.parser_ = parser;
  ps.func_ = internal_ly_eval_scm;

  SCM ans = parse_protect_global ? protected_ly_parse_scm (&ps)
            : internal_ly_eval_scm (&ps);
  scm_remember_upto_here_1 (form);
  return ans;
}
