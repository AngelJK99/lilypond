#!@PYTHON@
import codecs
import os
import sys
import getopt
import tempfile

# usage:
def usage ():
    print 'usage: bib2texi.py [-s style] [-o <outfile>] [-q] BIBFILES...'
    print '-q suppresses most output'

(options, files) = getopt.getopt (sys.argv[1:], 's:o:hq', [])

output = 'bib.itexi'
style = 'long'
show_output = True

for (o,a) in options:
    if o == '-h' or o == '--help':
        usage ()
        sys.exit (0)
    elif o == '-s' or o == '--style':
        style = a
    elif o == '-o' or o == '--output':
        output = a
    elif o == '-q':
        show_output = False
    else:
        raise Exception ('unknown option: %s' % o)

if not files:
   usage ()
   sys.exit (2)

marker = """@c This file was autogenerated
@c     from: %s
@c     by:   %s

""" % (", ".join(files), sys.argv[0])

def strip_extension (f, ext):
    (p, e) = os.path.splitext (f)
    if e == ext:
        e = ''
    return p + e

nf = []
for f in files:
    nf.append (strip_extension (f, '.bib'))

files = ','.join (nf)

tmpfile = tempfile.mkstemp ('bib2texi')[1]

#This writes a .aux file to the temporary directory.
#The .aux file contains the commands for bibtex
#PEH changed the bibstyle to allow a single template file in the parent directory
#The template filename is texi-*.bst, where * defaults to 'long' but can be a parameter
open (tmpfile + '.aux', 'w').write (r'''
\relax
\citation{*}
\bibstyle{%(style)s}
\bibdata{%(files)s}''' % vars ())

tmpdir = tempfile.gettempdir ()

if (show_output):
    quiet_flag = ''
else:
    quiet_flag = ' -terse '

#The command line to invoke bibtex
cmd = "TEXMFOUTPUT=%s bibtex %s %s" % (tmpdir, quiet_flag, tmpfile)

if (show_output):
    sys.stdout.write ("Running bibtex on %s\n" % files)
    sys.stdout.write (cmd)
#And invoke it
stat = os.system (cmd)
if stat != 0:
    sys.stderr.write ("Bibtex exited with nonzero exit status!")
    sys.exit (1)

#TODO: do tex -> itexi on output
# Following lines copy tmpfile.bbl to the desired output file
bbl = codecs.open (tmpfile + '.bbl', 'r', 'utf-8').read ()

if bbl.strip () == '':
    sys.stderr.write ("Bibtex generated an empty file!")
    sys.exit (1)

fout = codecs.open (output, 'w', 'utf-8')
fout.write (marker)
fout.write (bbl)
fout.close ()

def cleanup (tmpfile):
    for a in ['aux','bbl', 'blg']:
        os.unlink (tmpfile + '.' + a)


cleanup (tmpfile)
#Following line added by PEH - script was leaving a dangling temporary file with no extension
os.unlink (tmpfile)

