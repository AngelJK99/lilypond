/*
   path.cc - manipulation of paths and filenames.
*/

#include "config.h"
#include <stdio.h>
#include <errno.h>
#include <limits.h>

#if HAVE_SYS_STAT_H 
#include <sys/stat.h>
#endif

#ifdef __CYGWIN__
#include <sys/cygwin.h>

// URGURG
#include "../lily/include/scm-option.hh"
#endif

#include "file-path.hh"


#ifndef PATHSEP
#define PATHSEP ':'
#endif

/* We don't have multiple roots, set this to '\0'? */
#ifndef ROOTSEP
#define ROOTSEP ':'
#endif

#ifndef DIRSEP
#define DIRSEP '/'
#endif

#ifndef EXTSEP
#define EXTSEP '.'
#endif



#ifdef __CYGWIN__
static String
dos_to_posix (String path)
{
  char buf[PATH_MAX];
  char *filename = path.get_copy_str0 ();
  /* urg, wtf? char const* argument gets modified! */
  cygwin_conv_to_posix_path (filename, buf);
  delete filename;
  return buf;
}

static String
dos_to_posix_list (String path)
{
  char *filename = path.get_copy_str0 ();
  int len = cygwin_win32_to_posix_path_list_buf_size (filename);
  if (len < PATH_MAX)
    len = PATH_MAX;
  char *buf = new char[len];
  /* urg, wtf? char const* argument gets modified! */
  cygwin_win32_to_posix_path_list (filename, buf);
  delete filename;
  
  String ret = buf;
  delete buf;
  return ret;
}
#endif /* __CYGWIN__ */

/* Join components to full path. */
String
Path::string () const
{
  String s;
  if (!root.empty_b ())
    s = root + to_string (ROOTSEP);
  if (!dir.empty_b ())
    s += dir + to_string (DIRSEP);
  s += base;
  if (!ext.empty_b ())
    s += to_string (EXTSEP) + ext;
  return s;
}

/**
   @param path the original full filename
   @return 4 components of the path. They can be empty
*/
Path
split_path (String path)
{
#ifdef __CYGWIN__
  /* All system functions would work, even if we don't convert to
     posix path, but we'd think that \foe\bar\baz.ly is in the cwd.
     On by default.  */
  if (!(testing_level_global & 1))
    path = dos_to_posix (path);
#endif

  Path p;
  int i = path.index (ROOTSEP);
  if (i >= 0)
    {
      p.root = path.left_string (i);
      path = path.right_string (path.length () - i - 1);
    }

  i = path.index_last (DIRSEP);
  if (i >= 0)
    {
      p.dir = path.left_string (i);
      path = path.right_string (path.length () - i - 1);
    }

  i = path.index_last ('.');
  if (i >= 0)
    {
      p.base = path.left_string (i);
      p.ext = path.right_string (path.length () - i - 1);
    }
  else
    p.base = path;
  return p;
}

void
File_path::parse_path (String p)
{
#ifdef __CYGWIN__
  if (testing_level_global & 4)
    p = dos_to_posix_list (p);
#endif

  int l;
  
  while ((l = p.length ()) )
    {
      int i = p.index (PATHSEP);
      if (i <0) 
	i = l;
      add (p.left_string (i));
      p = p.right_string (l- i - 1);
    }
}




/** Find a file.
  It will search in the current dir, in the construction-arg, and
  in any other added path, in this order.

  @return
  The full path if found, or empty string if not found
  */
String
File_path::find (String nm) const
{
  if (!nm.length () || (nm == "-") )
    return nm;
  for (int i=0; i < size (); i++)
    {
      String path  = elem (i);
      String sep = to_string (DIRSEP);
      String right (path.right_string (1));
      if (path.length () && right != sep)
	path += to_string (DIRSEP);

      path += nm;


#if 0
      /*
	Check if directory. TODO: encapsulate for autoconf
       */
      struct stat sbuf;
      if (stat (path.to_str0 (), &sbuf) != 0)
	continue;
      
      if (! (sbuf.st_mode & __S_IFREG))
	continue;
#endif
#if !STAT_MACROS_BROKEN
      
      struct stat sbuf;
      if (stat (path.to_str0 (), &sbuf) != 0)
	continue;

      if (S_ISDIR (sbuf.st_mode))
	continue;
#endif

      FILE *f = fopen (path.to_str0 (), "r"); // ugh!
      if (f)
	{
	  fclose (f);
	  return path;
	}
    }
  return "";
}

/**
   Add a directory, return false if failed
 */
bool
File_path::try_add (String s)
{
  if (s == "")
    s =  ".";
  FILE  * f = fopen (s.to_str0 (), "r");
  if (!f)
    return false;
  fclose (f);
    
  add (s);
  return true;
}

void
File_path::add (String s)
{
#ifdef __CYGWIN__
  if (testing_level_global & 2)
    s = dos_to_posix (s);
#endif

  push (s);
}

String
File_path::string () const
{
  String s;
  for (int i=0; i< size (); i++)
    {
      s = s + elem (i);
      if (i < size () -1 )
	s += ":";
    }
  return s;
}
