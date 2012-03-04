/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 1998--2012 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#include "side-position-interface.hh"

#include <cmath>                // ceil.
#include <algorithm>
#include <map>

using namespace std;

#include "accidental-interface.hh"
#include "axis-group-interface.hh"
#include "directional-element-interface.hh"
#include "grob.hh"
#include "grob-array.hh"
#include "main.hh"
#include "misc.hh"
#include "note-head.hh"
#include "note-column.hh"
#include "pointer-group-interface.hh"
#include "skyline-pair.hh"
#include "staff-symbol-referencer.hh"
#include "staff-symbol.hh"
#include "stem.hh"
#include "string-convert.hh"
#include "system.hh"
#include "warn.hh"

void
Side_position_interface::add_support (Grob *me, Grob *e)
{
  Pointer_group_interface::add_unordered_grob (me, ly_symbol2scm ("side-support-elements"), e);
}

SCM
finish_offset (Grob *me, Direction dir, Real total_off, Real *current_offset)
{
  Real ss = Staff_symbol_referencer::staff_space (me);
  Real minimum_space = ss * robust_scm2double (me->get_property ("minimum-space"), -1);
  total_off += dir * ss * robust_scm2double (me->get_property ("padding"), 0);

  if (minimum_space >= 0
      && dir
      && total_off * dir < minimum_space)
    total_off = minimum_space * dir;

  if (current_offset)
    total_off = dir * max (dir * total_off,
                           dir * (*current_offset));

  /* FIXME: 1000 should relate to paper size.  */
  if (fabs (total_off) > 1000)
    {
      string msg
        = String_convert::form_string ("Improbable offset for grob %s: %f",
                                       me->name ().c_str (), total_off);

      programming_error (msg);
      if (strict_infinity_checking)
        scm_misc_error (__FUNCTION__, "Improbable offset.", SCM_EOL);
    }

  return scm_from_double (total_off);
}

/* Put the element next to the support, optionally taking in
   account the extent of the support.

   Does not take into account the extent of ME.
*/
SCM
Side_position_interface::general_side_position (Grob *me, Axis a, bool use_extents,
                                                bool include_my_extent,
                                                bool pure, int start, int end,
                                                Real *current_offset)
{
  extract_grob_set (me, "side-support-elements", support);

  Grob *common = common_refpoint_of_array (support, me->get_parent (a), a);
  Grob *staff_symbol = Staff_symbol_referencer::get_staff_symbol (me);
  bool include_staff
    = staff_symbol
      && a == Y_AXIS
      && scm_is_number (me->get_property ("staff-padding"))
      && !to_boolean (me->get_property ("quantize-position"));

  Interval dim;
  Interval staff_extents;
  if (include_staff)
    {
      common = staff_symbol->common_refpoint (common, Y_AXIS);
      staff_extents = staff_symbol->maybe_pure_extent (common, Y_AXIS, pure, start, end);

      if (include_staff)
        dim.unite (staff_extents);
    }

  Direction dir = get_grob_direction (me);

  for (vsize i = 0; i < support.size (); i++)
    {
      Grob *e = support[i];

      // In the case of a stem, we will find a note head as well
      // ignoring the stem solves cyclic dependencies if the stem is
      // attached to a cross-staff beam.
      if (a == Y_AXIS
          && Stem::has_interface (e)
          && dir == - get_grob_direction (e))
        continue;

      if (e)
        {
          if (use_extents)
            dim.unite (e->maybe_pure_extent (common, a, pure, start, end));
          else
            {
              Real x = e->maybe_pure_coordinate (common, a, pure, start, end);
              dim.unite (Interval (x, x));
            }
        }
    }

  if (dim.is_empty ())
    dim = Interval (0, 0);

  Real off = me->get_parent (a)->maybe_pure_coordinate (common, a, pure, start, end);

  Real total_off = dim.linear_combination (dir) - off;
  if (include_my_extent)
    {
      Interval iv = me->maybe_pure_extent (me, a, pure, start, end);
      if (!iv.is_empty ())
        {
          if (!dir)
            {
              programming_error ("direction unknown, but aligned-side wanted");
              dir = DOWN;
            }
          total_off += -iv[-dir];
        }
    }

  return finish_offset (me, dir, total_off, current_offset);
}

SCM
Side_position_interface::skyline_side_position (Grob *me, Axis a,
                                                bool pure, int start, int end,
                                                Real *current_offset)
{
  extract_grob_set (me, "side-support-elements", support);

  Grob *common[2];
  for (Axis ax = X_AXIS; ax < NO_AXES; incr (ax))
    common[ax] = common_refpoint_of_array (support, ax == a ? me->get_parent (ax) : me, ax);

  Grob *staff_symbol = Staff_symbol_referencer::get_staff_symbol (me);
  Direction dir = get_grob_direction (me);

  Skyline my_dim;
  Skyline_pair *sp = Skyline_pair::unsmob (me->get_property ("vertical-skylines"));
  if (sp && a == Y_AXIS && !pure)
    {
      Skyline_pair copy = Skyline_pair (*sp);
      copy.shift (me->relative_coordinate (common[X_AXIS], X_AXIS));
      copy.raise (me->get_parent (Y_AXIS)->relative_coordinate (common[Y_AXIS], Y_AXIS));
      my_dim = copy[-dir];
    }
  else
    {
      Box off;
      for (Axis ax = X_AXIS; ax < NO_AXES; incr (ax))
        {
          if (ax == a)
            off[ax] = me->get_parent (ax)->maybe_pure_coordinate (common[ax], ax, pure, start, end)
                      + me->maybe_pure_extent (me, ax, pure, start, end);
          else
            off[ax] = me->maybe_pure_extent (common[ax], ax, pure, start, end);
        }

      if (off[X_AXIS].is_empty () || off[Y_AXIS].is_empty ())
        return scm_from_double (0.0);

      my_dim = Skyline (off, other_axis (a), -dir);
    }
  bool include_staff
    = staff_symbol
      && a == Y_AXIS
      && scm_is_number (me->get_property ("staff-padding"))
      && !to_boolean (me->get_property ("quantize-position"));

  vector<Box> boxes;
  vector<Skyline_pair *> skyps;
  Real min_h = dir == LEFT ? infinity_f : -infinity_f;
  map<Grob *, vector<Grob *> > note_column_map; // for parts of a note column
  for (vsize i = 0; i < support.size (); i++)
    {
      Grob *e = support[i];

      // In the case of a stem, we will find a note head as well
      // ignoring the stem solves cyclic dependencies if the stem is
      // attached to a cross-staff beam.
      if (a == Y_AXIS
          && Stem::has_interface (e)
          && dir == - get_grob_direction (e))
        continue;

      if (e)
        {
          if (Accidental_interface::has_interface (e))
            {
              vector<Box> bs = Accidental_interface::accurate_boxes (e, common);
              boxes.insert (boxes.end (), bs.begin (), bs.end ());
            }
          else
            {
              if (Note_column::has_interface (e->get_parent (X_AXIS))
                  && to_boolean (me->get_property ("add-stem-support")))
                {
                  note_column_map[e->get_parent (X_AXIS)].push_back (e);
                  continue;
                }

              Skyline_pair *sp = Skyline_pair::unsmob (e->get_property ("vertical-skylines"));
              if (sp && a == Y_AXIS && !pure)
                {
                  Skyline_pair *copy = new Skyline_pair (*sp);
                  copy->shift (e->relative_coordinate (common[X_AXIS], X_AXIS));
                  copy->raise (e->relative_coordinate (common[Y_AXIS], Y_AXIS));
                  skyps.push_back (copy);
                  continue;
                }
              Box b;
              for (Axis ax = X_AXIS; ax < NO_AXES; incr (ax))
                b[ax] = e->maybe_pure_extent (common[ax], ax, pure, start, end);

              if (b[X_AXIS].is_empty () || b[Y_AXIS].is_empty ())
                continue;

              boxes.push_back (b);
              min_h = minmax (dir, b[a][-dir], min_h);
            }
        }
    }

  // this loop ensures that parts of a note column will be in the same box
  // pushes scripts and such over stems instead of just over heads
  for (map<Grob *, vector<Grob *> >::iterator i = note_column_map.begin (); i != note_column_map.end (); i++)
    {
      Box big;
      for (vsize j = 0; j < (*i).second.size (); j++)
        {
          Grob *e = (*i).second[j];
          Box b;
          for (Axis ax = X_AXIS; ax < NO_AXES; incr (ax))
            b[ax] = e->maybe_pure_extent (common[ax], ax, pure, start, end);

          if (b[X_AXIS].is_empty () || b[Y_AXIS].is_empty ())
            continue;

          big.unite (b);
        }
      if (!big[X_AXIS].is_empty () && !big[Y_AXIS].is_empty ())
        boxes.push_back (big);
    }

  Skyline dim (boxes, other_axis (a), dir);
  if (skyps.size ())
    {
      Skyline_pair merged (skyps, other_axis (a));
      for (vsize i = 0; i < skyps.size (); i++)
        delete skyps[i];
      dim.merge (merged[dir]);
    }
  if (!boxes.size ())
    dim.set_minimum_height (0.0);
  else
    dim.set_minimum_height (min_h);

  if (include_staff)
    {
      Interval staff_extents;
      common[Y_AXIS] = staff_symbol->common_refpoint (common[Y_AXIS], Y_AXIS);
      staff_extents = staff_symbol->maybe_pure_extent (common[Y_AXIS], Y_AXIS, pure, start, end);
      dim.set_minimum_height (minmax (dir, min_h, staff_extents[dir]));
    }

  Real dist = dim.distance (my_dim);
  Real total_off = !isinf (dist) ? dir * dist : 0.0;

  return finish_offset (me, dir, total_off, current_offset);
}

MAKE_SCHEME_CALLBACK (Side_position_interface, y_aligned_on_support_refpoints, 1);
SCM
Side_position_interface::y_aligned_on_support_refpoints (SCM smob)
{
  return general_side_position (unsmob_grob (smob), Y_AXIS, false, false, false, 0, 0, 0);
}

MAKE_SCHEME_CALLBACK (Side_position_interface, pure_y_aligned_on_support_refpoints, 3);
SCM
Side_position_interface::pure_y_aligned_on_support_refpoints (SCM smob, SCM start, SCM end)
{
  return general_side_position (unsmob_grob (smob), Y_AXIS, false, false,
                                true, scm_to_int (start), scm_to_int (end), 0);
}

/*
  Position next to support, taking into account my own dimensions and padding.
*/
SCM
axis_aligned_side_helper (SCM smob, Axis a, bool pure, int start, int end, SCM current_off_scm)
{
  Real r;
  Real *current_off_ptr = 0;
  if (scm_is_number (current_off_scm))
    {
      r = scm_to_double (current_off_scm);
      current_off_ptr = &r;
    }

  return Side_position_interface::aligned_side (unsmob_grob (smob), a, pure, start, end, current_off_ptr);
}

MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, x_aligned_side, 2, 1, "");
SCM
Side_position_interface::x_aligned_side (SCM smob, SCM current_off)
{
  return axis_aligned_side_helper (smob, X_AXIS, false, 0, 0, current_off);
}

MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, y_aligned_side, 2, 1, "");
SCM
Side_position_interface::y_aligned_side (SCM smob, SCM current_off)
{
  return axis_aligned_side_helper (smob, Y_AXIS, false, 0, 0, current_off);
}

MAKE_SCHEME_CALLBACK_WITH_OPTARGS (Side_position_interface, pure_y_aligned_side, 4, 1, "");
SCM
Side_position_interface::pure_y_aligned_side (SCM smob, SCM start, SCM end, SCM cur_off)
{
  return axis_aligned_side_helper (smob, Y_AXIS, true,
                                   scm_to_int (start),
                                   scm_to_int (end),
                                   cur_off);
}

MAKE_SCHEME_CALLBACK (Side_position_interface, calc_cross_staff, 1)
SCM
Side_position_interface::calc_cross_staff (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  extract_grob_set (me, "side-support-elements", elts);

  for (vsize i = 0; i < elts.size (); i++)
    if (to_boolean (elts[i]->get_property ("cross-staff")))
      return SCM_BOOL_T;

  Grob *common = common_refpoint_of_array (elts, me->get_parent (Y_AXIS), Y_AXIS);
  return scm_from_bool (common != me->get_parent (Y_AXIS));
}

SCM
Side_position_interface::aligned_side (Grob *me, Axis a, bool pure, int start, int end,
                                       Real *current_off)
{
  Direction dir = get_grob_direction (me);
  bool skyline = to_boolean (me->get_property ("use-skylines"));

  Real o = scm_to_double (skyline && !pure
                          ? skyline_side_position (me, a, pure, start, end, current_off)
                          : general_side_position (me, a, true, true, pure, start, end, current_off));

  /*
    Maintain a minimum distance to the staff. This is similar to side
    position with padding, but it will put adjoining objects on a row if
    stuff sticks out of the staff a little.
  */
  Grob *staff = Staff_symbol_referencer::get_staff_symbol (me);
  if (staff && a == Y_AXIS)
    {
      if (to_boolean (me->get_property ("quantize-position")))
        {
          Grob *common = me->common_refpoint (staff, Y_AXIS);
          Real my_off = me->get_parent (Y_AXIS)->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
          Real staff_off = staff->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
          Real ss = Staff_symbol::staff_space (staff);
          Real position = 2 * (my_off + o - staff_off) / ss;
          Real rounded = directed_round (position, dir);
          Grob *head = me->get_parent (X_AXIS);

          if (fabs (position) <= 2 * Staff_symbol_referencer::staff_radius (me) + 1
              /* In case of a ledger lines, quantize even if we're outside the staff. */
              || (Note_head::has_interface (head)

                  && abs (Staff_symbol_referencer::get_position (head)) > abs (position)))
            {
              o += (rounded - position) * 0.5 * ss;
              if (Staff_symbol_referencer::on_line (me, int (rounded)))
                o += dir * 0.5 * ss;
            }
        }
      else if (scm_is_number (me->get_property ("staff-padding")) && dir)
        {
          Interval iv = me->maybe_pure_extent (me, a, pure, start, end);

          Real staff_padding
            = Staff_symbol_referencer::staff_space (me)
              * scm_to_double (me->get_property ("staff-padding"));

          Grob *parent = me->get_parent (Y_AXIS);
          Grob *common = me->common_refpoint (staff, Y_AXIS);
          Real parent_position = parent->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
          Real staff_position = staff->maybe_pure_coordinate (common, Y_AXIS, pure, start, end);
          Interval staff_extent = staff->maybe_pure_extent (staff, a, pure, start, end);
          Real diff = (dir * staff_extent[dir] + staff_padding
                       - dir * (o + iv[-dir])
                       + dir * (staff_position - parent_position));
          o += dir * max (diff, 0.0);
        }
    }
  return scm_from_double (o);
}

void
Side_position_interface::set_axis (Grob *me, Axis a)
{
  if (!scm_is_number (me->get_property ("side-axis")))
    {
      me->set_property ("side-axis", scm_from_int (a));
      chain_offset_callback (me,
                             (a == X_AXIS)
                             ? x_aligned_side_proc
                             : y_aligned_side_proc,
                             a);
    }
}

Axis
Side_position_interface::get_axis (Grob *me)
{
  if (scm_is_number (me->get_property ("side-axis")))
    return Axis (scm_to_int (me->get_property ("side-axis")));

  string msg = String_convert::form_string ("side-axis not set for grob %s.",
                                            me->name ().c_str ());
  me->programming_error (msg);
  return NO_AXES;
}

MAKE_SCHEME_CALLBACK (Side_position_interface, move_to_extremal_staff, 1);
SCM
Side_position_interface::move_to_extremal_staff (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  System *sys = dynamic_cast<System *> (me->get_system ());
  Direction dir = get_grob_direction (me);
  if (dir != DOWN)
    dir = UP;

  Interval iv = me->extent (sys, X_AXIS);
  iv.widen (1.0);
  Grob *top_staff = sys->get_extremal_staff (dir, iv);

  if (!top_staff)
    return SCM_BOOL_F;

  // Only move this grob if it is a direct child of the system.  We
  // are not interested in moving marks from other staves to the top
  // staff; we only want to move marks from the system to the top
  // staff.
  if (sys != me->get_parent (Y_AXIS))
    return SCM_BOOL_F;

  me->set_parent (top_staff, Y_AXIS);
  me->flush_extent_cache (Y_AXIS);
  Axis_group_interface::add_element (top_staff, me);

  // Remove any cross-staff side-support dependencies
  Grob_array *ga = unsmob_grob_array (me->get_object ("side-support-elements"));
  if (ga)
    {
      vector<Grob *> const &elts = ga->array ();
      vector<Grob *> new_elts;
      for (vsize i = 0; i < elts.size (); ++i)
        {
          if (me->common_refpoint (elts[i], Y_AXIS) == top_staff)
            new_elts.push_back (elts[i]);
        }
      ga->set_array (new_elts);
    }
  return SCM_BOOL_T;
}

ADD_INTERFACE (Side_position_interface,
               "Position a victim object (this one) next to other objects"
               " (the support).  The property @code{direction} signifies where"
               " to put the victim object relative to the support (left or"
               " right, up or down?)\n"
               "\n"
               "The routine also takes the size of the staff into account if"
               " @code{staff-padding} is set.  If undefined, the staff symbol"
               " is ignored.",

               /* properties */
               "add-stem-support "
               "direction "
               "minimum-space "
               "padding "
               "quantize-position "
               "side-axis "
               "side-support-elements "
               "slur-padding "
               "staff-padding "
               "use-skylines "
              );
