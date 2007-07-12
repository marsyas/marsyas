#!/bin/sh
tar -czf marsyas-docs.tar.gz out-www/*
scp marsyas-docs.tar.gz marsyas@marsyas.sness.net:
ssh -l marsyas marsyas.sness.net "tar -xf marsyas-docs.tar.gz"
rm marsyas-docs.tar.gz

