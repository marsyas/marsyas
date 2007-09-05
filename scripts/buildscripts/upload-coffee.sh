#!/bin/sh
### change as necessary
coffeeDir=~/usr/src/marsyas-coffee


VERSION=`cat $coffeeDir/VERSION`
echo "Uploading Marsyas Coffee Dataset $VERSION"
coffeeFile=marsyas-coffee-$VERSION.tar.bz2
cd $coffeeDir
cd ..
tar -cjf $coffeeFile marsyas-coffee
scp $coffeeFile marsyas@marsyas.sness.net:
ssh -l marsyas marsyas.sness.net "mv $coffeeFile marsyas-coffee; cd marsyas-coffee; ln -sf $coffeeFile marsyas-coffee-latest.tar.bz2"

