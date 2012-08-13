/*
  This file is part of LilyPond, the GNU music typesetter.

  Copyright (C) 2000--2012 Han-Wen Nienhuys <hanwen@xs4all.nl>

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

#include "axis-group-interface.hh"

#include <map>

#include "align-interface.hh"
#include "directional-element-interface.hh"
#include "grob-array.hh"
#include "hara-kiri-group-spanner.hh"
#include "international.hh"
#include "lookup.hh"
#include "paper-column.hh"
#include "paper-score.hh"
#include "pointer-group-interface.hh"
#include "separation-item.hh"
#include "skyline-pair.hh"
#include "staff-grouper-interface.hh"
#include "stem.hh"
#include "stencil.hh"
#include "system.hh"
#include "warn.hh"
#include "unpure-pure-container.hh"

static bool
pure_staff_priority_less (Grob *const &g1, Grob *const &g2);

void
Axis_group_interface::add_element (Grob *me, Grob *e)
{
  SCM axes = me->get_property ("axes");
  if (!scm_is_pair (axes))
    programming_error ("axes should be nonempty");

  for (SCM ax = axes; scm_is_pair (ax); ax = scm_cdr (ax))
    {
      Axis a = (Axis) scm_to_int (scm_car (ax));

      if (!e->get_parent (a))
        e->set_parent (me, a);

      e->set_object ((a == X_AXIS)
                     ? ly_symbol2scm ("axis-group-parent-X")
                     : ly_symbol2scm ("axis-group-parent-Y"),
                     me->self_scm ());
    }

  /* must be ordered, because Align_interface also uses
     Axis_group_interface  */
  Pointer_group_interface::add_grob (me, ly_symbol2scm ("elements"), e);
}

bool
Axis_group_interface::has_axis (Grob *me, Axis a)
{
  SCM axes = me->get_property ("axes");

  return (SCM_BOOL_F != scm_memq (scm_from_int (a), axes));
}

Interval
Axis_group_interface::relative_group_extent (vector<Grob *> const &elts,
                                             Grob *common, Axis a)
{
  return relative_maybe_bound_group_extent (elts, common, a, false);
}

Interval
Axis_group_interface::relative_maybe_bound_group_extent (vector<Grob *> const &elts,
                                                         Grob *common, Axis a, bool bound)
{
  Interval r;
  for (vsize i = 0; i < elts.size (); i++)
    {
      Grob *se = elts[i];
      if (!to_boolean (se->get_property ("cross-staff")))
        {
          Interval dims = (bound && has_interface (se)
                           ? generic_bound_extent (se, common, a)
                           : se->extent (common, a));
          if (!dims.is_empty ())
            r.unite (dims);
        }
    }
  return r;
}

Interval
Axis_group_interface::generic_bound_extent (Grob *me, Grob *common, Axis a)
{
  /* trigger the callback to do skyline-spacing on the children */
  if (a == Y_AXIS)
    (void) me->get_property ("vertical-skylines");

  extract_grob_set (me, "elements", elts);
  vector<Grob *> new_elts;

  SCM interfaces = me->get_property ("bound-alignment-interfaces");

  for (vsize i = 0; i < elts.size (); i++)
    for (SCM l = interfaces; scm_is_pair (l); l = scm_cdr (l))
      if (elts[i]->internal_has_interface (scm_car (l)))
        new_elts.push_back (elts[i]);

  if (!new_elts.size ())
    return robust_relative_extent (me, common, a);

  if (!common)
    common = common_refpoint_of_array (new_elts, me, a);

  return relative_maybe_bound_group_extent (new_elts, common, a, true);
}

Interval
Axis_group_interface::sum_partial_pure_heights (Grob *me, int start, int end)
{
  Interval iv = begin_of_line_pure_height (me, start);
  iv.unite (rest_of_line_pure_height (me, start, end));

  return iv;
}

Interval
Axis_group_interface::part_of_line_pure_height (Grob *me, bool begin, int start, int end)
{
  Spanner *sp = dynamic_cast<Spanner *> (me);
  SCM cache_symbol = begin
                     ? ly_symbol2scm ("begin-of-line-pure-height")
                     : ly_symbol2scm ("rest-of-line-pure-height");
  SCM cached = sp->get_cached_pure_property (cache_symbol, start, end);
  if (scm_is_pair (cached))
    return robust_scm2interval (cached, Interval (0, 0));

  SCM adjacent_pure_heights = me->get_property ("adjacent-pure-heights");
  Interval ret;

  if (!scm_is_pair (adjacent_pure_heights))
    ret = Interval (0, 0);
  else
    {
      SCM these_pure_heights = begin
                               ? scm_car (adjacent_pure_heights)
                               : scm_cdr (adjacent_pure_heights);

      if (scm_is_vector (these_pure_heights))
        ret = combine_pure_heights (me, these_pure_heights, start, end);
      else
        ret = Interval (0, 0);
    }

  sp->cache_pure_property (cache_symbol, start, end, ly_interval2scm (ret));
  return ret;
}

Interval
Axis_group_interface::begin_of_line_pure_height (Grob *me, int start)
{
  return part_of_line_pure_height (me, true, start, start + 1);
}

Interval
Axis_group_interface::rest_of_line_pure_height (Grob *me, int start, int end)
{
  return part_of_line_pure_height (me, false, start, end);
}

Interval
Axis_group_interface::combine_pure_heights (Grob *me, SCM measure_extents, int start, int end)
{
  Paper_score *ps = get_root_system (me)->paper_score ();
  vector<vsize> breaks = ps->get_break_indices ();
  vector<Grob *> cols = ps->get_columns ();

  Interval ext;
  for (vsize i = 0; i + 1 < breaks.size (); i++)
    {
      int r = Paper_column::get_rank (cols[breaks[i]]);
      if (r >= end)
        break;

      if (r >= start)
        ext.unite (ly_scm2interval (scm_c_vector_ref (measure_extents, i)));
    }

  return ext;
}

// adjacent-pure-heights is a pair of vectors, each of which has one element
// for every measure in the score. The first vector stores, for each measure,
// the combined height of the elements that are present only when the bar
// is at the beginning of a line. The second vector stores, for each measure,
// the combined height of the elements that are present only when the bar
// is not at the beginning of a line.
MAKE_SCHEME_CALLBACK (Axis_group_interface, adjacent_pure_heights, 1)
SCM
Axis_group_interface::adjacent_pure_heights (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  Grob *common = unsmob_grob (me->get_object ("pure-Y-common"));
  extract_grob_set (me, "pure-relevant-grobs", elts);

  Paper_score *ps = get_root_system (me)->paper_score ();
  vector<vsize> ranks = ps->get_break_ranks ();

  vector<Interval> begin_line_heights;
  vector<Interval> mid_line_heights;
  vector<Interval> begin_line_staff_heights;
  vector<Interval> mid_line_staff_heights;
  begin_line_heights.resize (ranks.size () - 1);
  mid_line_heights.resize (ranks.size () - 1);

  for (vsize i = 0; i < elts.size (); ++i)
    {
      Grob *g = elts[i];

      if (to_boolean (g->get_property ("cross-staff")))
        continue;

      bool outside_staff = scm_is_number (g->get_property ("outside-staff-priority"));
      Real padding = robust_scm2double (g->get_property ("outside-staff-padding"), 0.5);

      // When we encounter the first outside-staff grob, make a copy
      // of the current heights to use as an estimate for the staff heights.
      // Note that the outside-staff approximation that we use here doesn't
      // consider any collisions that might occur between outside-staff grobs,
      // but only the fact that outside-staff grobs may need to be raised above
      // the staff.
      if (outside_staff && begin_line_staff_heights.empty ())
        {
          begin_line_staff_heights = begin_line_heights;
          mid_line_staff_heights = mid_line_heights;
        }

      // TODO: consider a pure version of get_grob_direction?
      Direction d = to_dir (g->get_property_data ("direction"));
      d = (d == CENTER) ? UP : d;

      Interval_t<int> rank_span = g->spanned_rank_interval ();
      vsize first_break = lower_bound (ranks, (vsize)rank_span[LEFT], less<vsize> ());
      if (first_break > 0 && ranks[first_break] >= (vsize)rank_span[LEFT])
        first_break--;

      for (vsize j = first_break; j + 1 < ranks.size () && (int)ranks[j] <= rank_span[RIGHT]; ++j)
        {
          int start = ranks[j];
          int end = ranks[j + 1];

          // Take grobs that are visible with respect to a slightly longer line.
          // Otherwise, we will never include grobs at breakpoints which aren't
          // end-of-line-visible.
          int visibility_end = j + 2 < ranks.size () ? ranks[j + 2] : end;

          if (g->pure_is_visible (start, visibility_end))
            {
              Interval dims = g->pure_height (common, start, end);
              if (!dims.is_empty ())
                {
                  if (rank_span[LEFT] <= start)
                    {
                      if (outside_staff)
                        begin_line_heights[j].unite (begin_line_staff_heights[j].union_disjoint (dims, padding, d));
                      else
                        begin_line_heights[j].unite (dims);
                    }
                  if (rank_span[RIGHT] > start)
                    {
                      if (outside_staff)
                        mid_line_heights[j].unite (mid_line_staff_heights[j].union_disjoint (dims, padding, d));
                      else
                        mid_line_heights[j].unite (dims);
                    }
                }
            }
        }
    }

  // Convert begin_line_heights and min_line_heights to SCM.
  SCM begin_scm = scm_c_make_vector (ranks.size () - 1, SCM_EOL);
  SCM mid_scm = scm_c_make_vector (ranks.size () - 1, SCM_EOL);
  for (vsize i = 0; i < begin_line_heights.size (); ++i)
    {
      scm_vector_set_x (begin_scm, scm_from_int (i), ly_interval2scm (begin_line_heights[i]));
      scm_vector_set_x (mid_scm, scm_from_int (i), ly_interval2scm (mid_line_heights[i]));
    }

  return scm_cons (begin_scm, mid_scm);
}

Interval
Axis_group_interface::relative_pure_height (Grob *me, int start, int end)
{
  /* It saves a _lot_ of time if we assume a VerticalAxisGroup is additive
     (ie. height (i, k) = max (height (i, j) height (j, k)) for all i <= j <= k).
     Unfortunately, it isn't always true, particularly if there is a
     VerticalAlignment somewhere in the descendants.

     Usually, the only VerticalAlignment comes from Score. This makes it
     reasonably safe to assume that if our parent is a VerticalAlignment,
     we can assume additivity and cache things nicely. */
  Grob *p = me->get_parent (Y_AXIS);
  if (p && Align_interface::has_interface (p))
    return Axis_group_interface::sum_partial_pure_heights (me, start, end);

  Grob *common = unsmob_grob (me->get_object ("pure-Y-common"));
  extract_grob_set (me, "pure-relevant-grobs", elts);

  Interval r;
  for (vsize i = 0; i < elts.size (); i++)
    {
      Grob *g = elts[i];
      Interval_t<int> rank_span = g->spanned_rank_interval ();
      if (rank_span[LEFT] <= end && rank_span[RIGHT] >= start
          && g->pure_is_visible (start, end)
          && !(to_boolean (g->get_property ("cross-staff"))
               && Stem::has_interface (g)))
        {
          Interval dims = g->pure_height (common, start, end);
          if (!dims.is_empty ())
            r.unite (dims);
        }
    }
  return r;
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, width, 1);
SCM
Axis_group_interface::width (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  return generic_group_extent (me, X_AXIS);
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, height, 1);
SCM
Axis_group_interface::height (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  return generic_group_extent (me, Y_AXIS);
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, pure_height, 3);
SCM
Axis_group_interface::pure_height (SCM smob, SCM start_scm, SCM end_scm)
{
  int start = robust_scm2int (start_scm, 0);
  int end = robust_scm2int (end_scm, INT_MAX);
  Grob *me = unsmob_grob (smob);

  /* Maybe we are in the second pass of a two-pass spacing run. In that
     case, the Y-extent of a system is already given to us */
  System *system = dynamic_cast<System *> (me);
  if (system)
    {
      SCM line_break_details = system->column (start)->get_property ("line-break-system-details");
      SCM system_y_extent = scm_assq (ly_symbol2scm ("system-Y-extent"), line_break_details);
      if (scm_is_pair (system_y_extent))
        return scm_cdr (system_y_extent);
    }

  return ly_interval2scm (pure_group_height (me, start, end));
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, calc_skylines, 1);
SCM
Axis_group_interface::calc_skylines (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  extract_grob_set (me, Grob_array::unsmob (me->get_object ("vertical-skyline-elements")) ? "vertical-skyline-elements" : "elements", elts);
  Skyline_pair skylines = skyline_spacing (me, elts);

  return skylines.smobbed_copy ();
}

/* whereas calc_skylines calculates skylines for axis-groups with a lot of
   visible children, combine_skylines is designed for axis-groups whose only
   children are other axis-groups (ie. VerticalAlignment). Rather than
   calculating all the skylines from scratch, we just merge the skylines
   of the children.
*/
MAKE_SCHEME_CALLBACK (Axis_group_interface, combine_skylines, 1);
SCM
Axis_group_interface::combine_skylines (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  extract_grob_set (me, "elements", elements);
  Grob *y_common = common_refpoint_of_array (elements, me, Y_AXIS);
  Grob *x_common = common_refpoint_of_array (elements, me, X_AXIS);

  if (y_common != me)
    programming_error ("combining skylines that don't belong to me");

  Skyline_pair ret;
  for (vsize i = 0; i < elements.size (); i++)
    {
      SCM skyline_scm = elements[i]->get_property ("vertical-skylines");
      if (Skyline_pair::unsmob (skyline_scm))
        {
          Real offset = elements[i]->relative_coordinate (y_common, Y_AXIS);
          Skyline_pair other = *Skyline_pair::unsmob (skyline_scm);
          other.raise (offset);
          other.shift (elements[i]->relative_coordinate (x_common, X_AXIS));
          ret.merge (other);
        }
    }
  return ret.smobbed_copy ();
}

SCM
Axis_group_interface::generic_group_extent (Grob *me, Axis a)
{
  /* trigger the callback to do skyline-spacing on the children */
  if (a == Y_AXIS)
    (void) me->get_property ("vertical-skylines");

  extract_grob_set (me, "elements", elts);
  Grob *common = common_refpoint_of_array (elts, me, a);

  Real my_coord = me->relative_coordinate (common, a);
  Interval r (relative_group_extent (elts, common, a));

  return ly_interval2scm (r - my_coord);
}

/* This is like generic_group_extent, but it only counts the grobs that
   are children of some other axis-group. This is uncached; if it becomes
   commonly used, it may be necessary to cache it somehow. */
Interval
Axis_group_interface::staff_extent (Grob *me, Grob *refp, Axis ext_a, Grob *staff, Axis parent_a)
{
  extract_grob_set (me, "elements", elts);
  vector<Grob *> new_elts;

  for (vsize i = 0; i < elts.size (); i++)
    if (elts[i]->common_refpoint (staff, parent_a) == staff)
      new_elts.push_back (elts[i]);

  return relative_group_extent (new_elts, refp, ext_a);
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, calc_pure_relevant_grobs, 1);
SCM
Axis_group_interface::calc_pure_relevant_grobs (SCM smob)
{
  Grob *me = unsmob_grob (smob);
  return internal_calc_pure_relevant_grobs (me, "elements");
}

SCM
Axis_group_interface::internal_calc_pure_relevant_grobs (Grob *me, string grob_set_name)
{
  extract_grob_set (me, grob_set_name.c_str (), elts);

  vector<Grob *> relevant_grobs;
  SCM pure_relevant_p = ly_lily_module_constant ("pure-relevant?");

  for (vsize i = 0; i < elts.size (); i++)
    {
      if (to_boolean (scm_apply_1 (pure_relevant_p, elts[i]->self_scm (), SCM_EOL)))
        relevant_grobs.push_back (elts[i]);

      if (Item *it = dynamic_cast<Item *> (elts[i]))
        {
          for (LEFT_and_RIGHT (d))
            {
              Item *piece = it->find_prebroken_piece (d);
              if (piece && to_boolean (scm_apply_1 (pure_relevant_p, piece->self_scm (), SCM_EOL)))
                relevant_grobs.push_back (piece);
            }
        }
    }

  vector_sort (relevant_grobs, pure_staff_priority_less);
  SCM grobs_scm = Grob_array::make_array ();
  unsmob_grob_array (grobs_scm)->set_array (relevant_grobs);

  return grobs_scm;
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, calc_pure_y_common, 1);
SCM
Axis_group_interface::calc_pure_y_common (SCM smob)
{
  Grob *me = unsmob_grob (smob);

  extract_grob_set (me, "pure-relevant-grobs", elts);
  Grob *common = common_refpoint_of_array (elts, me, Y_AXIS);
  if (!common)
    {
      me->programming_error ("No common parent found in calc_pure_y_common.");
      return SCM_EOL;
    }

  return common->self_scm ();
}

SCM
Axis_group_interface::calc_common (Grob *me, Axis axis)
{
  extract_grob_set (me, "elements", elts);
  Grob *common = common_refpoint_of_array (elts, me, axis);
  if (!common)
    {
      me->programming_error ("No common parent found in calc_common axis.");
      return SCM_EOL;
    }

  return common->self_scm ();
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, calc_x_common, 1);
SCM
Axis_group_interface::calc_x_common (SCM grob)
{
  return calc_common (unsmob_grob (grob), X_AXIS);
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, calc_y_common, 1);
SCM
Axis_group_interface::calc_y_common (SCM grob)
{
  return calc_common (unsmob_grob (grob), Y_AXIS);
}

Interval
Axis_group_interface::pure_group_height (Grob *me, int start, int end)
{
  Grob *common = unsmob_grob (me->get_object ("pure-Y-common"));

  if (!common)
    {
      programming_error ("no pure Y common refpoint");
      return Interval ();
    }
  Real my_coord = me->relative_coordinate (common, Y_AXIS);
  Interval r (relative_pure_height (me, start, end));

  return r - my_coord;
}

void
Axis_group_interface::get_children (Grob *me, vector<Grob *> *found)
{
  found->push_back (me);

  if (!has_interface (me))
    return;

  extract_grob_set (me, "elements", elements);
  for (vsize i = 0; i < elements.size (); i++)
    {
      Grob *e = elements[i];
      Axis_group_interface::get_children (e, found);
    }
}

static bool
staff_priority_less (Grob *const &g1, Grob *const &g2)
{
  Real priority_1 = robust_scm2double (g1->get_property ("outside-staff-priority"), -infinity_f);
  Real priority_2 = robust_scm2double (g2->get_property ("outside-staff-priority"), -infinity_f);

  if (priority_1 < priority_2)
    return true;
  else if (priority_1 > priority_2)
    return false;

  /* if neither grob has an outside-staff priority, the ordering will have no
     effect -- we just need to choose a consistent ordering. We do this to
     avoid the side-effect of calculating extents. */
  if (isinf (priority_1))
    return g1 < g2;

  /* if there is no preference in staff priority, choose the left-most one */
  Grob *common = g1->common_refpoint (g2, X_AXIS);
  Real start_1 = g1->extent (common, X_AXIS)[LEFT];
  Real start_2 = g2->extent (common, X_AXIS)[LEFT];
  return start_1 < start_2;
}

static bool
pure_staff_priority_less (Grob *const &g1, Grob *const &g2)
{
  Real priority_1 = robust_scm2double (g1->get_property ("outside-staff-priority"), -infinity_f);
  Real priority_2 = robust_scm2double (g2->get_property ("outside-staff-priority"), -infinity_f);

  return priority_1 < priority_2;
}

static void
add_interior_skylines (Grob *me, Grob *x_common, Grob *y_common, vector<Skyline_pair> *skylines)
{
  if (Grob_array *elements = unsmob_grob_array (me->get_object ("elements")))
    {
      for (vsize i = 0; i < elements->size (); i++)
        add_interior_skylines (elements->grob (i), x_common, y_common, skylines);
    }
  else if (!scm_is_number (me->get_property ("outside-staff-priority"))
      && !to_boolean (me->get_property ("cross-staff")))
    {
      Skyline_pair *maybe_pair = Skyline_pair::unsmob (me->get_property ("vertical-skylines"));
      if (!maybe_pair)
        return;
      if (maybe_pair->is_empty ())
        return;
      skylines->push_back (Skyline_pair (*maybe_pair));
      skylines->back ().shift (me->relative_coordinate (x_common, X_AXIS));
      skylines->back ().raise (me->relative_coordinate (y_common, Y_AXIS));
    }
}

// Raises the grob elt (whose skylines are given by h_skyline
// and v_skyline) so that it doesn't intersect with staff_skyline,
// or with anything in other_h_skylines and other_v_skylines.
void
avoid_outside_staff_collisions (Grob *elt,
                                Skyline_pair const &staff_skyline,
                                Skyline_pair *h_skyline,
                                Skyline_pair *v_skyline,
                                vector<Skyline_pair> const &other_h_skylines,
                                vector<Skyline_pair> const &other_v_skylines,
                                Direction const dir)
{
  Real EPSILON = 0.001;
  bool dirty = false;

  // TODO: by modifying Skyline::smallest_shift to allow a vector of skylines,
  // we can get rid of the do {} while (dirty) loop.
  do
    {
      Real max_bump = dir * staff_skyline[dir].distance ((*v_skyline)[-dir]);
      for (vsize j = 0; j < other_h_skylines.size (); j++)
        {
          Skyline_pair const& h_other = other_h_skylines[j];
          Skyline_pair const& v_other = other_v_skylines[j];

          // In order for one object to avoid the other,
          // it is sufficient for /either/ the horizontal skylines
          // /or/ the vertical skylines to not intersect.
          // Find the smallest shift so that one of these
          // constraints will be satisfied.

          Real bump = h_skyline->smallest_shift (h_other, dir, 0.0, 0.0);
          if (v_skyline->intersects (v_other))
            {
              Real dist = v_other[dir].distance ((*v_skyline)[-dir]);
              bump = dir * min (dir * bump, dist);              
            }

          if (!isinf (bump))
            max_bump = dir * max (dir * bump, dir * max_bump);
        }
          
      dirty = dir * max_bump > EPSILON;
      if (dirty)
        {
          max_bump = max_bump + (dir * EPSILON);
          v_skyline->raise (max_bump);
          h_skyline->shift (max_bump);
          elt->translate_axis (max_bump, Y_AXIS);
        }
    }
  while (dirty);
}

// Shifts the grobs in elements to ensure that they (and any
// connected riders) don't collide with the staff skylines
// or anything in all_X_skylines.  Afterwards, the skylines
// of the grobs in elements will be added to all_h_skylines
// and all_v_skylines.
static void
add_grobs_of_one_priority (Skyline_pair const& skylines,
                           Drul_array<vector<Skyline_pair> >& all_h_skylines,
                           Drul_array<vector<Skyline_pair> >& all_v_skylines,
                           vector<Grob *> const& elements,
                           Grob *x_common,
                           Grob *y_common,
                           multimap<Grob*, Grob*> const& riders)
{
  vector<Box> boxes;
  vector<Skyline_pair> skylines_to_merge;

  for (vsize i = 0; i < elements.size (); i++)
    {
      Grob *elt = elements[i];
      Direction dir = get_grob_direction (elt);
      if (dir == CENTER)
        {
          warning (_ ("an outside-staff object should have a direction, defaulting to up"));
          dir = UP;
        }

      SCM horizon_padding_scm = elt->get_property ("outside-staff-horizontal-padding");
      Real horizon_padding = robust_scm2double (horizon_padding_scm, 0.0);
      Skyline_pair *v_orig = Skyline_pair::unsmob (elt->get_property ("vertical-skylines"));
      if (v_orig->is_empty ())
          continue;

      // Find the riders associated with this grob.
      typedef multimap<Grob*, Grob*>::const_iterator GrobMapIterator;
      pair<GrobMapIterator, GrobMapIterator> range = riders.equal_range (elt);
      vector<Skyline_pair> rider_v_skylines;
      vector<Skyline_pair> rider_h_skylines;
      for (GrobMapIterator j = range.first; j != range.second; j++)
        {
          Grob *rider = j->second;
          Skyline_pair *v_rider = Skyline_pair::unsmob (rider->get_property ("vertical-skylines"));
          Skyline_pair *h_rider = Skyline_pair::unsmob (rider->get_property ("horizontal-skylines"));
          if (v_rider)
            {
              Skyline_pair copy (*v_rider);
              copy.shift (rider->relative_coordinate (x_common, X_AXIS));
              copy.raise (rider->relative_coordinate (y_common, Y_AXIS));
              rider_v_skylines.push_back (copy);
            }

          if (h_rider)
            {
              Skyline_pair copy (*h_rider);
              copy.shift (rider->relative_coordinate (y_common, Y_AXIS));
              copy.raise (rider->relative_coordinate (x_common, X_AXIS));
              rider_h_skylines.push_back (copy);
            }
        }

      // Make a copy that we can modify.
      Skyline_pair v_skylines (*v_orig);
      v_skylines.shift (elt->relative_coordinate (x_common, X_AXIS));
      v_skylines.raise (elt->relative_coordinate (y_common, Y_AXIS));
      v_skylines.merge (Skyline_pair (rider_v_skylines));

      Skyline_pair *h_orig = Skyline_pair::unsmob (elt->get_property ("horizontal-skylines"));
      Skyline_pair h_skylines (*h_orig);
      h_skylines.shift (elt->relative_coordinate (y_common, Y_AXIS));
      h_skylines.raise (elt->relative_coordinate (x_common, X_AXIS));
      h_skylines.merge (Skyline_pair (rider_h_skylines));

      avoid_outside_staff_collisions (elt,
                                      skylines,
                                      &h_skylines,
                                      &v_skylines,
                                      all_h_skylines[dir],
                                      all_v_skylines[dir],
                                      dir);

      elt->set_property ("outside-staff-priority", SCM_BOOL_F);
      all_h_skylines[dir].push_back (h_skylines);
      all_v_skylines[dir].push_back (v_skylines);
    }
}

// If the Grob has a Y-ancestor with outside-staff-priority, return it.
// Otherwise, return 0.
Grob*
Axis_group_interface::outside_staff_ancestor (Grob *me)
{
  Grob *parent = me->get_parent (Y_AXIS);
  if (!parent)
    return 0;

  if (scm_is_number (parent->get_property ("outside-staff-priority")))
    return parent;

  return outside_staff_ancestor (parent);
}

// It is tricky to correctly handle skyline placement of cross-staff grobs.
// For example, cross-staff beams cannot be formatted until the distance between
// staves is known and therefore any grobs that depend on the beam cannot be placed
// until the skylines are known. On the other hand, the distance between staves should
// really depend on position of the cross-staff grobs that lie between them.
// Currently, we just leave cross-staff grobs out of the
// skyline altogether, but this could mean that staves are placed so close together
// that there is no room for the cross-staff grob. It also means, of course, that
// we don't get the benefits of skyline placement for cross-staff grobs.
Skyline_pair
Axis_group_interface::skyline_spacing (Grob *me, vector<Grob *> elements)
{
  /*
    As a sanity check, we make sure that no grob with an outside staff priority
    has a Y-parent that also has an outside staff priority, which would result
    in two movings.
  */
  for (vsize i = 0; i < elements.size (); i++)
    if (scm_is_number (elements[i]->get_property ("outside-staff-priority"))
        && outside_staff_ancestor (elements[i]))
      {
        elements[i]->warning ("Cannot set outside-staff-priority for element and elements' Y parent.");
        elements[i]->set_property ("outside-staff-priority", SCM_BOOL_F);
      }

  /* For grobs with an outside-staff-priority, the sorting function might
     call extent and cause suicide. This breaks the contract that is required
     for the STL sort function. To avoid this, we make sure that any suicides
     are triggered beforehand.
  */
  for (vsize i = 0; i < elements.size (); i++)
    if (scm_is_number (elements[i]->get_property ("outside-staff-priority")))
      elements[i]->extent (elements[i], X_AXIS);

  vector_sort (elements, staff_priority_less);
  Grob *x_common = common_refpoint_of_array (elements, me, X_AXIS);
  Grob *y_common = common_refpoint_of_array (elements, me, Y_AXIS);

  assert (y_common == me);


  // A rider is a grob that is not outside-staff, but has an outside-staff
  // ancestor.  In that case, the rider gets moved along with its ancestor.
  multimap<Grob*,Grob*> riders;

  vsize i = 0;
  vector<Skyline_pair> to_constructor;
  for (i = 0; i < elements.size ()
       && !scm_is_number (elements[i]->get_property ("outside-staff-priority")); i++)
    {
      Grob *elt = elements[i];
      Grob *ancestor = outside_staff_ancestor (elt);
      if (!(to_boolean (elt->get_property ("cross-staff")) || ancestor))
        add_interior_skylines (elt, x_common, y_common, &to_constructor);
      if (ancestor)
        riders.insert (pair<Grob*, Grob*> (ancestor, elt));
    }

  Skyline_pair skylines (to_constructor);

  // These two collection of contain the skylines of all outside-staff grobs
  // that have already been processed.  We keep them around in order to
  // check them for collisions with the currently active outside-staff grob.
  Drul_array<vector<Skyline_pair> > all_h_skylines;
  Drul_array<vector<Skyline_pair> > all_v_skylines;

  for (; i < elements.size (); i++)
    {
      if (to_boolean (elements[i]->get_property ("cross-staff")))
        continue;

      // Collect all the outside-staff grobs that have a particular priority.
      SCM priority = elements[i]->get_property ("outside-staff-priority");
      vector<Grob *> current_elts;
      current_elts.push_back (elements[i]);
      while (i + 1 < elements.size ()
             && scm_is_eq (elements[i + 1]->get_property ("outside-staff-priority"), priority))
        {
          if (!to_boolean (elements[i + 1]->get_property ("cross-staff")))
            current_elts.push_back (elements[i + 1]);
          ++i;
        }

      add_grobs_of_one_priority (skylines,
                                 all_h_skylines,
                                 all_v_skylines,
                                 current_elts,
                                 x_common,
                                 y_common,
                                 riders);
    }

  // Now everything in all_v_skylines has been shifted appropriately; merge
  // them all into skylines to get the complete outline.
  Skyline_pair other_skylines (all_v_skylines[UP]);
  other_skylines.merge (Skyline_pair (all_v_skylines[DOWN]));
  skylines.merge (other_skylines);

  // We began by shifting my skyline to be relative to the common refpoint; now
  // shift it back.
  skylines.shift (-me->relative_coordinate (x_common, X_AXIS));

  return skylines;
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, print, 1)
SCM
Axis_group_interface::print (SCM smob)
{
  if (!debug_skylines)
    return SCM_BOOL_F;

  Grob *me = unsmob_grob (smob);
  Stencil ret;
  if (Skyline_pair *s = Skyline_pair::unsmob (me->get_property ("vertical-skylines")))
    {
      ret.add_stencil (Lookup::points_to_line_stencil (0.1, (*s)[UP].to_points (X_AXIS))
                       .in_color (255, 0, 255));
      ret.add_stencil (Lookup::points_to_line_stencil (0.1, (*s)[DOWN].to_points (X_AXIS))
                       .in_color (0, 255, 255));
    }
  return ret.smobbed_copy ();
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, calc_pure_staff_staff_spacing, 3)
SCM
Axis_group_interface::calc_pure_staff_staff_spacing (SCM smob, SCM start, SCM end)
{
  return calc_maybe_pure_staff_staff_spacing (unsmob_grob (smob),
                                              true,
                                              scm_to_int (start),
                                              scm_to_int (end));
}

MAKE_SCHEME_CALLBACK (Axis_group_interface, calc_staff_staff_spacing, 1)
SCM
Axis_group_interface::calc_staff_staff_spacing (SCM smob)
{
  return calc_maybe_pure_staff_staff_spacing (unsmob_grob (smob),
                                              false,
                                              0,
                                              INT_MAX);
}

SCM
Axis_group_interface::calc_maybe_pure_staff_staff_spacing (Grob *me, bool pure, int start, int end)
{
  Grob *grouper = unsmob_grob (me->get_object ("staff-grouper"));

  if (grouper)
    {
      bool within_group = Staff_grouper_interface::maybe_pure_within_group (grouper, me, pure, start, end);
      if (within_group)
        return grouper->get_maybe_pure_property ("staff-staff-spacing", pure, start, end);
      else
        return grouper->get_maybe_pure_property ("staffgroup-staff-spacing", pure, start, end);
    }
  return me->get_maybe_pure_property ("default-staff-staff-spacing", pure, start, end);
}

Real
Axis_group_interface::minimum_distance (Grob *g1, Grob *g2, Axis a)
{
  SCM sym = ly_symbol2scm ((a == Y_AXIS) ? "vertical-skylines" : "horizontal-skylines");

  Skyline_pair *s1 = Skyline_pair::unsmob (g1->get_property (sym));
  Skyline_pair *s2 = Skyline_pair::unsmob (g2->get_property (sym));
  if (s1 && s2)
    return (*s1)[DOWN].distance ((*s2)[UP]);
  return 0;
}

ADD_INTERFACE (Axis_group_interface,
               "An object that groups other layout objects.",

               // TODO: some of these properties are specific to
               // VerticalAxisGroup. We should split off a
               // vertical-axis-group-interface.
               /* properties */
               "adjacent-pure-heights "
               "axes "
               "bound-alignment-interfaces "
               "default-staff-staff-spacing "
               "elements "
               "max-stretch "
               "no-alignment "
               "nonstaff-nonstaff-spacing "
               "nonstaff-relatedstaff-spacing "
               "nonstaff-unrelatedstaff-spacing "
               "pure-relevant-grobs "
               "pure-relevant-items "
               "pure-relevant-spanners "
               "pure-Y-common "
               "staff-affinity "
               "staff-grouper "
               "staff-staff-spacing "
               "system-Y-offset "
               "vertical-skyline-elements "
               "X-common "
               "Y-common "
              );
