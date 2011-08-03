#!/bin/sh
rm -f *.mf

for d in *
do
  if [ -d $d ]
  then
    mkcollection -l $d -c $d $d/
  fi
done

cat *.mf > basic-waves.mf

for d in *.mf
do
  if [ $d != "basic-waves.mf" ]
  then
    rm $d
  fi
done

