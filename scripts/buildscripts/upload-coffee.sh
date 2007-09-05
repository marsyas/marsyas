#!/bin/sh
coffeeDir=~/usr/src/marsyas-coffee

VERSION=`cat $coffeeDir/VERSION`
echo "Uploading Marsyas Coffee Dataset $VERSION"
coffeeFile=marsyas-coffee-$VERSION.tar.bz2
cd $coffeeDir
cd ..
tar -cjf $coffeeFile marsyas-coffee
scp $coffeeFile marsyas@marsyas.sness.net:
ssh -l marsyas marsyas.sness.net "mv $coffeeFile marsyas-coffee; cd marsyas-coffee; ln -sf $coffeeFile marsyas-coffee-latest.tar.bz2"
#tar -cjf marsyas-docs.tar.gz out-www/
#scp marsyas-docs.tar.gz marsyas@marsyas.sness.net:
#ssh -l marsyas marsyas.sness.net "rm -rf out-www/ ; tar -xf marsyas-docs.tar.gz ; mv marsyas-docs.tar.gz out-www/"

#rm marsyas-docs.tar.gz

