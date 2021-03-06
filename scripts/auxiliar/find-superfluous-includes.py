#!/usr/bin/env python
#
# This file is part of LilyPond, the GNU music typesetter.
#
# Copyright (C) 2006--2020 Han-Wen Nienhuys <hanwen@xs4all.nl>
#
# LilyPond is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LilyPond is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.


import sys
import re
import os


full_paths = {}
incs = {}
inc_re = re.compile('^#include "([^"]+)"')


def parse_file(fn):
    lst = []

    lc = 0
    for l in open(fn).readlines():
        lc += 1
        m = inc_re.search(l)
        if m:
            lst.append((lc, m.group(1)))

    base = os.path.split(fn)[1]
    full_paths[base] = fn
    incs[base] = lst


def has_include(f, name):
    try:
        return name in [b for (a, b) in incs[f]]
    except KeyError:
        return False


for a in sys.argv:
    parse_file(a)

print('-*-compilation-*-')
for (f, lst) in list(incs.items()):
    for (n, inc) in lst:
        for (n2, inc2) in lst:
            if has_include(inc2, inc):
                print("%s:%d: already have %s from %s" % (full_paths[f], n,
                                                          inc, inc2))
                break
