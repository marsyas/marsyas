#!/bin/sh
### change as necessary
marsyasDir=~/usr/src/marsyas/

cd $marsyasDir/doc/
tar -czf marsyas-docs.tar.gz out-www/
scp marsyas-docs.tar.gz marsyas@marsyas.sness.net:
ssh -l marsyas marsyas.sness.net "rm -rf out-www/ ; tar -xf marsyas-docs.tar.gz ; mv marsyas-docs.tar.gz out-www/"

rm marsyas-docs.tar.gz

