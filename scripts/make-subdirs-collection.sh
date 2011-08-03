#!/bin/sh
rm -f *.mf

for d in *
do
  if [ -d $d ]
  then
    mkcollection -l $d -c $d $d/
  fi
done

cat *.mf > collection.mf

for d in *.mf
do
  if [ $d != "collection.mf" ]
  then
    rm $d
  fi
done

