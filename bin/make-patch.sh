#!/bin/sh

case $# in
0) echo make_patch old new name
	exit 1;;
esac

old=$1
new=$2
nm=$3-

newarc=../releases/$nm$new.tar.gz
oldarc=../releases/$nm$old.tar.gz


if [ ! -x  $nm$new ]
then
   echo untarring ..
  if [ ! -f $newarc ]
  then
	echo "can't find $newarc"
	exit
  fi
  tar zfx $newarc
fi
if [ ! -x  $nm$old ]
then
   echo untarring

  if [ ! -f $oldarc ]
  then
	echo "can't find $oldarc"
	exit
  fi
   tar zfx $oldarc
fi

# not interested in auto generated files.
for a in lilypond.lsm INSTALL.text AUTHORS.text lilypond.spec configure; do
    rm `find  $nm$old $nm$new  -name $a`
done    

cat <<EOF > patch-$new
Generated with 

	make_patch $1 $2 $3

usage 

	cd lilypond-source-dir; patch -E -p0 < patch-$new
	
Patches do not contain automatically generated files, 
i.e. you should rerun configure
EOF

(cd $nm$new; diff -urN  ../$nm$old . >> ../patch-$new)
rm -rf $nm$old $nm$new
